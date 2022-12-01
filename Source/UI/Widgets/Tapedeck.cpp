#include "Tapedeck.hpp"

Tapedeck::Tapedeck(int width, int height, int sampleRate, std::shared_ptr<juce::ApplicationCommandManager> cmdManager) {
    setSize(width, height);
    tapedeckWindow = std::make_unique<DecoratedWindow>("TAPEDECK", getWidth(), getHeight());
    tapedeckWindow->backgroundColour = ColourPalette::colourGreyDark;
    tapedeckWindow->outlineColour = ColourPalette::colourTextDark;
    
    trackStack = make_shared<TrackStack>(sampleRate, cmdManager);
    trackStack->setName("Track Stack");
    this->cmdManager = cmdManager;
    
    //Initialise main ui components
    addAndMakeVisible(tapedeckWindow.get());
    addAndMakeVisible(trackStack.get());

    //updatePlayheadPosition();
    /*selectedTrack = trackStack->subWidgets.front().get();
    selectedTrack->isSelected = true;*/
    resized();
}

Tapedeck::~Tapedeck() {}


void Tapedeck::updateTrackStack()
{
    trackStack->process();
}

std::vector<std::shared_ptr<TrackWidget>> Tapedeck::getSelectedTracks()
{
    std::vector<std::shared_ptr<TrackWidget>> selectedTracks;
    for (auto &trackWidget : trackStack->trackWidgets) {
        if (trackWidget->track->isSelected) {
            selectedTracks.push_back(trackWidget);
        }
    }
    return selectedTracks;
}
std::shared_ptr<TrackStack> Tapedeck::getTrackStack()
{
    return trackStack;
}
//void Tapedeck::selectPreviousTrack()
//{
//    for (int i = 0; i < trackStack->subWidgets.size(); i++) {
//        if (trackStack->subWidgets[i].get() == selectedTrack) {
//            if (i != 0) {
//                selectedTrack = trackStack->subWidgets[i - 1].get();
//            } else {
//                selectedTrack = trackStack->subWidgets.back().get();
//                break;
//            }
//        }
//    }
//}
//
//void Tapedeck::selectNextTrack() {
//    for (int i = 0; i < trackStack->subWidgets.size(); i++) {
//        if (trackStack->subWidgets[i].get() == selectedTrack) {
//            if (i + 1 == trackStack->subWidgets.size()) {
//                selectedTrack = trackStack->subWidgets.front().get();
//            } else {
//                selectedTrack = trackStack->subWidgets[i + 1].get();
//                break;
//            }
//        }
//    }
//}
//void Tapedeck::selectPreviousClip() {
//    for (int i = 0; i < selectedTrack->clipWidgets.size(); i++) {
//        if (selectedTrack->clipWidgets[i].get() == selectedClip) {
//            if (i != 0) {
//                selectedClip = selectedTrack->clipWidgets[i - 1].get();
//            }
//        }
//    }
//}
//void Tapedeck::selectNextClip() {
//    for (int i = 0; i < selectedTrack->clipWidgets.size(); i++) {
//        if (selectedTrack->clipWidgets[i].get() == selectedClip) {
//            if (i + 1 != selectedTrack->clipWidgets.size()) {
//                selectedClip = selectedTrack->clipWidgets[i + 1].get();
//                break;
//            }
//        }
//    }
//    if (selectedClip == nullptr) {
//        selectedClip = selectedTrack->clipWidgets[0].get();
//    }
//}
void Tapedeck::advanceClipWindow() {
    /*if (session->getCurrentTimeInSamples() > timeRuler->startTimeOnScreenInSamples + timeRuler->windowSizeInSamples) {
        timeRuler->startTimeOnScreenInSamples = session->getCurrentTimeInSamples();
    }*/
}
void Tapedeck::retreatClipWindow() {
    /*if (session->getCurrentTimeInSamples() < timeRuler->startTimeOnScreenInSamples) {
        timeRuler->startTimeOnScreenInSamples = session->getCurrentTimeInSamples() - timeRuler->windowSizeInSamples;
        if (timeRuler->startTimeOnScreenInSamples < 0) {
            timeRuler->startTimeOnScreenInSamples = 0;
        }
    }*/
}
void Tapedeck::createTrackWidget(std::shared_ptr<Track> track) {
    trackStack->createTrackWidget(track);
}
void Tapedeck::removeTrack() {
    /*std::vector<std::unique_ptr<TrackWidget>>::iterator widgetIterator = trackStack->subWidgets.begin();
    for (int i = 0; i < trackStack->subWidgets.size(); i++) {
        if (selectedTrack == trackStack->subWidgets[i].get()) {
            selectedTrack = nullptr;
            advance(widgetIterator, i);
            trackStack->subWidgets.erase(widgetIterator);
            session->deleteTrack(i);
        }
    }
    trackStack->resized();*/
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
    }
}



