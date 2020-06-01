#include "Session.h"

Session::Session(int sample_rate, int buffer_size) {
    this->sample_rate = sample_rate;
    this->buffer_size = buffer_size;
}
void Session::createTrack() {
    tracks.push_back(Track("track" + std::to_string(tracks.size() + 1)));
}
void Session::deleteTrack(int index) {
    std::vector<Track>::iterator it = tracks.begin();
    advance(it, index);
    tracks.erase(it);
}
void Session::prepareAudio() {
    for (int i = 0; i < tracks.size(); i++) {
        if (tracks[i].is_record_enabled) {
            tracks[i].createClip(current_time);
            record_armed_tracks.push_back(i);
        }
    }
}

void Session::processBlock(double *input_buffer, double *output_buffer) {
    //Record Input
    for (int sample = 0; sample < buffer_size; sample++) {
        for (int channel = 0; channel < 2; channel++, *input_buffer++) {
            for (auto record_track : record_armed_tracks)
                tracks[record_track].clips.back().addSample(*input_buffer);
        }
    }
}