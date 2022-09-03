#include "Tapedeck.hpp"

Tapedeck::Tapedeck(int width, int height) {
    setWantsKeyboardFocus(true);
    setSize(width, height);
    tapedeckWindow = std::make_unique<DecoratedWindow>("TAPEDECK", getWidth(), getHeight());
    initialiseSession("test_session");
    
	playhead = std::make_unique<juce::DrawableRectangle>();
    playhead->setFill(juce::Colours::white);
    
    //Initialise main ui components
    addAndMakeVisible(tapedeckWindow.get());
    addAndMakeVisible(trackStack.get());
    addAndMakeVisible(playhead.get());
    addAndMakeVisible(timeRuler.get());

    updatePlayheadPosition();
    selectedTrack = trackStack->subWidgets.front().get();
    selectedTrack->isSelected = true;
    resized();
}

Tapedeck::~Tapedeck() {}

void Tapedeck::initialiseSession(std::string sessionName) {
    this->sessionName = sessionName;
    if (filesystem::exists(sessionName)) {
        // Here we should add check for setting up loading functionality
        // For now to test the project we'll clear out the directory and start from scratch
        filesystem::remove_all(sessionName);
    } 
    audioManager = make_unique<AudioManager>(false, sessionName, "");
    session = audioManager->session;
    session->sessionXml->createXmlDocument();
    timeRuler = make_shared<TimeRuler>(audioManager->params->sampleRate);
    trackStack = make_shared<TrackStack>(timeRuler);
    for (int i = 0; i < 4; i++) {
        createTrack();
    }
}

bool keyStateChanged(bool isKeyDown, juce::Component *originatingComponent) { return true; }

