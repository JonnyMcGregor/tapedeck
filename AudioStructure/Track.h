#pragma once
#ifndef _TRACK_H_
#define _TRACK_H_

#include "Clip.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

class Track {
public:
    Track(std::string name) { track_name = name; }

    void setName(std::string new_name) { track_name = new_name; }

    void createClip(double start_time) {
        int clipNum = ((int)clips.size()) + 1;
        clips.push_back(Clip(track_name + "_clip" + std::to_string(clipNum), start_time));
    }

    void eraseClip(int clip_index) {
        std::vector<Clip>::iterator it = clips.begin();
        advance(it, clip_index);
        clips.erase(it);
    }

    std::vector<Clip> clips = {};
    bool is_record_enabled = true;

private:
    std::string track_name;
};

#endif