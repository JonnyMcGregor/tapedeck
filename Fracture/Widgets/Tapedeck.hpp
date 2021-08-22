#ifndef _TAPEDECK_H_
#define _TAPEDECK_H_

#include "../../Seismic/AudioManager.hpp"
#include "../Components/Point.hpp"
#include "../Components/Widget.hpp"
#include "DecoratedWindow.hpp"
#include "PlayheadWidget.hpp"
#include "TimeRuler.hpp"
#include "TrackStack.hpp"

#include <iostream>
#include <optional>
#include <string>

class Tapedeck : public Widget 
{
public:
    Tapedeck();
    ~Tapedeck();
    // Sets up the session wih a given session name
    void initialise_session(std::string sessionName);
    
    //Manages all cases of keyboard input
    void process(std::vector<KeyPress> &keyboardInput);

    void select_previous_track();
    void select_next_track();
    void select_previous_clip();
    void select_next_clip();
    void advance_clip_window();
    void retreat_clip_window();
    void create_track();
    void remove_track();
    /** Draws all elements on the main screen */
    void render(Screen &screen);
    int playhead_x_position(Screen trackScreen);
    void update_clip_window();

    bool closeUIThread = false;

private:

    DecoratedWindow tapedeckWindow;
    std::shared_ptr<TrackStack> trackStack;
    std::shared_ptr<TimeRuler> timeRuler;
    PlayheadWidget playheadWidget;
    TrackWidget *selectedTrack = nullptr;
    ClipWidget *selectedClip = nullptr;
    std::string sessionName;
    std::unique_ptr<AudioManager> audioManager;
    std::shared_ptr<Session> session;
    bool isPlaying = false;
};
#endif