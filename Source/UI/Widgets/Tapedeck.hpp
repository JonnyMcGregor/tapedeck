
#include "../../Model/AudioManager.hpp"
#include "../../defs_UI.h"
#include "DecoratedWindow.hpp"
#include "TimeRuler.hpp"
#include "TrackStack.hpp"

#include <iostream>
#include <optional>
#include <string>

class Tapedeck : public juce::Component, private juce::Timer 
{
public:
    Tapedeck(int width, int height);
    ~Tapedeck();
    // Sets up the session wih a given session name
    void initialiseSession(std::string sessionName);

    //Manages all cases of keyboard input
    bool keyPressed(const juce::KeyPress &key) override;

    void selectPreviousTrack();
    void selectNextTrack();
    void selectPreviousClip();
    void selectNextClip();
    void advanceClipWindow();
    void retreatClipWindow();
    void createTrack();
    void removeTrack();
    /** Draws all elements on the main screen */
    void paint(juce::Graphics &screen) override;
    void resized() override;
    int playheadXPosition();
    void updateClipWindow();
    void updatePlayheadPosition();
    bool closeUIThread = false;

private:
    void timerCallback() override;

    std::unique_ptr<DecoratedWindow> tapedeckWindow;
    std::shared_ptr<TrackStack> trackStack;
    std::shared_ptr<TimeRuler> timeRuler;
    std::unique_ptr<juce::DrawableRectangle> playhead;

    TrackWidget *selectedTrack = nullptr;
    ClipWidget *selectedClip = nullptr;
    std::string sessionName;
    std::unique_ptr<AudioManager> audioManager;
    std::shared_ptr<Session> session;
    bool isPlaying = false;
};
