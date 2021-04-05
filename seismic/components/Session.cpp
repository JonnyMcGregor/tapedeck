#include "Session.hpp"
Session::Session(std::string session_name, Audio_Params params) {
    
    this->session_name = session_name;
    this->sample_rate = params.sample_rate;
    this->buffer_size = params.buffer_size;
    this->num_input_channels = params.num_input_channels;
    this->num_output_channels = params.num_output_channels;
    wav_gen.initialise(sample_rate, bit_depth, num_input_channels);
    playhead = std::make_unique<Playhead>(sample_rate);
    create_project_file_structure();
    std::string xml_file_name = this->session_name + ".xml";
    session_xml = std::make_unique<Xml_Wrapper>(this->session_name, xml_file_name);
}

Session::~Session() {
    //Clears the Audio clips from temp memory
    clear_all_audio_clips();
}
void Session::create_project_file_structure() {
    if (!filesystem::exists(session_name))
        filesystem::create_directory(session_name);
    if (!filesystem::exists(session_name + "/recorded_audio"))
        filesystem::create_directory(session_name + "/recorded_audio");
    if (!filesystem::exists(session_name + "/exported_audio"))
        filesystem::create_directory(session_name + "/exported_audio");
}
void Session::load_session_from_xml(string xml_file_name) {
    session_xml = make_unique<Xml_Wrapper>(this->session_name, xml_file_name);
    session_xml->xml_doc.LoadFile(session_xml->file_name.c_str());
    session_xml->set_elements();
    session_xml->load_playhead_data(*playhead);
    tracks = session_xml->load_tracks();
    session_xml->xml_doc.SaveFile(session_xml->file_name.c_str());
}

void Session::clear_all_audio_clips() {
    for (auto &track : tracks) {
        for (auto &clip : track.clips) {
            clip.audio_data.clear();
        }
    }
}

void Session::create_track() {
    tracks.push_back(Track());
    tracks.back().name = ("Track" + std::to_string(tracks.size()));
}

void Session::delete_track(int index) {
    std::vector<Track>::iterator track_iterator = tracks.begin();
    advance(track_iterator, index);
    tracks.erase(track_iterator);
}

void Session::prepare_audio() {
    for (auto &track : tracks) {
        if (track.record_armed) {
            track.create_clip(playhead->current_time_in_samples, (session_name + "/recorded_audio/"));
        }
    }
}
void Session::process_audio_block(double *input_buffer, double *output_buffer) {
    for (int sample = 0; sample < buffer_size; sample++, playhead->current_time_in_samples++, input_buffer++) {
        for (int channel = 0; channel < 2; channel++, output_buffer++) {
            Sample output_sample;
            //when solo enabled is true all non-solo'd tracks are ignored
            for (Track &track : tracks) {
                //input
                if (play_state == Play_State::Recording && track.record_armed && channel == 0) {
                    track.clips.back().append_sample(*input_buffer);
                }
                //output
                else {
                    if (track.solo || (!is_solo_enabled() && !track.mute)) {
                        output_sample += track.get_sample(get_current_time_in_samples());
                    }
                }
            }
            *output_buffer = output_sample.value * 0.5;
        }
    }
}

bool Session::is_solo_enabled() {
    for (auto &track : tracks) {
        if (track.solo) {
            return true;
        }
    }
    return false;
}

void Session::create_files_from_recorded_clips() {
    assert(play_state == Play_State::Stopping);
    for (auto &track : tracks) {
        if (track.record_armed) {
            for (int i = 0; i < track.clips.size(); i++) {
                if (track.clips[i].size() > 0) {
                    wav_file_streamer.open(track.clip_metadata[i].reference_file_path, ios::binary);
                    wav_gen.openWaveFile(wav_file_streamer);
                    for (int j = 0; j < track.clips[i].size(); j++) {
                        wav_gen.writeInputToFile(wav_file_streamer, track.clips[i].get_sample(j).value);
                    }
                    wav_gen.closeWaveFile(wav_file_streamer);
                    wav_file_streamer.close();
                }
            }
        }
    }
}

int Session::num_record_armed_tracks() {
    int num_record_armed_tracks = 0;
    for (int i = 0; i < tracks.size(); i++) {
        if (tracks[i].record_armed) {
            num_record_armed_tracks++;
        }
    }
    return num_record_armed_tracks;
}
u_int Session::get_current_time_in_samples() {
    return playhead->current_time_in_samples;
}

float Session::get_current_time_in_seconds() {
    return 1.0f * playhead->current_time_in_samples / sample_rate;
}
