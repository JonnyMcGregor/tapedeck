#ifndef _SESSION_H_
#define _SESSION_H_

#include "../wave_file_generator/WaveFileGenerator.h"
#include "Track.h"
#include <experimental/filesystem>

using namespace experimental;

class Session {
public:
    Session(u_int sample_rate, u_int buffer_size, u_int num_input_channels, u_int num_output_channels);

    void createTrack();
    void deleteTrack(int index);

    void prepareAudio();
    void processAudioBlock(double *input_buffer, double *output_buffer);
    void recordProcessing(double *input_buffer, double &output_sample, Track &track);
    void createFilesFromRecordedClips();

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
    u_int current_time = 0, sample_rate = 0, buffer_size = 0,
          num_input_channels = 0, num_output_channels = 0, bit_depth = 16;
    WaveFileGenerator wav_gen;
    ofstream wav_file_streamer;
};
#endif