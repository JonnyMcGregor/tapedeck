/*
  ==============================================================================

    LevelMeter.h
    Created: 2 Dec 2022 1:58:58pm
    Author:  Jonny McGregor

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>

class LevelMeter : public juce::Component {
public:
    LevelMeter() {  
    }
    ~LevelMeter() {}
    void paint(juce::Graphics &g) {
        auto bounds = getLocalBounds().toFloat();
        

        g.setColour(juce::Colours::black);
        g.fillRect(bounds);
        g.setColour(ColourPalette::playheadColour);
        g.drawRect(bounds, 1);

        g.setColour(juce::Colours::white);
        float barHeight = juce::jmap(level, -100.0f, 6.0f, 0.0f, (float)getHeight());
        g.fillRect(0.0f, getHeight() - barHeight, bounds.getWidth(), barHeight);
    }
    void resized() {
       
    }
    void setLevel(const float levelDB)
    {
        level = levelDB;
    }

private:
    float level = -60;
};