bool Tapedeck::keyPressed(const juce::KeyPress &key) {
    session->sessionXml->refreshXmlDocument(*session->playhead, session->tracks);

    /// Flip bool for selected track in case it changes
    if (selectedTrack != nullptr) {
        selectedTrack->isSelected = false;
    }
    if (selectedClip != nullptr) {
        selectedClip->isSelected = false;
    }

    // QUIT
    if (key.getKeyCode() == 'q' && key.getModifiers() == juce::ModifierKeys::ctrlModifier) {
        closeUIThread = true;
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }
    if (key == juce::KeyPress::upKey) {
        selectPreviousTrack();
        selectedClip = nullptr;
    }
    if (key == juce::KeyPress::downKey) {
        selectNextTrack();
        selectedClip = nullptr;
    }
    if (key == juce::KeyPress::leftKey) {
        if (selectedTrack->clipWidgets.size() > 0) {
            selectPreviousClip();
        }
    }
    if (key == juce::KeyPress::rightKey) {
        if (selectedTrack->clipWidgets.size() > 0) {
            selectNextClip();
        }
    }
    if (key == juce::KeyPress::leftKey && key.getModifiers() == juce::ModifierKeys::ctrlModifier) {
        if (selectedClip != nullptr) {
            if (selectedClip->clipStartTime - timeRuler->windowSizeInSamples * 0.125 >= 0) {
                selectedClip->clipStartTime -= timeRuler->windowSizeInSamples * 0.125;
            } else {
                selectedClip->clipStartTime = 0;
            }
        }
    }
    if (key == juce::KeyPress::rightKey && key.getModifiers() == juce::ModifierKeys::ctrlModifier) {
        if (selectedClip != nullptr) {
            selectedClip->clipStartTime += timeRuler->windowSizeInSamples * 0.125;
        }
    }
    if (key == juce::KeyPress::leftKey && key.getModifiers() == juce::ModifierKeys::shiftModifier) {
        if (selectedClip != nullptr) {
            if ((int)selectedClip->clipStartTime - 500 >= 0) {
                selectedClip->clipStartTime -= 500;
            } else {
                selectedClip->clipStartTime = 0;
            }
        }
    }
    if (key == juce::KeyPress::rightKey && key.getModifiers() == juce::ModifierKeys::shiftModifier) {
        if (selectedClip != nullptr) {
            selectedClip->clipStartTime += 500;
        }
    }
    if (key.getKeyCode() == ',') {
        // scrub_backward
        session->playhead->movePlayhead(-0.5f);
        updatePlayheadPosition();
        retreatClipWindow();
    }
    if (key.getKeyCode() == '.') {
        // scrub_forward
        session->playhead->movePlayhead(0.5f);
        updatePlayheadPosition();
        advanceClipWindow();
    }
    if (key.getKeyCode() == ',' && key.getModifiers() == juce::ModifierKeys::shiftModifier) {
        // big_scrub_backward
        session->playhead->movePlayhead((-0.5f * 4.0f));
        updatePlayheadPosition();
        retreatClipWindow();
    }
    if (key.getKeyCode() == '.' && key.getModifiers() == juce::ModifierKeys::shiftModifier) {
        // big_scrub_forward
        session->playhead->movePlayhead((0.5f * 4.0f));
        updatePlayheadPosition();
        advanceClipWindow();
    }
    if (key.getKeyCode() == '[') {
        // zoom_out
        timeRuler->cellsPerSecond *= 0.75;
    }
    if (key.getKeyCode() == ']') {
        // zoom_in
        timeRuler->cellsPerSecond *= 1.25;
    }
    if (key.getKeyCode() == '=') {
        createTrack();
    }
    if (key.getKeyCode() == '-') {
        removeTrack();
    }
    if (key.getTextCharacter() == 's') {
        // toggle_track_solo
        selectedTrack->track->solo = !selectedTrack->track->solo;
        selectedTrack->refresh();
    }
    if (key.getTextCharacter() == 'm') {
        // toggle_track_mute
        selectedTrack->track->mute = !selectedTrack->track->mute;
        selectedTrack->refresh();
    }
    if (key.getTextCharacter() == 'r') {
        // toggle_track_armed
        selectedTrack->track->recordArmed = !selectedTrack->track->recordArmed;
        selectedTrack->refresh();
    }
    if (key == juce::KeyPress::spaceKey) {
        // play/pause
        if (audioManager) {
            if (isPlaying) {
                audioManager->stopAudioStream();
                isPlaying = false;
                trackStack->process();
                stopTimer();
            } else {
                audioManager->startAudioStream();
                isPlaying = true;
                startTimerHz(30);
            }
        }
    }
    if (selectedTrack != nullptr) {
        selectedTrack->isSelected = true;
    } else if (trackStack->subWidgets.size() > 0) {
        selectedTrack = trackStack->subWidgets[0].get();
        selectedTrack->isSelected = true;
    }
    if (selectedClip != nullptr) {
        selectedClip->isSelected = true;
    }
    repaint();
    return true;
}
void Tapedeck::selectPreviousTrack() {
    for (int i = 0; i < trackStack->subWidgets.size(); i++) {
        if (trackStack->subWidgets[i].get() == selectedTrack) {
            if (i != 0) {
                selectedTrack = trackStack->subWidgets[i - 1].get();
            } else {
                selectedTrack = trackStack->subWidgets.back().get();
                break;
            }
        }
    }
}
void Tapedeck::selectNextTrack() {
    for (int i = 0; i < trackStack->subWidgets.size(); i++) {
        if (trackStack->subWidgets[i].get() == selectedTrack) {
            if (i + 1 == trackStack->subWidgets.size()) {
                selectedTrack = trackStack->subWidgets.front().get();
            } else {
                selectedTrack = trackStack->subWidgets[i + 1].get();
                break;
            }
        }
    }
}
void Tapedeck::selectPreviousClip() {
    for (int i = 0; i < selectedTrack->clipWidgets.size(); i++) {
        if (selectedTrack->clipWidgets[i].get() == selectedClip) {
            if (i != 0) {
                selectedClip = selectedTrack->clipWidgets[i - 1].get();
            }
        }
    }
}
void Tapedeck::selectNextClip() {
    for (int i = 0; i < selectedTrack->clipWidgets.size(); i++) {
        if (selectedTrack->clipWidgets[i].get() == selectedClip) {
            if (i + 1 != selectedTrack->clipWidgets.size()) {
                selectedClip = selectedTrack->clipWidgets[i + 1].get();
                break;
            }
        }
    }
    if (selectedClip == nullptr) {
        selectedClip = selectedTrack->clipWidgets[0].get();
    }
}
void Tapedeck::advanceClipWindow() {
    if (session->getCurrentTimeInSamples() > timeRuler->startTimeOnScreenInSamples + timeRuler->windowSizeInSamples) {
        timeRuler->startTimeOnScreenInSamples = session->getCurrentTimeInSamples();
    }
}
void Tapedeck::retreatClipWindow() {
    if (session->getCurrentTimeInSamples() < timeRuler->startTimeOnScreenInSamples) {
        timeRuler->startTimeOnScreenInSamples = session->getCurrentTimeInSamples() - timeRuler->windowSizeInSamples;
        if (timeRuler->startTimeOnScreenInSamples < 0) {
            timeRuler->startTimeOnScreenInSamples = 0;
        }
    }
}
void Tapedeck::createTrack() {
    session->createTrack();
    trackStack->createTrackSubWidgets(session->tracks.back());
    if (trackStack->subWidgets.size() == 1) {
        selectedTrack = trackStack->subWidgets.back().get();
    }
    trackStack->resized();
}
void Tapedeck::removeTrack() {
    std::vector<std::unique_ptr<TrackWidget>>::iterator widgetIterator = trackStack->subWidgets.begin();
    for (int i = 0; i < trackStack->subWidgets.size(); i++) {
        if (selectedTrack == trackStack->subWidgets[i].get()) {
            selectedTrack = nullptr;
            advance(widgetIterator, i);
            trackStack->subWidgets.erase(widgetIterator);
            session->deleteTrack(i);
        }
    }
    trackStack->resized();
}
void Tapedeck::paint(juce::Graphics &screen) {

    // topBar.draw(Point(2, 1), "Key Commands:");
    // topBar.draw(Point(2, 3), "Create/Delete track    '='/'-'");
    // topBar.draw(Point(2, 4), "Navigate tracks        'Up'/'Down'");
    // topBar.draw(Point(2, 5), "Scrub through time     ','/'.'");
    // topBar.draw(Point(40, 3), "Record Arm Selected Track    'R'");
    // topBar.draw(Point(40, 4), "Solo Selected Track          'S'");
    // topBar.draw(Point(40, 5), "Mute Selected Track          'M'");
    // topBar.draw(Point(78, 3), "Playback/Record/Stop Audio    'Space'");
    // topBar.draw(Point(78, 4), "Zoom out/in                   '['/']'");
    // topBar.draw(Point(78, 4), "Quit Tapedeck                 'Ctrl-Q'");
}

