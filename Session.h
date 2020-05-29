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
    void prepareClip() {
        for (int i = 0; i < tracks.size(); i++) {
            if (tracks[i].is_record_enabled) {
                tracks[i].createClip(current_time);
            }
        }
    }
    void processBlock(double *input_buffer) {
        for (int i = 0; i < tracks.size(); i++) {
            if (tracks[i].is_record_enabled) {
                for (int sample = 0; sample < buffer_size; sample++) {
                    for (int channel = 0; channel < 2; channel++, *input_buffer++)
                        tracks[i].clips.back().addSample(*input_buffer);
                }
            }
        }
    }
    enum play_state {
        Play,
        Stop,
        Pause,
        Record
    };

    std::vector<Track> tracks;

private:
    double current_time = 0.0;
    int sample_rate, buffer_size;
};
