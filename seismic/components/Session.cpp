#include "Session.h"
Session::Session(std::string session_name, SeismicParams params) {
    this->session_name = session_name;
    this->sample_rate = params.sample_rate;
    this->buffer_size = params.buffer_size;
    this->num_input_channels = params.num_input_channels;
    this->num_output_channels = params.num_output_channels;
    wav_gen.initialise(sample_rate, bit_depth, num_input_channels);
    std::string xml_file_name = this->session_name + ".xml";
    session_xml = std::make_unique<XmlWrapper>(this->session_name, xml_file_name);
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
    clearAllAudioClips();
}
void Session::loadAllAudioClips() {
    for (auto &track : tracks) {
        for (auto &clip : track.clips) {
            loadAudioClip(clip);
        }
    }
}

void Session::loadAudioClip(Clip &clip) {
    if (!clip.audio_stream.empty()) {
        clip.audio_stream.clear();
    }
    for (int i = 0; i < clip.length; i++) {
        int first_sample_index = 44;
        //each audio sample is 2 bytes therefore index is multiplied by 2 to get byte index of sample
        int sample_pos = first_sample_index + clip.start_time_in_reference + (i * 2);
        //as samples are stored as little endian, both bytes within sample must be retrieved individually
        char first_byte, second_byte;
        clip.reference_file_stream.open(clip.reference_file_path, ios::binary);
        clip.reference_file_stream.seekg(sample_pos, ios::beg);
        clip.reference_file_stream.read(&first_byte, 1);
        clip.reference_file_stream.read(&second_byte, 1);
        clip.reference_file_stream.close();
        //sample data is then converted to its original double between 1.0 and -1.0
        clip.audio_stream.push_back(bytesToDouble(first_byte, second_byte, wav_gen.getMaxAmplitude()));
    }
}

double Session::bytesToDouble(unsigned char first_byte, unsigned char second_byte, double max_amplitude) {
    // convert two bytes to one 16-bit int (little endian)
    int16_t output = (second_byte << 8) | first_byte;
    // convert to range from -1 to (just below) 1
    return output / max_amplitude;
}

void Session::loadSessionFromXML(string xml_file_name) {
    session_xml->xml_doc.LoadFile(session_xml->file_name.c_str());
    session_xml->session_node = session_xml->xml_doc.RootElement();
    session_xml->playhead_element = session_xml->session_node->FirstChildElement();
    loadPlayhead();
    loadTracks(session_xml->playhead_element->FindAttribute("number_of_tracks")->IntValue());
    session_xml->xml_doc.SaveFile(session_xml->file_name.c_str());
}

void Session::loadPlayhead() {
    playhead.movePlayhead(session_xml->playhead_element->FindAttribute("current_time_samples")->IntValue());
    playhead.tempo = session_xml->playhead_element->FindAttribute("tempo")->IntValue();
    playhead.time_sig_num = session_xml->playhead_element->FindAttribute("time_sig_numerator")->IntValue();
    playhead.time_sig_denom = session_xml->playhead_element->FindAttribute("time_sig_denominator")->IntValue();
}
void Session::loadTracks(int number_of_tracks) {
    session_xml->track_element = session_xml->playhead_element->NextSiblingElement();
    for (int i = 0; i < number_of_tracks; i++) {
        createTrack();
        loadClips(session_xml->track_element->FindAttribute("number_of_clips")->IntValue());
        session_xml->track_element = session_xml->track_element->NextSiblingElement();
    }
}
void Session::loadClips(int number_of_clips) {
    session_xml->clip_element = session_xml->track_element->FirstChildElement();

    for (int j = 0; j < number_of_clips; j++) {
        tracks.back().createClip(session_xml->clip_element->FindAttribute("start_time_in_session")->IntValue());
        tracks.back().clips[j].length = session_xml->clip_element->FindAttribute("length_in_samples")->IntValue();
        tracks.back().clips[j].reference_file_path = session_xml->clip_element->FindAttribute("reference_file_path")->Value();
        session_xml->clip_element = session_xml->clip_element->NextSiblingElement();
    }
}

void Session::clearAllAudioClips() {
    for (auto &track : tracks) {
        for (auto &clip : track.clips) {
            clip.audio_stream.clear();
        }
    }
}

void Session::createTrack() {
    tracks.push_back(Track(session_name, "track" + std::to_string(tracks.size() + 1)));
}

void Session::deleteTrack(int index) {
    std::vector<Track *>::iterator track_ptr_iterator = record_armed_tracks.begin();
    for (int i = 0; i < record_armed_tracks.size(); i++, track_ptr_iterator++) {
        if (&tracks[index] == record_armed_tracks[i]) {
            record_armed_tracks.erase(track_ptr_iterator);
        }
    }
    std::vector<Track>::iterator track_iterator = tracks.begin();
    advance(track_iterator, index);
    tracks.erase(track_iterator);
}

void Session::prepareAudio() {
    record_armed_tracks.clear();
    for (auto &track : tracks) {
        if (track.is_record_enabled) {
            track.createClip(playhead.current_time_in_samples);
            record_armed_tracks.push_back(&track);
        }
    }
}
void Session::processAudioBlock(double *input_buffer, double *output_buffer) {
    //Record Input
    for (int sample = 0; sample < buffer_size; sample++, playhead.current_time_in_samples++, *input_buffer++) {
        for (int channel = 0; channel < 2; channel++, *output_buffer++) {
            double output_sample = 0;
            for (auto &track : tracks) {
                //input
                if (play_state == Play_State::Recording && track.is_record_enabled && channel == 0) {
                    track.clips.back().addSample(*input_buffer);
                }
                //output
                else {
                    output_sample += track.getSample(getCurrentTimeInSamples(), wav_gen.getMaxAmplitude());
                    limitOutputSample(output_sample);
                }
            }
            *output_buffer = output_sample;
        }
    }
}

void Session::limitOutputSample(double &output_sample) {
    output_sample = output_sample * 0.5;
    if (output_sample >= 1) {
        output_sample = 0.999;
    } else if (output_sample <= -1) {
        output_sample = -0.999;
    }
}
void Session::createFilesFromRecordedClips() {
    assert(play_state == Play_State::Stopping);
    for (auto track : record_armed_tracks) {
        for (auto &clip : track->clips) {
            if (clip.length > 0) {
                wav_file_streamer.open(clip.reference_file_path, ios::binary);
                wav_gen.openWaveFile(wav_file_streamer);
                for (int i = 0; i < clip.length; i++) {
                    wav_gen.writeInputToFile(wav_file_streamer, clip.audio_stream[i]);
                }
                wav_gen.closeWaveFile(wav_file_streamer);
                wav_file_streamer.close();
            }
        }
    }
}

u_int Session::getCurrentTimeInSamples() {
    return playhead.current_time_in_samples;
}

float Session::getCurrentTimeInSeconds() {
    return 1.0f * playhead.current_time_in_samples / sample_rate;
}
