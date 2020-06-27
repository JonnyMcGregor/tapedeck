#pragma once
#ifndef _CLIP_H_
#define _CLIP_H_

#include <assert.h>
#include <fstream>
#include <memory>
#include <vector>

using namespace std;

class Clip {
public:
    Clip(std::string clip_name, u_int start_time) {
        this->clip_name = clip_name;
        this->start_time_in_session = start_time;
        reference_file_path = "recorded_audio/" + clip_name + ".wav";
    }

    void addSample(double sample) {
        temp_audio_stream.push_back(sample);
        length++;
    }

    void setName(std::string clip_name) { this->clip_name = clip_name; }

    void setStartTime(int start_time) {
        start_time_in_session = start_time;
    }

    void setEndTime(u_int end_time) {
        length = start_time_in_session + end_time;
    }

    void clearAudioStream() { temp_audio_stream.clear(); }

    std::string getName() { return clip_name; }
    std::string getReferenceFilePath() { return reference_file_path; }

    double getSample(int index, int max_amplitude_of_source) {
        int first_sample_index = 44;
        char sample = 0;
        reference_file_stream.open(reference_file_path);
        reference_file_stream.seekg(first_sample_index + (index * 2));
        reference_file_stream.get(&sample, 2);
        reference_file_stream.close();
        return ((int)sample / max_amplitude_of_source);
    }

    u_int getNumSamples() { return length; }
    int getStartTime() { return start_time_in_session; }
    int getEndTime() { return (start_time_in_session + length); }

    vector<double> temp_audio_stream;

private:
    u_int start_time_in_session, start_time_in_reference, length = 0;
    string clip_name, reference_file_path;
    ifstream reference_file_stream = {};
};

#endif