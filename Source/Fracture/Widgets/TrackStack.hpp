#pragma once
#include "TrackWidget.hpp"

struct TrackStack : public juce::Component {
    
    TrackStack(std::shared_ptr<TimeRuler> timeRuler) 
    { 
        this->timeRuler = timeRuler; 
        DecoratedWindow window = {"Track Stack", getWidth(), getHeight()};
        window.setTopLeftPosition(0,0);
        addAndMakeVisible(window);
    }

    void createTrackSubWidgets(std::shared_ptr<Track> track) {
        this->subWidgets.push_back(std::make_unique<TrackWidget>(track, timeRuler));
        addAndMakeVisible(subWidgets.back().get());
        resized();
    }
    
    void process() {
        for (int i = 0; i < subWidgets.size(); i++) {
            subWidgets[i]->updateClipWidgets();
        }
    }

    void paint(juce::Graphics &screen) override{
    }
    void resized() override
    {
        u_int widgetY = 0;
        u_int widgetHeight = this->subWidgets.size() > 0 ? this->getHeight() / this->subWidgets.size() : this->getHeight();

        for(auto &subWidget : subWidgets)
        {
            subWidget->setBounds(0, widgetY, getWidth(), widgetHeight);
            widgetY += widgetHeight;
        }
    }
    juce::Colour backgroundColour = juce::Colours::orange;
    std::vector<std::unique_ptr<TrackWidget>> subWidgets;
    std::shared_ptr<TimeRuler> timeRuler;
};
