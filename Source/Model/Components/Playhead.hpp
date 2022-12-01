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
        if ((currentTimeInSamples + secondsToSamples(timeInSeconds)) < 0) {
            currentTimeInSamples = 0;
        } else {
            currentTimeInSamples += secondsToSamples(timeInSeconds);
        }
    }

    int secondsToSamples(float timeInSeconds)
    {
        return timeInSeconds * sampleRate;
    }
    private:
    int sampleRate = 0;
};
