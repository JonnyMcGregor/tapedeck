/*
  ==============================================================================

    PropertiesWindow.h
    Created: 24 Sep 2022 8:06:11am
    Author:  Jonny McGregor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../defs_UI.h"

class PropertiesWindow : public juce::DocumentWindow {
public:
    PropertiesWindow(const juce::String &name,
                     juce::Colour backgroundColour,
                     int requiredButtons, std::shared_ptr<juce::ApplicationCommandManager> cmdManager) : DocumentWindow(name, backgroundColour, requiredButtons, true)
    {
        this->cmdManager = cmdManager;
    }
    ~PropertiesWindow(){};
    virtual void closeButtonPressed()
    {
        cmdManager->invoke(togglePropertiesWindow, true);
    }

private:
    std::shared_ptr<juce::ApplicationCommandManager> cmdManager;
};
