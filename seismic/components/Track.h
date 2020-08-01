#pragma once
#ifndef _TRACK_H_
#define _TRACK_H_

#include "Clip.h"

class Track {
public:
    Track(std::string project_name, std::string track_name) {
        this->project_name = project_name;
        this->track_name = track_name;
    }

    void setName(std::string track_name) { this->track_name = track_name; }

    string getName() { return track_name; }

    void createClip(u_int start_time) {
        int clipNum = (int)clips.size() + 1;
        clips.push_back(Clip(project_name, track_name + "_clip" + std::to_string(clipNum), start_time));
    }

    void eraseClip(int clip_index) {
        std::vector<Clip>::iterator it = clips.begin();
        advance(it, clip_index);
        clips.erase(it);
    }

    double getSample(int channel, int time_in_samples, int max_amplitude) {
        for (auto &clip : clips) {
            if (clip.getStartTime() <= time_in_samples && time_in_samples <= clip.getEndTime()) {
                //As audio is interleaved in wav, we must select sample from appropriate channel
                int sample_start = (time_in_samples - clip.getStartTime()) * 2;
                if (channel == 0) {
                    return clip.getSample(sample_start, max_amplitude);

                } else if (channel == 1) {
                    return clip.getSample(sample_start + 1, max_amplitude);
                }
            }
        }
        return 0;
    }

    std::vector<Clip> clips = {};
    bool is_record_enabled = true;

private:
    std::string track_name, project_name;
};

#endif