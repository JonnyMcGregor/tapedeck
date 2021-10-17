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
	void drawToggleButton(Graphics &g, ToggleButton &but,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
	{
        juce::Rectangle<int> area = { but.getWidth(), but.getHeight()};

		if (but.getToggleState())
		{
            g.setColour(shouldDrawButtonAsHighlighted ? but.findColour(ToggleButton::tickColourId).brighter() : but.findColour(ToggleButton::tickColourId));
		}
		else
		{
            g.setColour(shouldDrawButtonAsHighlighted ? Colours::black.brighter() : Colours::black);
		}

		g.fillRect(area);
        g.setColour(but.findColour(ToggleButton::textColourId));
        g.drawFittedText(but.getButtonText(), area, Justification::centred, 1);
        g.drawRect(area);

	}
};