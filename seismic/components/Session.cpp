#include "Session.h"

Session::Session(unsigned int sample_rate, unsigned int buffer_size) {
    this->sample_rate = sample_rate;
    this->buffer_size = buffer_size;
}
void Session::createTrack() {
    tracks.push_back(Track("track" + std::to_string(tracks.size() + 1)));
}
void Session::deleteTrack(int index) {

    std::vector<Track *>::iterator track_ptr_iterator = record_armed_tracks.begin();
    for (int i = 0; i < record_armed_tracks.size(); i++, track_ptr_iterator++)
        if (&tracks[index] == record_armed_tracks[i])
            record_armed_tracks.erase(track_ptr_iterator);

    std::vector<Track>::iterator track_iterator = tracks.begin();
    advance(track_iterator, index);
    tracks.erase(track_iterator);
}

void Session::prepareAudio() {
    for (int i = 0; i < tracks.size(); i++) {
        if (tracks[i].is_record_enabled) {
            tracks[i].createClip(current_time);
            record_armed_tracks.push_back(&tracks[i]);
        }
    }
}

void Session::processAudioBlock(double *input_buffer, double *output_buffer) {
    //Record Input
    for (int sample = 0; sample < buffer_size; sample++) {
        for (int channel = 0; channel < 2; channel++, *input_buffer++, *output_buffer++) {
            double output_sample = 0;
            if (play_state == Play_State::Recording) {
                for (auto &track : tracks) {
                    //input
                    if (track.is_record_enabled)
                        track.clips.back().addSample(*input_buffer);
                    //output
                    else {
                        for (auto clip : track.clips) {
                            if (clip.getStartTime() <= current_time && current_time <= clip.getEndTime())
                                output_sample += clip.getSample(current_time - clip.getStartTime());
                        }
                    }
                }
            }
            //Output
            else {
                for (auto track : tracks) {
                    for (auto clip : track.clips) {
                        if (clip.getStartTime() <= current_time && current_time <= clip.getEndTime())
                            output_sample += clip.getSample(current_time - clip.getStartTime());
                    }
                }
            }
            *output_buffer = output_sample;
        }
        current_time++;
    }
}