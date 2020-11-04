#ifndef _SESSION_H_
#define _SESSION_H_

#include "Playhead.hpp"
#include "SeismicParams.hpp"
#include "Track.hpp"
#include "XmlWrapper.hpp"
#include "wave_file_generator/WaveFileGenerator.hpp"
#include <assert.h>
#include <experimental/filesystem>
using namespace experimental;

class Session {
public:
    Session(std::string session_name, SeismicParams params);
    Session(std::string session_name, u_int sample_rate, u_int buffer_size, u_int num_input_channels, u_int num_output_channels);
    ~Session();

    void loadAllAudioClips();
    void clearAllAudioClips();

    void createTrack();
    void deleteTrack(int index);

    void prepareAudio();
    void processAudioBlock(double *input_buffer, double *output_buffer);
    bool is_solo_enabled();
    void limitOutputSample(double &output_sample);

    void createFilesFromRecordedClips();

    float getCurrentTimeInSeconds();
    u_int getCurrentTimeInSamples();

    void loadAudioClip(Clip &clip, int start_time_in_reference, int clip_length);
    static double bytesToDouble(unsigned char first_byte, unsigned char second_byte, double max_amplitude);

    void loadSessionFromXML(string xml_file_name);
    void loadPlayhead();
    void loadTracks(int number_of_tracks);
    void loadClips(int number_of_clips);
    int numRecordArmedTracks();

    enum struct Play_State {
        ToPlay,
        Playing,
        Recording,
        Stopping,
        Stopped,
    };

    Play_State play_state = Play_State::Stopped;
    std::vector<Track> tracks;
    WaveFileGenerator wav_gen;
    std::unique_ptr<XmlWrapper> session_xml;
    Playhead playhead;

private:
    u_int sample_rate = 0, buffer_size = 0,
          num_input_channels = 0, num_output_channels = 0, bit_depth = 16;
    ofstream wav_file_streamer;
    std::string session_name;
    bool is_solo_enabled_tracks = false;
};
#endif