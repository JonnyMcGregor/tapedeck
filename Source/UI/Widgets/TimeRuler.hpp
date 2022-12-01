#pragma once
//#include "../Components/Widget.hpp"
#include <memory>
struct TimeRuler : juce::Component {
    float pixelsPerSecond = 60.0f;
    int pixelsPerTimeMarker = 20;
    int startTimeOnScreenInSamples = 0;
    int sampleRate = 0;
    int windowSizeInSamples = 0;
    int bpm = 120;
    int timeSigNumer = 4, timeSigDenom = 4;

    TimeRuler(int sampleRate) : sampleRate(sampleRate) {}

    void paint(juce::Graphics &screen) override {
        windowSizeInSamples = getWidth() * samplesPerPixel();

        pixelsPerTimeMarker = pixelsPerSecond * (bpm / 60);
        int pixelsPerMicroMarker = pixelsPerTimeMarker / timeSigNumer;
        for (int x = 0; x < getWidth(); x++) {
            if (x % pixelsPerTimeMarker == 0) {
                int time = float(xPositionToTimeInSamples(x) / (samplesPerBeat()));
                screen.setColour(juce::Colours::white);
                screen.drawText(juce::String(time), x, 0, 20, getHeight(), juce::Justification::left);
            } else if (x % pixelsPerMicroMarker == 0) {
                screen.setColour(juce::Colours::white);
                screen.drawText("|", x, 0, 10, getHeight(), juce::Justification::left);                
            }
        }
    }

    int samplesPerPixel() {
        return (sampleRate / pixelsPerSecond);
    }
    int samplesPerBeat() {
        return (bpm / 60) * sampleRate;
    }
    int xPositionToTimeInSamples(int xPosition) {
        return (xPosition * samplesPerPixel()) + startTimeOnScreenInSamples;
    }

    int timeInSamplesToXPosition(int timeInSamples) {
        return (timeInSamples - startTimeOnScreenInSamples) / samplesPerPixel();
    }
};