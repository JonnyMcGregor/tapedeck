/*
  ==============================================================================

    AudioPropertiesTab.h
    Created: 25 Sep 2022 11:17:27am
    Author:  Jonny McGregor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class AudioPropertiesTab : public juce::Component {
public:
    AudioPropertiesTab(std::shared_ptr<juce::AudioDeviceManager> deviceManager) {
        deviceSelector.reset(new juce::AudioDeviceSelectorComponent(*deviceManager, 0, 32, 0, 8, true, true, true, false));
        addAndMakeVisible(deviceSelector.get());
    }
    ~AudioPropertiesTab() {}

    void resized() override {
        deviceSelector->setBounds(10, 40, getWidth() - 20, getHeight() - 80);
    }
    void paint(juce::Graphics &g) override {
    }

private:
    std::shared_ptr<juce::AudioDeviceSelectorComponent> deviceSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPropertiesTab)
};