void Tapedeck::resized() {
    if (tapedeckWindow) {
        tapedeckWindow->setBounds(getBounds());
        trackStack->setBounds(getWidth() * 0.01, getHeight() * 0.2, getWidth() * 0.98, getHeight() * 0.75);
        timeRuler->setBounds(trackStack->getX() + (trackStack->getWidth() * 0.1), trackStack->getY() - (getHeight() * 0.03), getWidth() * 0.9, getHeight() * 0.1);
        updatePlayheadPosition();
    }
}

void Tapedeck::timerCallback() {
    if (playhead->getX() != playheadXPosition()) {
        updatePlayheadPosition();
    }
}

int Tapedeck::playheadXPosition() {
    int x_pos = trackStack->timeRuler->getX();
    x_pos += (session->getCurrentTimeInSamples() - timeRuler->startTimeOnScreenInSamples) / timeRuler->samplesPerCell();
    if (x_pos >= getWidth() - (getWidth() * 0.9 * 0.5) && isPlaying) {
        x_pos = getWidth() - (getWidth() * 0.9 * 0.5);
        updateClipWindow();
    }
    return x_pos;
}
void Tapedeck::updatePlayheadPosition() {
    juce::Rectangle<float> r = { (float)playheadXPosition(), (float)trackStack->getY(), 2.0f, (float)trackStack->getHeight() };
    playhead->setRectangle(juce::Parallelogram<float>(r));
}
void Tapedeck::updateClipWindow() {
    timeRuler->startTimeOnScreenInSamples = session->getCurrentTimeInSamples() - timeRuler->windowSizeInSamples / 2;
}