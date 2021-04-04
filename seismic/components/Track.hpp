#pragma once
#ifndef _TRACK_H_
#define _TRACK_H_

#include "Clip.hpp"
struct ClipMetadata {
    u_int start_time, start_time_in_reference = 0;
    std::string reference_file_path;
    ClipMetadata(u_int start_time, std::string reference_file_path) {
        this->start_time = start_time;
        this->reference_file_path = reference_file_path;
    };
};
struct Track {
    std::vector<Clip> clips;
    std::vector<ClipMetadata> clip_metadata;
    std::string name; // a user-provided name for display purposes
    bool record_armed = false, solo = false, mute = false;

    Track() {
    }

    void create_clip(u_int start_time, std::string reference_file_path) {
        std::string clip_name = name + "Clip" + std::to_string(clips.size() + 1);
        reference_file_path = reference_file_path + clip_name + ".wav";
        this->clips.push_back(Clip());
        this->clip_metadata.push_back(ClipMetadata(start_time, reference_file_path));
    }

    void create_clip(u_int start_time, std::string reference_file_path, Clip &clip)
    {
        std::string clip_name = name + "Clip" + std::to_string(clips.size() + 1);
        reference_file_path = reference_file_path + clip_name + ".wav";
        this->clips.push_back(clip);
        this->clip_metadata.push_back(ClipMetadata(start_time, reference_file_path));
    }

    void delete_clip(int clip_index) {
        std::vector<Clip>::iterator it = clips.begin();
        advance(it, clip_index);
        clips.erase(it);
    }

    void move_clip(u_int index, u_int new_start_time) {
        clip_metadata[index].start_time = new_start_time;
    }

    Sample get_sample(int sample_index) {
        for (int i = 0; i < this->clips.size(); i++) {
            u_int clip_start_time = clip_metadata[i].start_time;
            u_int clip_end_time = clip_start_time + this->clips.at(i).size();
            if (clip_start_time <= sample_index && sample_index <= clip_end_time) {
                return this->clips.at(i).get_sample(sample_index - clip_start_time);
            }
        }
        return Sample(0);
    }
};
#endif