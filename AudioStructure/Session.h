#ifndef _SESSION_H_
#define _SESSION_H_

#include "Track.h"

class Session {
public:
    Session(int sample_rate, int buffer_size);
    void createTrack();
    void deleteTrack(int index);
    void prepareAudio();
    void processBlock(double *input_buffer, double *output_buffer);

    enum Play_State {
        Play,
        Stop,
        Pause,
        Record
    };

    Play_State play_state = Stop;
    std::vector<Track> tracks;
    std::vector<int> record_armed_tracks = {};

private:
    double current_time = 0.0;
    int sample_rate = 0, buffer_size = 0;
};
#endif