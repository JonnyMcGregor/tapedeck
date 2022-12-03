#pragma once
#ifndef _TRACK_H_
#define _TRACK_H_
#include <memory>
#include "Clip.hpp"
#include <JuceHeader.h>

struct Track {
    std::vector<std::shared_ptr<Clip>> clips; 
    std::vector<ClipMetadata> clipMetadata;
    std::string name; // a user-provided name for display purposes
    bool recordArmed = false, solo = false, mute = false;
    bool isSelected = false;
    Track() {
    }

    void createClip(u_int startTime, std::string referenceFilePath) {
        jassert(startTime >= 0);
        jassert(referenceFilePath != "");

        std::string clip_name = name + "Clip" + std::to_string(clips.size() + 1);
        referenceFilePath = referenceFilePath + clip_name + ".wav";
        this->clips.push_back(std::shared_ptr<Clip>());
        this->clips.back() = std::make_shared<Clip>(Clip());
        this->clipMetadata.push_back(ClipMetadata(startTime, referenceFilePath));
    }

    void createClip(u_int startTime, std::string referenceFilePath, Clip &clip)
    {
        jassert(startTime >= 0);
        jassert(referenceFilePath != "");
        jassert(&clip != nullptr);
     
        std::string clip_name = name + "Clip" + std::to_string(clips.size() + 1);
        referenceFilePath = referenceFilePath + clip_name + ".wav";
        this->clips.push_back(std::make_shared<Clip>(clip));
        this->clipMetadata.push_back(ClipMetadata(startTime, referenceFilePath));
    }

    void deleteClip(int clipIndex) {
        jassert(clipIndex >= 0);
        std::vector<std::shared_ptr<Clip>>::iterator it = clips.begin();
        std::vector<ClipMetadata>::iterator it2 = clipMetadata.begin();

        advance(it, clipIndex);
        advance(it2, clipIndex);
        clips.erase(it);
        clipMetadata.erase(it2);
    }

    void moveClip(u_int index, u_int newStartTime) {
        jassert(index >= 0 && index < clipMetadata.size()); //Index is within bounds of array
        clipMetadata[index].startTime = newStartTime;
    }

    Sample getSample(int sampleIndex) {
        for (int i = 0; i < this->clips.size(); i++) {
            u_int clipStartTime = clipMetadata[i].startTime;
            u_int clipEndTime = clipStartTime + this->clips[i]->size();
            if (clipStartTime <= sampleIndex && sampleIndex < clipEndTime) {
                return this->clips[i]->getSample(sampleIndex - clipStartTime);
            }
        }
        return Sample(0);
    }
};
#endif