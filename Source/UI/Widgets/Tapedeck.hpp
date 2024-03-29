
#include "../../Model/AudioManager.hpp"
#include "../../defs_UI.h"
#include "DecoratedWindow.hpp"
#include "TimeRuler.hpp"
#include "TrackStack.hpp"
#include "LevelMeter.h"

#include <iostream>
#include <optional>
#include <string>
#include <array>

class Tapedeck : public juce::Component 
{
public:
    Tapedeck(int width, int height, int sampleRate, std::shared_ptr<juce::ApplicationCommandManager> cmdManager);
    ~Tapedeck();
    
   
    void createTrackWidget(std::shared_ptr<Track> track);
    void removeTrack();
    void updateTrackStack();
    /** Draws all elements on the main screen */
    void paint(juce::Graphics &screen) override;
    void resized() override;
    bool closeUIThread = false;
    
    std::vector<std::shared_ptr<TrackWidget>> getSelectedTracks();
    std::shared_ptr<TrackStack> getTrackStack();
    LevelMeter masterMeter[2] = {LevelMeter(), LevelMeter()};

private:

    std::unique_ptr<DecoratedWindow> tapedeckWindow;
    std::shared_ptr<TrackStack> trackStack;
    std::shared_ptr<TimeRuler> timeRuler;
    std::shared_ptr<juce::ApplicationCommandManager> cmdManager;
    std::string sessionName;
};
