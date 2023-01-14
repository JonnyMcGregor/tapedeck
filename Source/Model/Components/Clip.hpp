#pragma once

#include "Sample.hpp"
#include <stdexcept>
#include <string>
#include <vector>

struct ClipMetadata {
    
    int clipLength = 0; //Length in samples of clip before truncation
    std::string referenceFilePath;

    ClipMetadata(int startTime, std::string referenceFilePath) : startTime(startTime), referenceFilePath(referenceFilePath){};
    // Returns the length in samples of the active section of the clip visible in the timeline
    int getTruncatedClipLength() {
        return clipLength - startTimeInSourceFile - endTimeInSourceFile;
    }
    
    void setStartTimeInSession(int startTime) {
        this->startTime = startTime;
    }
    int getStartTimeInSession() {
        return startTime;
    }

    void setStartTimeInSourceFile(int timeSamples)
    {
        // Start Time Cannot be negative
        if (timeSamples < 0) {
            startTimeInSourceFile = 0;
        }
        // Start time cannot be greater than end time
        else if (timeSamples >= clipLength - 1 - endTimeInSourceFile) {
            startTimeInSourceFile = clipLength - 2 - endTimeInSourceFile;
        }
        else {
            startTimeInSourceFile = timeSamples;
        }
    }

    int getStartTimeInSourceFile() {
        return startTimeInSourceFile;
    }
    void setEndTimeInSourceFile(int timeSamples)
    {
        // End Time cannot be negative
        if (timeSamples < 0) {
            endTimeInSourceFile = 0;
        }
        // End time cannot point beyond the sample range
        else if (timeSamples >= clipLength) {
            endTimeInSourceFile = clipLength - 1;
        }
        // End time must occur after Start Time
        else if (clipLength - timeSamples < startTimeInSourceFile) {
            endTimeInSourceFile = startTimeInSourceFile + 1;
        }
        else {
            endTimeInSourceFile = timeSamples;
        }
    }

    int getEndTimeInSourceFile()
    {
        return endTimeInSourceFile;
    }

private:
    int startTime;                 // Sample time in project where clip starts
    int startTimeInSourceFile = 0; // distance in samples from start of reference file that clip starts
    int endTimeInSourceFile = 0;   // distance in samples from end of reference file that clip ends
};

struct Clip {
    std::vector<Sample> audioData;
    ClipMetadata metadata;

    Clip(int startTime, std::string referenceFilePath) : metadata(startTime, referenceFilePath) {}
    Clip(ClipMetadata metadata) : metadata(metadata) {}

    void appendSample(Sample sample) {
        audioData.push_back(sample);
        metadata.clipLength++;
    }

    void removeLastSample() {
        audioData.pop_back();
        metadata.clipLength--;
    }

    Sample getSample(int index) {
        assertIsIndexInBounds(index);
        return audioData[index];
    }

    int size() {
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