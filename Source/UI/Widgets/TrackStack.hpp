#pragma once
#include "TrackWidget.hpp"
#include "../../defs.h"
struct TrackStack : public juce::Component {
    
    TrackStack(int sampleRate, std::shared_ptr<juce::ApplicationCommandManager> cmdManager) 
    {
        timeRuler = make_shared<TimeRuler>(sampleRate);
        addAndMakeVisible(timeRuler.get());

        playhead = std::make_unique<juce::DrawableRectangle>();
        playhead->setFill(ColourPalette::colourGreyLight);
        playhead->setAlwaysOnTop(true);
        addAndMakeVisible(playhead.get());
        this->cmdManager = cmdManager;
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
        for (auto &subwidget : subWidgets) {
            subwidget->repaint();
        }
        playhead->repaint();
    }
    void resized() override
    {
        auto bounds = getBounds();
        u_int widgetY = 0;
        u_int widgetHeight = this->subWidgets.size() > 0 ? this->getHeight() / this->subWidgets.size() : this->getHeight();

        for(auto &subWidget : subWidgets)
        {
            subWidget->setBounds(0, widgetY, getWidth(), widgetHeight);
            widgetY += widgetHeight;
        }
        timeRuler->setBounds(getX() + (getWidth() * 0.1), getY() - (getHeight() * 0.03), getWidth() * 0.9, getHeight() * 0.1);
    }

    void mouseDown(const juce::MouseEvent &e) override
    {
        for (auto &track : subWidgets) {
            track->track->isSelected = false;
        }

        // when in clip area, update playhead position
        if (e.getPosition().getX() > subWidgets[0]->trackBar->getWidth()) {
            updatePlayheadPosition(e.getPosition().getX(), true);
            cmdManager->invokeDirectly(CommandIDs::updatePlayheadInModel, true); // Updates the current playhead time in the model
        }

        repaint();
    }

 
    void updatePlayheadPosition(int xPosition, bool usingPixelPosition) {
        float playheadX = usingPixelPosition ? xPosition : subWidgets[0]->trackBar->getWidth() + timeRuler->timeInSamplesToXPosition(xPosition);
        juce::Rectangle<float> r = {playheadX, 0, 2.0f, (float)getHeight()};
        playhead->setRectangle(juce::Parallelogram<float>(r));
        repaint();
    }
    //Gets The Playhead X Position in the Clip Area
    int getPlayheadXPosition()
    {
        return playhead->getX() - subWidgets[0]->trackBar->getWidth();
    }

    std::unique_ptr<juce::DrawableRectangle> playhead;
    juce::Colour backgroundColour = ColourPalette::colourDark;
    std::vector<std::shared_ptr<TrackWidget>> subWidgets;
    std::shared_ptr<TimeRuler> timeRuler;
    std::shared_ptr<juce::ApplicationCommandManager> cmdManager;
};
