#ifndef _SESSION_H_
#define _SESSION_H_

#include "Track.h"

class Session {
public:
    Session(u_int sample_rate, u_int buffer_size);
    void createTrack();
    void deleteTrack(int index);
    void prepareAudio();
    void playAudio();
    void processAudioBlock(double *input_buffer, double *output_buffer);

    enum struct Play_State {
        ToPlay,
        Playing,
        Recording,
        Stopping,
        Stopped,
    };

    Play_State play_state = Play_State::Stopped;
    std::vector<Track> tracks;
    std::vector<Track *> record_armed_tracks = {};

private:
    u_int current_time = 0;
    int sample_rate = 0, buffer_size = 0;
};
#endif