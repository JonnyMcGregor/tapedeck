#pragma once
#include "../Components/Widget.hpp"
#include "../../Seismic/Components/Playhead.hpp"
#include <JuceHeader.h>

struct PlayheadWidget : juce::Component
{
public:
    PlayheadWidget(){}

	void paint(Graphics& g) override
	{
        g.setColour(Colours::white);
        g.fillAll();
	}
    void resized() override {}

private:
};
