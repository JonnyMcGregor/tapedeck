#pragma once
#include "../../Seismic/Components/Clip.hpp"
#include "../Components/Point.hpp"
//#include "../Components/Screen.hpp"
//#include "../Components/Widget.hpp"
#include <stdexcept>

struct ClipWidget : juce::Component 
{
    std::shared_ptr<Clip> clip;
    std::shared_ptr<TimeRuler> timeRuler; 
    u_int clipStartTime;
    ClipWidget(std::shared_ptr<Clip> clip, std::shared_ptr<TimeRuler> timeRuler, u_int clipStartTime) {
        this->clip = clip;
        this->timeRuler = timeRuler;
        this->clipStartTime = clipStartTime;
    }

    void paint(juce::Graphics &screen) override
    {
		if (getWidth() > 0)
		{
            int firstSampleToDraw = jmax(0, (int)timeRuler->startTimeOnScreenInSamples - (int)clipStartTime);
            int lastSampleToDraw = jmin((int)(clip->size() - 1), (timeRuler->startTimeOnScreenInSamples + timeRuler->windowSizeInSamples - (int)clipStartTime));

            u_int bucketSize = (lastSampleToDraw - firstSampleToDraw) / getWidth();
            juce::Path waveform;
            waveform.startNewSubPath(0, juce::jmap(clip->getSample(0).value, -1.0f, 1.0f, (float)getHeight(), 0.0f));
            
			for (int x = 0; x < getWidth(); x++) 
			{
                int currentSampleToDraw = (x * bucketSize) + firstSampleToDraw;
                if (currentSampleToDraw < lastSampleToDraw) 
				{
                    int y = juce::jmap(clip->getSample(currentSampleToDraw).value, -1.0f, 1.0f, (float)getHeight(), 0.0f);
                    waveform.lineTo(x, y);
                } 
				else 
				{
                    jassertfalse; // currentSampleToDraw should always be less than lastSampleToDraw...
                }
            }

            if (isSelected) {
                screen.fillAll(backgroundColour.brighter(0.5));

            } else {
                screen.fillAll(backgroundColour);
            }

            screen.setColour(waveformColour);
            screen.strokePath(waveform, PathStrokeType{2});
		}
        
    }

    void resized() override
    {

    }

    void mouseDown (const juce::MouseEvent& event) override
    {
        isSelected = true;
    }

    bool isSelected = false;
private:
    
    juce::Colour backgroundColour = juce::Colours::blue;
    juce::Colour waveformColour = juce::Colours::white;
};

