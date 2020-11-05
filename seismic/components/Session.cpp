#include "Session.hpp"
Session::Session(std::string session_name, Audio_Params params) {
    this->session_name = session_name;
    this->sample_rate = params.sample_rate;
    this->buffer_size = params.buffer_size;
    this->num_input_channels = params.num_input_channels;
    this->num_output_channels = params.num_output_channels;
    wav_gen.initialise(sample_rate, bit_depth, num_input_channels);
    std::string xml_file_name = this->session_name + ".xml";
    session_xml = std::make_unique<Xml_Wrapper>(this->session_name, xml_file_name);
    //loads audio clips with data from .wav files - this allows for faster reading of audio data
    //as the data is being read from stack as opposed to disk
}

Session::Session(std::string project_name, u_int sample_rate, u_int buffer_size, u_int num_input_channels, u_int num_output_channels) {
    this->session_name = project_name;
    this->sample_rate = sample_rate;
    this->buffer_size = buffer_size;
    this->num_input_channels = num_input_channels;
    this->num_output_channels = num_output_channels;
    wav_gen.initialise(sample_rate, bit_depth, num_output_channels);
}
Session::~Session() {
    //Clears the Audio clips from temp memory
    clear_all_audio_clips();
}

//Do I put the load functions in the XMLWrapper?

void Session::load_session_from_xml(string xml_file_name) {
    session_xml->xml_doc.LoadFile(session_xml->file_name.c_str());
    session_xml->session_node = session_xml->xml_doc.RootElement();
    session_xml->playhead_element = session_xml->session_node->FirstChildElement();
    load_playhead();
    load_tracks(session_xml->playhead_element->FindAttribute("number_of_tracks")->IntValue());
    session_xml->xml_doc.SaveFile(session_xml->file_name.c_str());
}
void Session::load_playhead() {
    playhead.movePlayhead(session_xml->playhead_element->FindAttribute("current_time_samples")->IntValue());
    playhead.tempo = session_xml->playhead_element->FindAttribute("tempo")->IntValue();
    playhead.time_sig_num = session_xml->playhead_element->FindAttribute("time_sig_numerator")->IntValue();
    playhead.time_sig_denom = session_xml->playhead_element->FindAttribute("time_sig_denominator")->IntValue();
}
void Session::load_tracks(int number_of_tracks) {
    session_xml->track_element = session_xml->playhead_element->NextSiblingElement();
    for (int i = 0; i < number_of_tracks; i++) {
        create_track();
        load_clips(session_xml->track_element->FindAttribute("number_of_clips")->IntValue());
        session_xml->track_element = session_xml->track_element->NextSiblingElement();
    }
}
void Session::load_clips(int number_of_clips) {
    session_xml->clip_element = session_xml->track_element->FirstChildElement();
    for (int i = 0; i < number_of_clips; i++) {
        tracks.back().create_clip(session_xml->clip_element->FindAttribute("start_time_in_session")->IntValue(), (session_name + "/recorded_audio/"));
        tracks.back().clips[i].start_time_in_reference = session_xml->clip_element->FindAttribute("start_time_in_reference")->IntValue();
        tracks.back().clips[i].reference_file_path = session_xml->clip_element->FindAttribute("reference_file_path")->Value();
        load_audio_clip(tracks.back().clips[i], tracks.back().clips[i].start_time_in_reference, session_xml->clip_element->FindAttribute("length_in_samples")->IntValue());
        session_xml->clip_element = session_xml->clip_element->NextSiblingElement();
    }
}

void Session::load_audio_clip(Clip &clip, int start_time_in_reference, int clip_length) {
    if (!clip.audio_data.empty()) {
        clip.clear();
    }
    for (int i = 0; i < clip_length; i++) {
        int first_sample_index = 44;
        //each audio sample is 2 bytes therefore index is multiplied by 2 to get byte index of sample
        int sample_pos = first_sample_index + start_time_in_reference + (i * 2);
        //as samples are stored as little endian, both bytes within sample must be retrieved individually
        char first_byte, second_byte;
        ifstream reference_file;
        reference_file.open(clip.reference_file_path, ios::binary);
        reference_file.seekg(sample_pos, ios::beg);
        reference_file.read(&first_byte, 1);
        reference_file.read(&second_byte, 1);
        reference_file.close();
        //sample data is then converted to its original double between 1.0 and -1.0
        clip.audio_data.push_back(bytes_to_double(first_byte, second_byte, wav_gen.getMaxAmplitude()));
    }
}

double Session::bytes_to_double(unsigned char first_byte, unsigned char second_byte, double max_amplitude) {
    // convert two bytes to one 16-bit int (little endian)
    int16_t output = (second_byte << 8) | first_byte;
    // convert to range from -1 to (just below) 1
    return output / max_amplitude;
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
            track.create_clip(playhead.current_time_in_samples, (session_name + "/recorded_audio/"));
        }
    }
}
void Session::process_audio_block(double *input_buffer, double *output_buffer) {
    for (int sample = 0; sample < buffer_size; sample++, playhead.current_time_in_samples++, input_buffer++) {
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
}

void Session::create_files_from_recorded_clips() {
    assert(play_state == Play_State::Stopping);
    for (auto &track : tracks) {
        if (track.record_armed) {
            for (auto &clip : track.clips) {
                if (clip.size() > 0) {
                    wav_file_streamer.open(clip.reference_file_path, ios::binary);
                    wav_gen.openWaveFile(wav_file_streamer);
                    for (int i = 0; i < clip.size(); i++) {
                        wav_gen.writeInputToFile(wav_file_streamer, clip.get_sample(i).value);
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
    return playhead.current_time_in_samples;
}

float Session::get_current_time_in_seconds() {
    return 1.0f * playhead.current_time_in_samples / sample_rate;
}
