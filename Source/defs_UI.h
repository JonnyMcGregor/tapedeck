/*
  ==============================================================================

    defs.h
    Created: 4 Sep 2022 11:14:55am
    Author:  Jonny McGregor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


namespace juce {
    namespace Colours {
        const Colour OuterSpaceCrayola {0xff36413e};
        const Colour DavysGrey {0xff5d5e60};
        const Colour TaupeGrey {0xff8d8d92};
        const Colour Thistle {0xffbeb2c8};
        const Colour Opal {0xffbdcfca};
        const Colour MagicMint {0xffbcebcb};
    }
}

namespace ColourPalette {
    const juce::Colour colourPrimary = juce::Colours::Thistle;
    const juce::Colour colourSecondary = juce::Colours::MagicMint;
    const juce::Colour colourAlternate = juce::Colours::Opal;
    const juce::Colour colourGreyLight = juce::Colours::TaupeGrey;
    const juce::Colour colourGreyDark = juce::Colours::DavysGrey;
    const juce::Colour colourDark = juce::Colours::OuterSpaceCrayola;
}
