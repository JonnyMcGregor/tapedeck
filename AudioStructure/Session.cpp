#include "Session.h"

Session::Session(int sample_rate, int buffer_size) {
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

void Session::processBlock(double *input_buffer, double *output_buffer) {
    //Record Input
    for (int sample = 0; sample < buffer_size; sample++) {
        for (int channel = 0; channel < 2; channel++, *input_buffer++) {
            for (auto track : tracks)
                //manage output
                /*
                Here we need to pass to the output_buffer relevant samples from 
                the clips in each track.
                */
                for (auto record_track : record_armed_tracks)
                    record_track->clips.back().addSample(*input_buffer);
        }
    }
}