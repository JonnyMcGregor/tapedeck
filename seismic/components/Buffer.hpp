#pragma once

#include "Sample.hpp"
#include <assert.h>
#include <memory>
#include <vector>

struct Buffer {
    int num_channels;
    int buffer_size;

    Buffer(float **&audio_buffer) {
        num_channels = sizeof(audio_buffer);
        buffer_size = sizeof(*audio_buffer);
        buffer_data = audio_buffer;
    }
     Buffer(const float **&audio_buffer) {
        num_channels = sizeof(audio_buffer);
        buffer_size = sizeof(*audio_buffer);
        fixed_buffer_data =  audio_buffer;
    }

    Sample get_sample(int channel, int index) {
        if(fixed_buffer_data != nullptr)
        {
            return Sample(fixed_buffer_data[channel][index]);
        }
        assert(buffer_data != nullptr);
        return Sample(buffer_data[channel][index]);
    }

    void set_sample(int channel, int index, Sample sample_value)
    {
        assert(fixed_buffer_data == nullptr); //if fixed_buffer_data is read only
        assert(buffer_data != nullptr); // buffer data must be defined
        buffer_data[channel][index] = sample_value.value;
    }
private : 
    float **buffer_data = nullptr;
    const float **fixed_buffer_data = nullptr;
};
