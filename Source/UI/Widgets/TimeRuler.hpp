#pragma once
//#include "../Components/Widget.hpp"
#include <memory>
struct TimeRuler : juce::Component {
    TimeRuler(int sampleRate) : sampleRate(sampleRate) {}

    void paint(juce::Graphics &screen) override {
        windowSizeInSamples = getWidth() * samplesPerPixel();

        pixelsPerTimeMarker = pixelsPerSample() * samplesPerBar(); // every bar

        int pixelsPerMicroMarker = pixelsPerSample() * samplesPerBeat(); //every beat
        int samplesPerrBar = samplesPerBar();
        int currentBarNumber = (int)ceil((float)startTimeOnScreenInSamples / samplesPerBar());
        for (int x = 0; x < getWidth(); x++) {
            if(xPositionToTimeInSamples(x) % samplesPerBar() >= samplesPerBar() - samplesPerPixel() * 0.5 || 
                xPositionToTimeInSamples(x) % samplesPerBar() <= samplesPerPixel() * 0.5) {
                screen.setColour(juce::Colours::white);
                screen.drawText(juce::String(currentBarNumber), x, 0, 20, getHeight(), juce::Justification::left);
                currentBarNumber++;
            }

            else if (xPositionToTimeInSamples(x) % samplesPerBeat() >= samplesPerBeat() - samplesPerPixel() * 0.5 ||
                     xPositionToTimeInSamples(x) % samplesPerBeat() <= samplesPerPixel() * 0.5) {
                screen.setColour(juce::Colours::white);
                screen.drawText("|", x, 0, 10, getHeight(), juce::Justification::left);                
            }
        }
    }

    void setStartTimeOnScreenInSamples(int startTime) {
        startTimeOnScreenInSamples = max(0, startTime);
    }
    int getStartTimeOnScreenInSamples() {
        return startTimeOnScreenInSamples;
    }
    int getWindowSizeInSamples() {
        return windowSizeInSamples;
    }
    float pixelsPerSample() {
       return (pixelsPerSecond / sampleRate);
    }
    int samplesPerPixel() {
        return (sampleRate / pixelsPerSecond);
    }
    int samplesPerBeat() {
        return (60.0f/bpm) * sampleRate;
    }
    int samplesPerBar() {
        return samplesPerBeat() * timeSigNumer;
    }
    int xPositionToTimeInSamples(int xPosition) {
        return (xPosition * samplesPerPixel()) + startTimeOnScreenInSamples;
    }

    int timeInSamplesToXPosition(int timeInSamples) {
        return (timeInSamples - startTimeOnScreenInSamples) / samplesPerPixel();
    }

private:
    float pixelsPerSecond = 60.0f;
    int pixelsPerTimeMarker = 20;
    int startTimeOnScreenInSamples = 0;
    int sampleRate = 0;
    int windowSizeInSamples = 0;
    int bpm = 120;
    int timeSigNumer = 4, timeSigDenom = 4;
};