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
    Clip(std::string session_name, std::string clip_name, u_int start_time) {
        this->clip_name = clip_name;
        this->start_time_in_session = start_time;
        reference_file_path = session_name + "/recorded_audio/" + clip_name + ".wav";
    }

    void addSample(double sample) {
        audio_stream.push_back(sample);
        length++;
    }

    void setName(std::string clip_name) { this->clip_name = clip_name; }

    void setStartTime(int start_time) {
        start_time_in_session = start_time;
    }

    void setLength(u_int length) {
        this->length = length;
    }

    void setReferenceFilePath(string path) {
        reference_file_path = path;
    }

    std::string getName() { return clip_name; }
    std::string getReferenceFilePath() { return reference_file_path; }

    double getSample(int index) {
        return audio_stream[index];
    }
    u_int getNumSamples() { return length; }
    int getStartTime() { return start_time_in_session; }
    int getStartTimeInReference() { return start_time_in_reference; }
    int getEndTime() { return (start_time_in_session + (length * 0.5)); }

    vector<double> audio_stream;
    ifstream reference_file_stream = {};

private:
    u_int start_time_in_session, start_time_in_reference = 0, length = 0;
    std::string clip_name, reference_file_path;
};

#endif