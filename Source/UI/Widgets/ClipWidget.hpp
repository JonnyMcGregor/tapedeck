#pragma once
#include "../../Model/Components/Clip.hpp"
//#include "../Components/Point.hpp"
//#include "../Components/Screen.hpp"
//#include "../Components/Widget.hpp"
#include <stdexcept>

struct ClipWidget : juce::Component 
{
    std::shared_ptr<Clip> clip;
    std::shared_ptr<TimeRuler> timeRuler; 
    std::vector<float> waveformAmplitudeValues;
    ClipWidget(std::shared_ptr<Clip> clip, std::shared_ptr<TimeRuler> timeRuler) 
        : clip(clip), timeRuler(timeRuler)
    {
    }
    void calculateWaveformAmplitudeValues() {
        //Clear the values in case anything has changed
        waveformAmplitudeValues.clear();

        // Constants for Log Scaling
        // Define a constant for the minimum possible sample value
        constexpr float MIN_SAMPLE_VALUE = -1.0f;
        // Define a constant for the maximum possible sample value
        constexpr float MAX_SAMPLE_VALUE = 1.0f;
        // Define a constant for the minimum y-coordinate in the waveform
        float MIN_Y = 0;
        // Define a constant for the maximum y-coordinate in the waveform
        float MAX_Y = getHeight();
        // Compute the range of the y-coordinate values
        const float yRange = MAX_Y - MIN_Y;
        // Compute the range of the sample values
        const float sampleRange = MAX_SAMPLE_VALUE - MIN_SAMPLE_VALUE;
        // Compute the scaling factor for converting sample values to y-coordinates
        const float scaleFactor = yRange / sampleRange;

        // Logarithmically scale the amplitudes
        for (auto sampleAmplitude : clip->audioData) {
            float offsetInput = sampleAmplitude.value - MIN_SAMPLE_VALUE;
            waveformAmplitudeValues.push_back(MIN_Y + (log(offsetInput) - MIN_SAMPLE_VALUE) * scaleFactor);
        }
    }
    void paint(juce::Graphics &screen) override
    {
       
		if (getWidth() > 0)
		{
            if (waveformAmplitudeValues.size() <= 0) {
                calculateWaveformAmplitudeValues();
            }
            // Look into startTimeOnScreenInSamples - somethings going wrong here...
            int firstSampleInTruncatedClip = juce::jmax(0, (int)clip->metadata.getStartTimeInSourceFile());
            int firstSampleVisibleOnScreen = timeRuler->getStartTimeOnScreenInSamples() - clip->metadata.getStartTimeInSession() + clip->metadata.getStartTimeInSourceFile();
            int firstSampleToDraw = juce::jmax(firstSampleInTruncatedClip, firstSampleVisibleOnScreen);
            int lastSampleInTruncatedClip = juce::jmin(clip->size() - 1 - clip->metadata.getEndTimeInSourceFile(), clip->size() - 1);
            int lastSampleVisibleOnScreen = timeRuler->getStartTimeOnScreenInSamples() + timeRuler->getWindowSizeInSamples() - clip->metadata.getStartTimeInSession();
            int lastSampleToDraw = juce::jmin(lastSampleInTruncatedClip, lastSampleVisibleOnScreen);
            if (firstSampleToDraw > lastSampleToDraw) {
                return;
            }

            int bucketSize = (lastSampleToDraw - firstSampleToDraw) / getWidth();
            juce::Path waveform;
            waveform.startNewSubPath(0, waveformAmplitudeValues[firstSampleToDraw]);
            
			for (int x = 0; x < getWidth(); x++) 
			{
                int currentSampleToDraw = (x * bucketSize) + firstSampleToDraw;// do something here to fix it pls
                if (currentSampleToDraw < lastSampleToDraw) 
				{
                    int y = waveformAmplitudeValues[currentSampleToDraw];
                    waveform.lineTo(x, y);
                } 
				else 
				{
                    jassertfalse; // currentSampleToDraw should always be less than lastSampleToDraw...
                }
            }

            if (isSelected) {
                screen.fillAll(ColourPalette::waveformColour.brighter(0.6));

            } else {
                screen.fillAll(backgroundColour);
            }

            screen.setColour(waveformColour);
            screen.strokePath(waveform, juce::PathStrokeType{2});
		}
        
    }

    void resized() override
    {

    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        isSelected = true;
        
    }
    void mouseDrag(const juce::MouseEvent& e) override {
        
    }

    void mouseMove(const juce::MouseEvent& e) override {
        // Setting Mouse Cursor based on position in clip...
        if (e.x <= 10) {
            if (getMouseCursor() != juce::MouseCursor::LeftEdgeResizeCursor) {
                setMouseCursor(juce::MouseCursor::LeftEdgeResizeCursor);
            }
        }
        else if (e.x >= getWidth() - 10) {
            if (getMouseCursor() != juce::MouseCursor::LeftEdgeResizeCursor) {
                setMouseCursor(juce::MouseCursor::RightEdgeResizeCursor);
            }
        }
        else  {
            if (getMouseCursor() != juce::MouseCursor::DraggingHandCursor) {
                setMouseCursor(juce::MouseCursor::DraggingHandCursor);
            }
        }
    }

    
    bool isSelected = false;
private:
    juce::Colour backgroundColour = ColourPalette::clipBackground;
    juce::Colour waveformColour = ColourPalette::waveformColour;
};

