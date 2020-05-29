#ifndef _SESSION_H_
#define _SESSION_H_

#include "Track.h"
#include <stdlib.h>
#include <vector>

class Session {
public:
    Session(int current_sample_rate, int block_size) {
        sample_rate = current_sample_rate;
        buffer_size = block_size;
    }
    void createTrack() {
        tracks.push_back(Track("track" + std::to_string(tracks.size() + 1)));
    }
    void prepareAudio() {
        for (int i = 0; i < tracks.size(); i++) {
            if (tracks[i].is_record_enabled) {
                tracks[i].createClip(current_time);
                record_armed_tracks.push_back(i);
            }
        }
    }
    void processBlock(double *input_buffer, double *output_buffer) {
        //Record Input
        for (auto record_track : record_armed_tracks) {
            for (int sample = 0; sample < buffer_size; sample++) {
                for (int channel = 0; channel < 2; channel++, *input_buffer++)
                    tracks[record_track].clips.back().addSample(*input_buffer);
            }
        }
    }

    enum Play_State {
        Play,
        Stop,
        Pause,
        Record
    };

    Play_State play_state = Record;
    std::vector<Track> tracks;

private:
    double current_time = 0.0;
    int sample_rate = 0, buffer_size = 0;
    std::vector<int> record_armed_tracks = {};
};
#endif