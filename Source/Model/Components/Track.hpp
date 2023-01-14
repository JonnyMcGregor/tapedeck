#pragma once
#ifndef _TRACK_H_
#define _TRACK_H_
#include <memory>
#include "Clip.hpp"
#include <JuceHeader.h>

struct Track {
    std::vector<std::shared_ptr<Clip>> clips; 
    std::string name; // a user-provided name for display purposes
    bool recordArmed = false, solo = false, mute = false;
    bool isSelected = false;
    Track() {
    }

    void createClip(int startTime, std::string referenceFilePath) {
        jassert(startTime >= 0);
        jassert(referenceFilePath != "");

        std::string clip_name = name + "Clip" + std::to_string(clips.size() + 1);
        referenceFilePath = referenceFilePath + clip_name + ".wav";
        this->clips.push_back(std::make_shared<Clip>(Clip(ClipMetadata(startTime, referenceFilePath))));
    }

    void createClip(Clip &clip)
    {
        jassert(&clip != nullptr);
        this->clips.push_back(std::make_shared<Clip>(clip));
    }

    void deleteClip(int clipIndex) {
        jassert(clipIndex >= 0);
        std::vector<std::shared_ptr<Clip>>::iterator it = clips.begin();

        advance(it, clipIndex);
        clips.erase(it);
    }

    void moveClip(int index, int newStartTime) {
        jassert(index >= 0 && index < clips.size()); //Index is within bounds of array
        clips[index]->metadata.setStartTimeInSession(newStartTime);
    }
    // Returns the clip sample occuring at sampleIndex in absolute time in session
    Sample getSample(int sampleIndex) {
        // Iterate through clips to check if sample falls within bounds
        for (int i = 0; i < this->clips.size(); i++) {
            int clipStartTime = clips[i]->metadata.getStartTimeInSession() + clips[i]->metadata.getStartTimeInSourceFile();
            int clipEndTime = clips[i]->metadata.getStartTimeInSession() + this->clips[i]->size() - 1 - clips[i]->metadata.getEndTimeInSourceFile();
            if (clipStartTime <= sampleIndex && sampleIndex < clipEndTime) {
                return this->clips[i]->getSample(sampleIndex - clipStartTime + clips[i]->metadata.getStartTimeInSourceFile()); 
            }
        }
        return Sample(0);
    }
};
#endif