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
    Clip(std::string project_name, std::string clip_name, u_int start_time) {
        this->project_name = project_name;
        this->clip_name = clip_name;
        this->start_time_in_session = start_time;
        reference_file_path = project_name + "/recorded_audio/" + clip_name + ".wav";
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

    std::string getName() { return clip_name; }
    std::string getReferenceFilePath() { return reference_file_path; }

    void loadAudioStream(double max_amplitude_of_source) {
        if (!audio_stream.empty()) {
            audio_stream.clear();
        }
        for (int i = 0; i < length; i++) {
            int first_sample_index = 44;
            //each audio sample is 2 bytes therefore index is multiplied by 2 to get byte index of sample
            int sample_pos = first_sample_index + start_time_in_reference + (i * 2);
            //as samples are stored as little endian, both bytes within sample must be retrieved individually
            char first_byte, second_byte;
            reference_file_stream.open(reference_file_path, ios::binary);
            reference_file_stream.seekg(sample_pos, ios::beg);
            reference_file_stream.read(&first_byte, 1);
            reference_file_stream.read(&second_byte, 1);
            reference_file_stream.close();
            //sample data is then converted to its original double between 1.0 and -1.0
            audio_stream.push_back(bytesToDouble(first_byte, second_byte, max_amplitude_of_source));
        }
    }

    static double bytesToDouble(unsigned char first_byte, unsigned char second_byte, double max_amplitude) {
        // convert two bytes to one 16-bit int (little endian)
        int16_t output = (second_byte << 8) | first_byte;
        // convert to range from -1 to (just below) 1
        return output / max_amplitude;
    }

    double getSample(int index) {
        return audio_stream[index];
    }

    u_int getNumSamples() { return length; }
    int getStartTime() { return start_time_in_session; }
    int getStartTimeInReference() { return start_time_in_reference; }
    int getEndTime() { return (start_time_in_session + (length * 0.5)); }

    vector<double> audio_stream;

private:
    u_int start_time_in_session, start_time_in_reference = 0, length = 0;
    string project_name, clip_name, reference_file_path;
    ifstream reference_file_stream = {};
};

#endif