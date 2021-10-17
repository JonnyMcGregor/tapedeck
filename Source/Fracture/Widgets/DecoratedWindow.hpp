#pragma once
#include "../Components/Point.hpp"
#include "../Components/Widget.hpp"
#include <iostream>
#include <optional>
#include <string>

struct DecoratedWindow : juce::Component {
    std::string title;
    std::optional<Widget *> subWidget;
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

    // Takes a reference
    void set_sub_widget(Widget &subWidget) {
        // Set this->subWidget pointer to point to the _address_ of subWidget.
        this->subWidget = &subWidget;
    }

    //void process(std::vector<KeyPress> &keyboardInput) {}

    void paint(juce::Graphics &screen) {
        screen.fillAll(backgroundColour);
        juce::Rectangle<int> area = {0, 0, getWidth(), getHeight()};
        screen.setColour(outlineColour);        
        screen.drawRect(area);
        if(title != "") { screen.drawText(title, 0, 8, getWidth(), getHeight() * 0.05, juce::Justification::centred, false); }
    };
};