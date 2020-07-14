#include "Session.h"

Session::Session(u_int sample_rate, u_int buffer_size, u_int num_input_channels, u_int num_output_channels) {
    this->sample_rate = sample_rate;
    this->buffer_size = buffer_size;
    this->num_input_channels = num_input_channels;
    this->num_output_channels = num_output_channels;
    wav_gen.initialise(sample_rate, bit_depth, num_output_channels);

    if (!filesystem::exists("recorded_audio")) {
        filesystem::create_directory("recorded_audio");
    }
}
void Session::createTrack() {
    tracks.push_back(Track("track" + std::to_string(tracks.size() + 1)));
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
    current_time = 0;
    for (auto &track : tracks) {
        if (track.is_record_enabled) {
            track.createClip(current_time);
            record_armed_tracks.push_back(&track);
        }
    }
}

void Session::processAudioBlock(double *input_buffer, double *output_buffer) {
    //Record Input
    for (int sample = 0; sample < buffer_size; sample++, current_time++) {
        for (int channel = 0; channel < 2; channel++, *input_buffer++, *output_buffer++) {
            double output_sample = 0;
            if (play_state == Play_State::Recording) {
                for (auto &track : tracks) {
                    recordProcessing(channel, input_buffer, output_sample, track);
                }
            } else {
                for (auto &track : tracks) {
                    output_sample += track.getSample(channel, current_time, wav_gen.getMaxAmplitude());
                }
                *output_buffer = output_sample;
            }
        }
    }
}
void Session::recordProcessing(int channel, double *input_buffer, double &output_sample, Track &track) {
    //input
    if (track.is_record_enabled) {
        track.clips.back().addSample(*input_buffer);
    }
    //output
    else {
        output_sample += track.getSample(channel, current_time, wav_gen.getMaxAmplitude());
    }
}

void Session::createFilesFromRecordedClips() {
    assert(play_state == Play_State::Stopping);
    for (auto track : record_armed_tracks) {
        for (auto &clip : track->clips) {
            if (clip.getNumSamples() > 0) {
                wav_file_streamer.open(clip.getReferenceFilePath(), ios::binary);
                wav_gen.openWaveFile(wav_file_streamer);
                for (int i = 0; i < clip.getNumSamples(); i++) {
                    wav_gen.writeInputToFile(wav_file_streamer, clip.write_audio_stream[i]);
                }
                wav_gen.closeWaveFile(wav_file_streamer);
                wav_file_streamer.close();
                clip.clearAudioStream();
            }
        }
    }
}