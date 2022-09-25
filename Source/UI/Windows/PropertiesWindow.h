/*
  ==============================================================================

    PropertiesWindow.h
    Created: 24 Sep 2022 8:06:11am
    Author:  Jonny McGregor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../../defs.h"
#include "Tabs/AudioPropertiesTab.h"

class PropertiesPanel : public juce::Component {
public:
    PropertiesPanel(std::shared_ptr<juce::AudioDeviceManager> deviceManager)
    {
        propertiesTabs = std::make_unique<juce::TabbedComponent>(juce::TabbedButtonBar::Orientation::TabsAtLeft);
        propertiesTabs->setColour(juce::TabbedComponent::ColourIds::backgroundColourId, juce::Colours::azure);
        audioPropertiesTab.reset(new AudioPropertiesTab(deviceManager));
        propertiesTabs->addTab("Audio Devices", ColourPalette::colourAlternate, audioPropertiesTab.get(), true, 1);
        addAndMakeVisible(propertiesTabs.get());
    }
    ~PropertiesPanel(){};

    void paint(juce::Graphics &g) override
    {
        g.fillAll(juce::Colours::black);
    }
    void resized() override
    {
        propertiesTabs->setBounds(0, 0, this->getWidth(), this->getHeight());
    }

private:
    std::unique_ptr<juce::TabbedComponent> propertiesTabs;
    std::unique_ptr<AudioPropertiesTab> audioPropertiesTab;
};

class PropertiesWindow : public juce::DocumentWindow {
public:
    PropertiesWindow(std::shared_ptr<PropertiesPanel> propertiesPanel, std::shared_ptr<juce::ApplicationCommandManager> cmdManager, juce::String title, juce::Colour backgroundColour) 
        : DocumentWindow(title, backgroundColour, DocumentWindow::closeButton, true)
    {
        setContentComponent(propertiesPanel.get());
        this->cmdManager = cmdManager;
    }
    virtual void closeButtonPressed() {
        cmdManager->invoke(CommandIDs::togglePropertiesWindow, true);
    }

private:
    std::shared_ptr<juce::ApplicationCommandManager> cmdManager;
};

