#pragma once
#include "TrackWidget.hpp"
#include "../../defs.h"
struct TrackStack : public juce::Viewport, public juce::MouseListener {
    
    TrackStack(int sampleRate, std::shared_ptr<juce::ApplicationCommandManager> cmdManager) 
    {
        
        this->cmdManager = cmdManager;
        
        setViewedComponent(&trackArea);
        timeRuler = make_shared<TimeRuler>(sampleRate);
        addAndMakeVisible(timeRuler.get());

        playhead = std::make_unique<juce::DrawableRectangle>();
        playhead->setFill(ColourPalette::colourGreyLight);
        playhead->setAlwaysOnTop(true);
        addAndMakeVisible(playhead.get());

        trackArea.addMouseListener(this, true);
    }

    void createTrackWidget(std::shared_ptr<Track> track) {
        this->trackWidgets.push_back(std::make_unique<TrackWidget>(track, timeRuler));
        trackArea.addAndMakeVisible(trackWidgets.back().get());
        resized();
    }
    
    void process() {
        for (int i = 0; i < trackWidgets.size(); i++) {
            trackWidgets[i]->updateClipWidgets();
        }
    }

    void paint(juce::Graphics &screen) override{
        screen.fillAll(ColourPalette::colourPrimary);
        for (auto &trackWidget : trackWidgets) {
            trackWidget->repaint();
        }
        playhead->repaint();
    }

    void resized() override
    {
        auto bounds = getBounds();

        u_int widgetY = 0;
        u_int widgetHeight = 150;
        timeRuler->setBounds(getX() + (getWidth() * 0.1), 0, getWidth() * 0.9, 30);

        widgetY += 30;
        for(auto &subWidget : trackWidgets)
        {
            subWidget->setBounds(0, widgetY, getWidth(), widgetHeight);
            widgetY += widgetHeight;
        }

        trackArea.setBounds(0, 30, getWidth(), widgetY);
    }

    void mouseDown(const juce::MouseEvent &e) override
    {
        for (auto &track : trackWidgets) {
            track->track->isSelected = false;
        }

        // when in clip area, update playhead position
        if (e.getPosition().getX() > trackWidgets[0]->trackBar->getWidth()) {
            updatePlayheadPosition(e.getPosition().getX(), true);
            cmdManager->invokeDirectly(CommandIDs::updatePlayheadInModel, true); // Updates the current playhead time in the model
        }

        repaint();
    }
    //Updates the UI Position of the playhead either using Samples or Pixels
    void updatePlayheadPosition(int xPosition, bool usingPixelPosition) {
        float playheadX = usingPixelPosition ? xPosition : trackWidgets[0]->trackBar->getWidth() + timeRuler->timeInSamplesToXPosition(xPosition);
        juce::Rectangle<float> r = {playheadX, 0, 2.0f, (float)getHeight()};
        playhead->setRectangle(juce::Parallelogram<float>(r));
        repaint();
    }
    //Gets The Playhead X Position in the Clip Area
    int getPlayheadXPosition()
    {
        return playhead->getX() - trackWidgets[0]->trackBar->getWidth();
    }

    juce::Component trackArea;
    std::unique_ptr<juce::DrawableRectangle> playhead;
    juce::Colour backgroundColour = ColourPalette::colourDark;
    std::vector<std::shared_ptr<TrackWidget>> trackWidgets;
    std::shared_ptr<TimeRuler> timeRuler;
    std::shared_ptr<juce::ApplicationCommandManager> cmdManager;
};
