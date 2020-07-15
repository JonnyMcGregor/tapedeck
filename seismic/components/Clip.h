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
        write_audio_stream.push_back(sample);
        length++;
    }

    void setName(std::string clip_name) { this->clip_name = clip_name; }

    void setStartTime(int start_time) {
        start_time_in_session = start_time;
    }

    void setEndTime(u_int end_time) {
        length = start_time_in_session + end_time;
    }

    void clearAudioStream() { write_audio_stream.clear(); }

    std::string getName() { return clip_name; }
    std::string getReferenceFilePath() { return reference_file_path; }

    static double bytesToDouble(unsigned char first_byte, unsigned char second_byte, double max_amplitude) {
        // convert two bytes to one 16-bit int (little endian)
        int16_t output = (second_byte << 8) | first_byte;
        // convert to range from -1 to (just below) 1
        return output / max_amplitude;
    }

    double getSample(int index, double max_amplitude_of_source) {
        if (index < length) {
            //audio data in .wav file begins at the 44th byte
            int first_sample_index = 44;
            //each audio sample is 2 bytes therefore index is multiplied by 2 to get byte index of sample
            int sample_pos = first_sample_index + (index * 2);
            //as samples are stored as little endian, both bytes within sample must be retrieved individually
            char first_byte, second_byte;
            reference_file_stream.open(reference_file_path, ios::binary);
            reference_file_stream.seekg(sample_pos, ios::beg);
            reference_file_stream.read(&first_byte, 1);
            reference_file_stream.read(&second_byte, 1);
            reference_file_stream.close();
            //sample data is then converted to its original double between 1.0 and -1.0
            double sample = bytesToDouble(first_byte, second_byte, max_amplitude_of_source);
            return (sample);
        }
    }

    u_int getNumSamples() { return length; }
    int getStartTime() { return start_time_in_session; }
    int getStartTimeInReference() { return start_time_in_reference; }
    int getEndTime() { return (start_time_in_session + length); }

    vector<double> write_audio_stream;

private:
    u_int start_time_in_session, start_time_in_reference = 0, length = 0;
    string clip_name, reference_file_path;
    ifstream reference_file_stream = {};
};

#endif