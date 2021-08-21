#pragma once
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

struct TapeDeck : Widget {
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
    bool closeUIThread = false;

    TapeDeck() {
        DecoratedWindow dw = DecoratedWindow("TAPEDECK");
        this->tapedeckWindow = dw;
        initialise_session("test_session");
        selectedTrack = trackStack->subWidgets.front().get();
        selectedTrack->is_selected = true;
    }

    void initialise_session(std::string sessionName) {
        this->sessionName = sessionName;
        if (filesystem::exists(sessionName)) {
            filesystem::remove_all(sessionName);
        }
        audioManager = make_unique<AudioManager>(false, sessionName, "");
        session = audioManager->session;
        session->sessionXml->createXmlDocument();
        timeRuler = make_shared<TimeRuler>(audioManager->params->sampleRate);
        trackStack = make_shared<TrackStack>(timeRuler);
        for (int i = 0; i < 4; i++) {
            create_track();
        }
    }
    //Manages all cases of keyboard input, needs a better method than nested if statements
    void process(std::vector<KeyPress> &keyboardInput) {
        session->sessionXml->refreshXmlDocument(*session->playhead, session->tracks);
        if (keyboardInput.size() > 0) {
            /// Flip bool for selected track in case it changes
            if (selectedTrack != nullptr) {
                selectedTrack->is_selected = false;
            }
            if (selectedClip != nullptr) {
                selectedClip->is_selected = false;
            }
            
            KeyPress last = keyboardInput.back();
            // QUIT
            if (last == KeyPress(Key::K_Q, ModifierKey::Control)) {
                closeUIThread = true;
                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_UpArrow)) {
                select_previous_track();
                selectedClip = nullptr;
                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_DownArrow)) {
                select_next_track();
                selectedClip = nullptr;
                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_LeftArrow)) {
                if (selectedTrack->clipWidgets.size() > 0) {
                    select_previous_clip();
                }
                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_RightArrow)) {
                if (selectedTrack->clipWidgets.size() > 0) {
                    select_next_clip();
                }
                keyboardInput.pop_back();
            }

            if (last == KeyPress(Key::K_LeftArrow, ModifierKey::Control)) {
                if (selectedClip != nullptr) {
                    if (*selectedClip->clipStartTime - timeRuler->windowSizeInSamples * 0.125 >= 0) {
                        *selectedClip->clipStartTime -= timeRuler->windowSizeInSamples * 0.125;
                    } else {
                        *selectedClip->clipStartTime = 0;
                    }
                }

                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_RightArrow, ModifierKey::Control)) {
                if (selectedClip != nullptr) {
                    *selectedClip->clipStartTime += timeRuler->windowSizeInSamples * 0.125;
                }
                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_LeftArrow, ModifierKey::Shift)) {
                if (selectedClip != nullptr) {
                    if ((int)*selectedClip->clipStartTime - 500 >= 0) {
                        *selectedClip->clipStartTime -= 500;
                    } else {
                        *selectedClip->clipStartTime = 0;
                    }
                }

                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_RightArrow, ModifierKey::Shift)) {
                if (selectedClip != nullptr) {
                    *selectedClip->clipStartTime += 500;
                }
                keyboardInput.pop_back();
            }

            if (last == KeyPress(Key::K_Comma)) {
                // scrub_backward
                session->playhead->movePlayhead(-0.5f);
                retreat_clip_window();
                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_Period)) {
                // scrub_forward
                session->playhead->movePlayhead(0.5f);
                advance_clip_window();
                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_Comma, ModifierKey::Shift)) {
                // big_scrub_backward
                session->playhead->movePlayhead((-0.5f * 4.0f));
                retreat_clip_window();
                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_Period, ModifierKey::Shift)) {
                // big_scrub_forward
                session->playhead->movePlayhead((0.5f * 4.0f));
                advance_clip_window();
                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_BracketOpen)) {
                // zoom_out
                timeRuler->cellsPerSecond *= 0.75;
                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_BracketClose)) {
                // zoom_in
                timeRuler->cellsPerSecond *= 1.25;

                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_Equal)) {
                create_track();
                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_Minus)) {
                remove_track();
                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_S)) {
                // toggle_track_solo
                selectedTrack->track->solo = !selectedTrack->track->solo;
                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_M)) {
                // toggle_track_mute
                selectedTrack->track->mute = !selectedTrack->track->mute;
                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_R)) {
                // toggle_track_armed
                selectedTrack->track->recordArmed = !selectedTrack->track->recordArmed;
                keyboardInput.pop_back();
            }
            if (last == KeyPress(Key::K_Space)) {
                // play_pause
                if (isPlaying) {
                    audioManager->stopAudioStream();
                    isPlaying = false;
                    trackStack->process(keyboardInput);
                } else {
                    audioManager->startAudioStream();
                    isPlaying = true;
                }
                keyboardInput.pop_back();
            }

            if (selectedTrack != nullptr) {
                selectedTrack->is_selected = true;
            } else if (trackStack->subWidgets.size() > 0) {
                selectedTrack = trackStack->subWidgets[0].get();
                selectedTrack->is_selected = true;
            }
            if (selectedClip != nullptr) {
                selectedClip->is_selected = true;
            }
        }
    }
    void select_previous_track() {
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

    void select_next_track() {
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
    void select_previous_clip() {
        for (int i = 0; i < selectedTrack->clipWidgets.size(); i++) {
            if (selectedTrack->clipWidgets[i].get() == selectedClip) {
                if (i != 0) {
                    selectedClip = selectedTrack->clipWidgets[i - 1].get();
                }
            }
        }
    }
    void select_next_clip() {
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
    void advance_clip_window() {
        if (session->getCurrentTimeInSamples() > timeRuler->startTimeOnScreenInSamples + timeRuler->windowSizeInSamples) {
            timeRuler->startTimeOnScreenInSamples = session->getCurrentTimeInSamples();
        }
    }
    void retreat_clip_window() {
        if (session->getCurrentTimeInSamples() < timeRuler->startTimeOnScreenInSamples) {
            timeRuler->startTimeOnScreenInSamples = session->getCurrentTimeInSamples() - timeRuler->windowSizeInSamples;
            if (timeRuler->startTimeOnScreenInSamples < 0) {
                timeRuler->startTimeOnScreenInSamples = 0;
            }
        }
    }
    void create_track() {
        session->createTrack();
        trackStack->createTrackSubWidgets(session->tracks.back());
        if (trackStack->subWidgets.size() == 1) {
            selectedTrack = trackStack->subWidgets.back().get();
        }
    }

    void remove_track() {
        std::vector<std::unique_ptr<TrackWidget>>::iterator widgetIterator = trackStack->subWidgets.begin();
        for (int i = 0; i < trackStack->subWidgets.size(); i++) {
            if (selectedTrack == trackStack->subWidgets[i].get()) {
                selectedTrack = nullptr;
                advance(widgetIterator, i);
                trackStack->subWidgets.erase(widgetIterator);
                session->deleteTrack(i);
            }
        }
    }

    void render(Screen &screen) {
        Screen topBar = {screen.width - 2, screen.height * 0.2 - 2};
        Screen timeRulerScreen = {screen.width * 0.9, 3};
        Screen trackScreen = {screen.width - 2, screen.height * 0.8 - 2};
        Screen playheadScreen = {1, trackScreen.height};

        this->tapedeckWindow.render(screen);
        timeRuler->render(timeRulerScreen);
        trackStack->render(trackScreen);
        playheadWidget.render(playheadScreen);

        topBar.draw(Point(2, 1), "Key Commands:");
        topBar.draw(Point(2, 3), "Create/Delete track    '='/'-'");
        topBar.draw(Point(2, 4), "Navigate tracks        'Up'/'Down'");
        topBar.draw(Point(2, 5), "Scrub through time     ','/'.'");
        topBar.draw(Point(40, 3), "Record Arm Selected Track    'R'");
        topBar.draw(Point(40, 4), "Solo Selected Track          'S'");
        topBar.draw(Point(40, 5), "Mute Selected Track          'M'");
        topBar.draw(Point(78, 3), "Playback/Record/Stop Audio    'Space'");
        topBar.draw(Point(78, 4), "Zoom out/in                   '['/']'");
        topBar.draw(Point(78, 4), "Quit Tapedeck                 'Ctrl-Q'");

        trackScreen.draw(Point(playhead_x_position(trackScreen), 0), playheadScreen);
        screen.draw(Point(1, 1), topBar);
        screen.draw(Point(screen.width * 0.1, screen.height * 0.2 - 1), timeRulerScreen);
        screen.draw(Point(1, screen.height * 0.2 + 1), trackScreen);
    }

    int playhead_x_position(Screen trackScreen) {
        int x_pos = trackScreen.width * 0.1;
        x_pos += (session->getCurrentTimeInSamples() - timeRuler->startTimeOnScreenInSamples) / timeRuler->samplesPerCell();
        if (x_pos >= trackScreen.width - (trackScreen.width * 0.9 * 0.5) && isPlaying) {
            x_pos = trackScreen.width - (trackScreen.width * 0.9 * 0.5);
            update_clip_window();
        }
        return x_pos;
    }

    void update_clip_window() {
        timeRuler->startTimeOnScreenInSamples = session->getCurrentTimeInSamples() - timeRuler->windowSizeInSamples / 2;
    }
};
