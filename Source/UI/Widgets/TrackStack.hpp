#pragma once
#include "TrackWidget.hpp"
#include "../../defs.h"
struct TrackStack : public juce::Component {
    
    TrackStack(int sampleRate, std::shared_ptr<juce::ApplicationCommandManager> cmdManager) 
    {
        this->cmdManager = cmdManager;

        timeRuler = make_shared<TimeRuler>(sampleRate);
        addAndMakeVisible(timeRuler.get());

        playhead = std::make_unique<juce::DrawableRectangle>();
        playhead->setFill(ColourPalette::playheadColour);
        playhead->setAlwaysOnTop(true);
        addAndMakeVisible(playhead.get());

        scrollableViewport.setViewedComponent(&trackArea);
        scrollableViewport.addMouseListener(this, true);
        scrollableViewport.setScrollBarsShown(true, false);
        addAndMakeVisible(&scrollableViewport);
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
        screen.fillAll(ColourPalette::mainBackground);
        for (auto &trackWidget : trackWidgets) {
            trackWidget->repaint();
        }
    }

    void resized() override
    {
        auto bounds = getBounds();
        u_int widgetY = 0;
        u_int widgetHeight = 150;

        scrollableViewport.setBounds(0, 30, getWidth(), getHeight() - 30);
        
        for(auto &subWidget : trackWidgets)
        {
            subWidget->setBounds(0, widgetY, getWidth(), widgetHeight);
            subWidget->resized();
            widgetY += widgetHeight;
        }

        trackArea.setBounds(0, 0, getWidth(), widgetY);
        timeRuler->setBounds(trackWidgets[0]->clipArea->getX(), 0, getWidth() * 0.9, 30);

    }

    void mouseDown(const juce::MouseEvent &e) override
    {
        for (auto &track : trackWidgets) {
            track->track->isSelected = false;
        }
        int positionX = e.getEventRelativeTo(&scrollableViewport).getPosition().getX();
        // when in clip area, update playhead position
        if (positionX >= trackWidgets[0]->clipArea->getX()) {
            updatePlayheadPosition(positionX, true);
            cmdManager->invokeDirectly(CommandIDs::updatePlayheadInModel, false); // Updates the current playhead time in the model
        }

        repaint();
    }
    void mouseUp(const juce::MouseEvent &e) override {
        if (e.mouseWasDraggedSinceMouseDown()) {
            auto relativeEvent = e.getEventRelativeTo(this);
            // Clip Vertical Dragging Logic - done in trackstack as clip will be dragged between tracks
            if (auto currentClip = dynamic_cast<ClipWidget *>(e.originalComponent)) { // Check if original component is a ClipWidget
                auto parentTrack = static_cast<TrackWidget *>(currentClip->getParentComponent()); //Get parent TrackWidget component
                
                for (int t = 0; t < trackWidgets.size(); t++) {
                    // Check if clip is dragged to a different track
                    if (parentTrack != trackWidgets[t].get() &&
                        (trackWidgets[t]->getY() < relativeEvent.getPosition().getY() && 
                        trackWidgets[t]->getBottom() > relativeEvent.getPosition().getY())) {

                        for (int c = 0; c < parentTrack->clipWidgets.size(); c++) {
                            if (parentTrack->clipWidgets[c].get() == currentClip) {
                                auto clipCopy = (parentTrack->track->clips[c]);
                                auto clipMetadataCopy = (parentTrack->track->clipMetadata[c]);
                                trackWidgets[t]->track->clips.push_back(clipCopy);
                                trackWidgets[t]->track->clipMetadata.push_back(clipMetadataCopy);
                                parentTrack->track->deleteClip(c);

                                trackWidgets[t]->updateClipWidgets();
                                trackWidgets[t]->resized();
                                parentTrack->updateClipWidgets();
                                parentTrack->resized();
                            }  
                        }
                    }
                }
            }
        }
       
    }
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override {
        int scrollThreshold = 0;
        if (wheel.deltaX > scrollThreshold || wheel.deltaX < scrollThreshold) {
            //if horizontal scrolling
            timeRuler->setStartTimeOnScreenInSamples(timeRuler->getStartTimeOnScreenInSamples() + timeRuler->samplesPerPixel() * wheel.deltaX * -15);
            //cmdManager->invokeDirectly(CommandIDs::updateTimeWindowInModel, false); // Updates the current time window in the model
            timeRuler->repaint();
            resized();
        }
     
    }
    //Updates the UI Position of the playhead either using Samples or Pixels
    void updatePlayheadPosition(int xPosition, bool usingPixelPosition) {
        float playheadX = usingPixelPosition ? xPosition : trackWidgets[0]->trackBar->getWidth() + timeRuler->timeInSamplesToXPosition(xPosition);
        juce::Rectangle<float> r = {playheadX, 0, 2.0f, (float)getHeight()};
        playhead->setRectangle(juce::Parallelogram<float>(r));
        playhead->repaint();
    }
    //Gets The Playhead X Position in the Clip Area
    int getPlayheadXPosition()
    {
        return playhead->getX() - trackWidgets[0]->trackBar->getWidth();
    }
    juce::Viewport scrollableViewport;
    juce::Component trackArea;
    std::unique_ptr<juce::DrawableRectangle> playhead;
    juce::Colour backgroundColour = ColourPalette::mainBackground;
    std::vector<std::shared_ptr<TrackWidget>> trackWidgets;
    std::shared_ptr<TimeRuler> timeRuler;
    std::shared_ptr<juce::ApplicationCommandManager> cmdManager;
};
