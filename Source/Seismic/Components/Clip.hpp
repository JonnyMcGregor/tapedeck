#pragma once

#include "Sample.hpp"
#include <stdexcept>
#include <string>
#include <vector>

using u_int = uint32_t;

struct Clip {
    std::vector<Sample> audioData;

    Clip() {
    }

    void appendSample(Sample sample) {
        audioData.push_back(sample);
    }

    void removeLastSample() {
        audioData.pop_back();
    }

    Sample getSample(int index) {
        assertIsIndexInBounds(index);
        return audioData[index];
    }

    u_int size() {
        return this->audioData.size();
    }

    void clear() {
        this->audioData.clear();
    }

private:
    void assertIsIndexInBounds(int index) {
        if (!isIndexInBounds(index)) {
            throw std::invalid_argument("Sample index is out of bounds");
        }
    }
    bool isIndexInBounds(int index) {
        if (0 <= index && index <= this->audioData.size()) {
            return true;
        }
        return false;
    }
};