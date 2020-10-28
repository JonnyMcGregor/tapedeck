#pragma once
#ifndef _CLIP_H_
#define _CLIP_H_

#include <assert.h>
#include <fstream>
#include <memory>
#include <vector>
using namespace std;

struct Clip {
    Clip(std::string session_name, std::string clip_name, u_int start_time) {
        this->clip_name = clip_name;
        this->start_time_in_session = start_time;
        reference_file_path = session_name + "/recorded_audio/" + clip_name + ".wav";
    }

    void addSample(double sample) {
        audio_stream.push_back(sample);
        length++;
    }

    vector<double> audio_stream;
    ifstream reference_file_stream = {};
    u_int start_time_in_session, start_time_in_reference = 0, length = 0;
    std::string clip_name, reference_file_path;
};

#endif