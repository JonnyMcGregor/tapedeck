#include "Session.h"
Session::Session(std::string project_name, SeismicParams params) {
    this->project_name = project_name;
    this->sample_rate = params.sample_rate;
    this->buffer_size = params.buffer_size;
    this->num_input_channels = params.num_input_channels;
    this->num_output_channels = params.num_output_channels;
    wav_gen.initialise(sample_rate, bit_depth, num_output_channels);

    //loads audio clips with data from .wav files - this allows for faster reading of audio data
    //as the data is being read from stack as opposed to disk
    for (auto &track : tracks) {
        track.loadAudioClips(wav_gen.getMaxAmplitude());
    }
}

Session::Session(std::string project_name, u_int sample_rate, u_int buffer_size, u_int num_input_channels, u_int num_output_channels) {
    this->project_name = project_name;
    this->sample_rate = sample_rate;
    this->buffer_size = buffer_size;
    this->num_input_channels = num_input_channels;
    this->num_output_channels = num_output_channels;
    wav_gen.initialise(sample_rate, bit_depth, num_output_channels);
}
Session::~Session() {
    //Clears the Audio clips from temp memory
    for (auto &track : tracks) {
        track.clearAudioClips();
    }
}

void Session::createTrack() {
    tracks.push_back(Track(project_name, "track" + std::to_string(tracks.size() + 1)));
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
                int counter = 0;
                for (auto &track : tracks) {
                    output_sample += track.getSample(channel, current_time, counter, wav_gen.getMaxAmplitude());
                }
                output_sample = output_sample * 0.5;
                if (output_sample >= 1) {
                    output_sample = 0.999;
                } else if (output_sample <= -1) {
                    output_sample = -0.999;
                }
            }
            *output_buffer = output_sample;
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
        int counter = 0;
        output_sample += track.getSample(channel, current_time, counter, wav_gen.getMaxAmplitude());
        output_sample = output_sample * 0.5;
        if (output_sample >= 1) {
            output_sample = 0.999;
        } else if (output_sample <= -1) {
            output_sample = -0.999;
        }
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
                    wav_gen.writeInputToFile(wav_file_streamer, clip.audio_stream[i]);
                }
                wav_gen.closeWaveFile(wav_file_streamer);
                wav_file_streamer.close();
            }
        }
    }
}

u_int Session::getCurrentTimeInSamples() {
    return current_time;
}

float Session::getCurrentTimeInSeconds() {
    return 1.0f * current_time / sample_rate;
}

void Session::movePlayhead(int time_in_samples) {
    if ((current_time + time_in_samples) < 0) {
        current_time = 0;
    } else {
        current_time += time_in_samples;
    }
}
