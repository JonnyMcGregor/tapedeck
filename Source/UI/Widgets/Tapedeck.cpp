#include "Tapedeck.hpp"

Tapedeck::Tapedeck(int width, int height, int sampleRate, std::shared_ptr<juce::ApplicationCommandManager> cmdManager) {
    setSize(width, height);
    tapedeckWindow = std::make_unique<DecoratedWindow>("TAPEDECK", getWidth(), getHeight());
    tapedeckWindow->backgroundColour = ColourPalette::mainBackground;
    tapedeckWindow->outlineColour = ColourPalette::textColour;
    
    trackStack = make_shared<TrackStack>(sampleRate, cmdManager);
    trackStack->setName("Track Stack");
    this->cmdManager = cmdManager;
    
    //Initialise main ui components
    addAndMakeVisible(tapedeckWindow.get());
    addAndMakeVisible(trackStack.get());
    addAndMakeVisible(&masterMeter[0]);
    addAndMakeVisible(&masterMeter[1]);

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

}

void Tapedeck::resized() {
    if (tapedeckWindow) {
        tapedeckWindow->setBounds(getBounds());
        trackStack->setBounds(15, getHeight() * 0.2, getWidth() - 75, getHeight() * 0.75);
        masterMeter[0].setBounds(getWidth() - 50, trackStack->getBottom() - 200, 15, 200);
        masterMeter[1].setBounds(getWidth() - 30, trackStack->getBottom() - 200, 15, 200);
    }
}



