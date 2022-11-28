#pragma once
//#include "../Components/Widget.hpp"
#include <memory>
struct TimeRuler : juce::Component {
    float pixelsPerSecond = 60.0f;
    int pixelsPerTimeMarker = 20;
    int startTimeOnScreenInSamples = 0;
    int sampleRate = 0;
    int windowSizeInSamples = 0;

    TimeRuler(int sampleRate) 
    { 
        this->sampleRate = sampleRate; 
    }

    void paint(juce::Graphics &screen) override {
        windowSizeInSamples = getWidth() * samplesPerPixel();

        pixelsPerTimeMarker = getWidth()/5;
        int cells_per_micro_marker = getWidth() / 15;
        for (int x = 0; x < getWidth(); x++) {
            if (x % pixelsPerTimeMarker == 0) {
                std::string time = to_string((1.0 * x / pixelsPerSecond) + (1.0 * startTimeOnScreenInSamples / sampleRate));
                time.resize(time.find(".") + 2);
                screen.setColour(juce::Colours::white);
                screen.drawText(time, x, 0, 10, 10, juce::Justification::centred);
            } else if (x % cells_per_micro_marker == 0) {
                screen.setColour(juce::Colours::white);
                screen.drawText("|", x, 0, 10, 10, juce::Justification::centred);                
            }
        }
    }

    int samplesPerPixel() {
        return (sampleRate / pixelsPerSecond);
    }

    int xPositionToTimeInSamples(int xPosition) {
        return (xPosition * samplesPerPixel()) + startTimeOnScreenInSamples;
    }

    int timeInSamplesToXPosition(int timeInSamples) {
        return (timeInSamples - startTimeOnScreenInSamples) / samplesPerPixel();
    }
};