
#include "../../Model/AudioManager.hpp"
#include "../../defs_UI.h"
#include "DecoratedWindow.hpp"
#include "TimeRuler.hpp"
#include "TrackStack.hpp"

#include <iostream>
#include <optional>
#include <string>

class Tapedeck : public juce::Component 
{
public:
    Tapedeck(int width, int height, int sampleRate, std::shared_ptr<juce::ApplicationCommandManager> cmdManager);
    ~Tapedeck();
    
    void selectPreviousTrack();
    void selectNextTrack();
    void selectPreviousClip();
    void selectNextClip();
    void advanceClipWindow();
    void retreatClipWindow();
    void createTrackWidget(std::shared_ptr<Track> track);
    void removeTrack();
    void updateTrackStack();
    /** Draws all elements on the main screen */
    void paint(juce::Graphics &screen) override;
    void resized() override;
    int playheadXPosition(int currentTimeSamples);
    void updateClipWindow();
    void updatePlayheadPosition(int currentTimeSamples);
    bool closeUIThread = false;
    std::vector<std::shared_ptr<TrackWidget>> getSelectedTracks();
    std::shared_ptr<TrackStack> getTrackStack();

private:

    std::unique_ptr<DecoratedWindow> tapedeckWindow;
    std::shared_ptr<TrackStack> trackStack;
    std::shared_ptr<TimeRuler> timeRuler;
    std::shared_ptr<juce::ApplicationCommandManager> cmdManager;
    std::string sessionName;
    //std::shared_ptr<Session> session;
};
