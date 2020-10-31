#pragma once
#ifndef _TRACK_H_
#define _TRACK_H_

#include "Clip.h"

class Track {
public:
    Track(std::string session_name, std::string track_name) {
        this->session_name = session_name;
        this->track_name = track_name;
    }

    void setName(std::string track_name) { this->track_name = track_name; }

    string getName() { return track_name; }

    void createClip(u_int start_time) {
        int clipNum = (int)clips.size() + 1;
        clips.push_back(Clip(session_name, track_name + "_clip" + std::to_string(clipNum), start_time));
    }

    void eraseClip(int clip_index) {
        std::vector<Clip>::iterator it = clips.begin();
        advance(it, clip_index);
        clips.erase(it);
    }

    double getSample(int time_in_samples, int max_amplitude) {
        for (auto &clip : clips) {
            if (clip.start_time_in_session <= time_in_samples && time_in_samples <= (clip.start_time_in_session + clip.length)) {
                int sample_start = time_in_samples - clip.start_time_in_session;
                return clip.audio_stream[sample_start];
            }
        }
        return 0;
    }

    std::vector<Clip> clips = {};
    bool is_record_enabled = false, is_solo = false, is_mute = false;

private:
    std::string track_name, session_name;
};

#endif