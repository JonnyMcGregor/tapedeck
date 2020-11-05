#pragma once
#ifndef _CLIP_H_
#define _CLIP_H_

#include "Sample.hpp"
#include <stdexcept>
#include <string>
#include <vector>

struct Clip {
    std::vector<Sample> audio_data;
    std::string reference_file_path;
    int start_time_in_reference = 0;

    Clip(std::string reference_file_path) {
        this->reference_file_path = reference_file_path;
    }

    void append_sample(Sample sample) {
        audio_data.push_back(sample);
    }

    void remove_sample() {
        audio_data.pop_back();
    }

    Sample get_sample(int index) {
        assert_is_index_in_bounds(index);
        return audio_data[index];
    }

    u_int size() {
        return this->audio_data.size();
    }

    void clear() {
        this->audio_data.clear();
    }

private:
    void assert_is_index_in_bounds(int index) {
        if (!is_index_in_bounds(index)) {
            throw std::invalid_argument("Sample index is out of bounds");
        }
    }
    bool is_index_in_bounds(int index) {
        if (0 <= index && index <= this->audio_data.size()) {
            return true;
        }
        return false;
    }
};

#endif