#pragma once
#include <iostream>
#include <optional>
#include <string>

struct DecoratedWindow : public juce::Component {
    std::string title;
    juce::Colour backgroundColour = juce::Colours::black;
    juce::Colour outlineColour = juce::Colours::white;

    DecoratedWindow(int width, int height) {
        this->title = "";
        setSize(width, height);
    }

    DecoratedWindow(std::string newTitle, int width, int height) {
        this->title = newTitle;
        setSize(width, height);
    }

    void paint(juce::Graphics &screen) {
        screen.fillAll(backgroundColour);
        juce::Rectangle<int> area = {0, 0, getWidth(), getHeight()};
        screen.setColour(outlineColour);        
        screen.drawRect(area);
        if(title != "") { screen.drawText(title, 0, 8, getWidth(), getHeight() * 0.05, juce::Justification::centred, false); }
    };

    void mouseDown(const juce::MouseEvent& m) override {

    }
};