#pragma once

struct Playhead {
    
    Playhead(int sampleRate)
    {
        this->sampleRate = sampleRate;
    }

    int currentTimeInSamples = 0,
        tempo = 120,
        timeSigNum = 4,
        timSigDenom = 4;

    void movePlayhead(int timeInSamples) {
        if ((currentTimeInSamples + timeInSamples) < 0) {
            currentTimeInSamples = 0;
        } else {
            currentTimeInSamples += timeInSamples;
        }
    }
    void movePlayhead(float timeInSeconds) {
        if ((currentTimeInSamples + seconds_to_samples(timeInSeconds)) < 0) {
            currentTimeInSamples = 0;
        } else {
            currentTimeInSamples += seconds_to_samples(timeInSeconds);
        }
    }

    int seconds_to_samples(float timeInSeconds)
    {
        return timeInSeconds * sampleRate;
    }
    private:
    int sampleRate = 0;
};
