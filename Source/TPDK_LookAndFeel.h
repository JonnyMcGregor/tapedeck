/*
  ==============================================================================

    TPDK_LookAndFeel.h
    Created: 16 Oct 2021 6:33:58pm
    Author:  jonny

  ==============================================================================
*/
#include "Fracture/Widgets/TrackToggleButton.h"
#include "JuceHeader.h"
#pragma once
class TPDK_LookAndFeel : public juce::LookAndFeel_V4
{
public:
	void drawToggleButton(juce::Graphics &g, juce::ToggleButton &but,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
	{
        juce::Rectangle<int> area = { but.getWidth(), but.getHeight()};

		if (but.getToggleState())
		{
            g.setColour(shouldDrawButtonAsHighlighted ? but.findColour(juce::ToggleButton::tickColourId).brighter() : 
                                                        but.findColour(juce::ToggleButton::tickColourId));
		}
		else
		{
            g.setColour(shouldDrawButtonAsHighlighted ? ColourPalette::colourPrimary.darker() : juce::Colours::transparentWhite);
		}

		g.fillRect(area);
        g.setColour(but.findColour(juce::ToggleButton::textColourId));
        g.drawFittedText(but.getButtonText(), area, juce::Justification::centred, 1);
        g.drawRect(area);

	}
};