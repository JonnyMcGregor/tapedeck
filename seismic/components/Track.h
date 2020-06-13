#pragma once
#ifndef _TRACK_H_
#define _TRACK_H_

#include "Clip.h"
#include <iostream>
#include <stdlib.h>
#include <string>

class Track {
public:
    Track(std::string track_name) { this->track_name = track_name; }

    void setName(std::string track_name) { this->track_name = track_name; }

    void createClip(u_int start_time) {
        int clipNum = ((int)clips.size()) + 1;
        clips.push_back(Clip(track_name + "_clip" + std::to_string(clipNum), start_time));
    }

    void eraseClip(int clip_index) {
        std::vector<Clip>::iterator it = clips.begin();
        advance(it, clip_index);
        clips.erase(it);
    }

    double getSample(int time_in_samples) {
        for (auto &clip : clips) {
            if (clip.getStartTime() <= time_in_samples && time_in_samples <= clip.getEndTime()) {
                return clip.getSample(time_in_samples - clip.getStartTime());
            } else
                return 0;
        }
    }

    std::vector<Clip> clips = {};
    bool is_record_enabled = true;

private:
    std::string track_name;
};

#endif