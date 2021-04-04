#ifndef _SESSION_H_
#define _SESSION_H_

#include "Audio_Params.hpp"
#include "Playhead.hpp"
#include "Track.hpp"
#include "Xml_Wrapper.hpp"
#include "wave_file_generator/Wave_File_Generator.hpp"
#include <assert.h>
#include <filesystem>

class Session {
public:
    Session(std::string session_name, Audio_Params params);
    ~Session();

    void create_project_file_structure();
    void load_all_audio_clips();
    void clear_all_audio_clips();

    void create_track();
    void delete_track(int index);

    void prepare_audio();
    void process_audio_block(double *input_buffer, double *output_buffer);
    bool is_solo_enabled();

    void create_files_from_recorded_clips();

    float get_current_time_in_seconds();
    u_int get_current_time_in_samples();

    void load_audio_clip(Clip &clip, int start_time_in_reference, int clip_length);
    static double bytes_to_double(unsigned char first_byte, unsigned char second_byte, double max_amplitude);

    void load_session_from_xml(string xml_file_name);
    void load_tracks(int number_of_tracks);
    void load_clips(int number_of_clips);
    int num_record_armed_tracks();

    enum struct Play_State {
        ToPlay,
        Playing,
        Recording,
        Stopping,
        Stopped,
    };

    Play_State play_state = Play_State::Stopped;
    std::vector<Track> tracks;
    Wave_File_Generator wav_gen;
    std::unique_ptr<Xml_Wrapper> session_xml;
    Playhead playhead;

private:
    u_int sample_rate = 0, buffer_size = 0,
          num_input_channels = 0, num_output_channels = 0, bit_depth = 16;
    ofstream wav_file_streamer;
    std::string session_name;
    bool is_solo_enabled_tracks = false;
};
#endif