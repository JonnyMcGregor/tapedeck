#pragma once
#include "../../Model/Components/Track.hpp"
#include "ClipWidget.hpp"
#include "DecoratedWindow.hpp"
#include "TrackToggleButton.h"

struct TrackWidget : public juce::Component, public juce::Button::Listener {
    std::shared_ptr<Track> track;
    std::vector<std::shared_ptr<ClipWidget>> clipWidgets;
    std::shared_ptr<TimeRuler> timeRuler;
    std::unique_ptr<DecoratedWindow> clipArea;
    std::unique_ptr<DecoratedWindow> trackBar;
    std::unique_ptr<TrackToggleButton> but_recordArm;
    std::unique_ptr<TrackToggleButton> but_solo;
    std::unique_ptr<TrackToggleButton> but_mute;

    std::shared_ptr<juce::MouseEvent> lastMouseDragEvent;
    bool isRecording = false;
    int mouseDownWithinClip = 0; //used for clip dragging

    TrackWidget(std::shared_ptr<Track> track, std::shared_ptr<TimeRuler> timeRuler) {
        this->track = track;
        for (int i = 0; i < track->clips.size(); i++) {
            clipWidgets.push_back(std::make_shared<ClipWidget>(track->clips[i], timeRuler));
            clipWidgets.back()->addMouseListener(this, true);
            addAndMakeVisible(clipWidgets.back().get());
        }
        this->timeRuler = timeRuler;
        trackBar = std::make_unique<DecoratedWindow>(track->name, getWidth() * 0.1, getHeight());
        clipArea = std::make_unique<DecoratedWindow>(getWidth() * 0.9, getHeight());
        but_recordArm = std::make_unique<TrackToggleButton>();
        but_recordArm->setButtonText("R");
        but_recordArm->setColour(juce::ToggleButton::ColourIds::tickColourId, ColourPalette::buttonActive);
        but_recordArm->addListener(this);
        but_solo = std::make_unique<TrackToggleButton>();
        but_solo->setButtonText("S");
        but_solo->setColour(juce::ToggleButton::ColourIds::tickColourId, ColourPalette::buttonActive);
        but_solo->addListener(this);
        but_mute = std::make_unique<TrackToggleButton>();
        but_mute->setButtonText("M");
        but_mute->setColour(juce::ToggleButton::ColourIds::tickColourId, ColourPalette::buttonActive);
        but_mute->addListener(this);

        addAndMakeVisible(trackBar.get());
        addAndMakeVisible(clipArea.get());
        addAndMakeVisible(but_recordArm.get());
        addAndMakeVisible(but_solo.get());
        addAndMakeVisible(but_mute.get());
    }
    
    void updateClipWidgets() {
        clipWidgets.clear();
        for (int i = 0; i < track->clips.size(); i++) {
            clipWidgets.emplace_back(std::make_shared<ClipWidget>(track->clips[i], timeRuler));
            clipWidgets.back()->addMouseListener(this, true);
            addAndMakeVisible(clipWidgets.back().get());
        }
        resized();
    }

    void refresh() {
        if (track->solo != but_solo->getToggleState()) {
            but_solo->setToggleState(track->solo, juce::dontSendNotification);
        }
        if (track->mute != but_mute->getToggleState()) {
            but_mute->setToggleState(track->mute, juce::dontSendNotification);
        }
        if (track->recordArmed != but_recordArm->getToggleState()) {
            but_recordArm->setToggleState(track->recordArmed, juce::dontSendNotification);
        }
    }

    void buttonClicked(juce::Button *but) override {
		if (but == but_recordArm.get())
		{
            track->recordArmed = but->getToggleState();
		}
        else if (but == but_solo.get()) 
		{
            track->solo = but->getToggleState();
        }
        else if (but == but_mute.get()) 
		{
            track->mute = but->getToggleState();
        }
    }

    void paint(juce::Graphics &screen) override {
        trackBar->backgroundColour = track->isSelected ? ColourPalette::trackBarBackground : ColourPalette::trackBarBackground.brighter();
        trackBar->outlineColour = ColourPalette::textColour;
        clipArea->backgroundColour = ColourPalette::clipAreaBackground;
        clipArea->outlineColour = ColourPalette::clipAreaBackground;
        if (isRecording) {
            clipArea->backgroundColour = ColourPalette::buttonActive;
        }
    }

    void resized() override {
        trackBar->setBounds(0, 0, getWidth() * 0.1, getHeight());
        juce::Rectangle<float> toggleBtnArea = {trackBar->getWidth() - 2.0f, trackBar->getHeight() - 2.0f};
        float btnHeight = toggleBtnArea.getHeight() * 0.15f;
        but_solo->setBounds(toggleBtnArea.getCentreX() - (btnHeight * 0.5), toggleBtnArea.getCentreY() - (btnHeight * 0.5), btnHeight, btnHeight);
        but_recordArm->setBounds(but_solo->getX() - btnHeight - 5, but_solo->getY(), btnHeight, btnHeight);
        but_mute->setBounds(but_solo->getX() + btnHeight + 5, but_solo->getY(), btnHeight, btnHeight);

        clipArea->setBounds(trackBar->getRight(), 0, getWidth() - trackBar->getWidth(), getHeight());
        juce::Rectangle<float> clipDataArea = {clipArea->getX() + 5.0f, clipArea->getY() + (clipArea->getHeight() * 0.05f), clipArea->getWidth() - 2.0f, clipArea->getHeight() * 0.9f};
        drawClips(clipDataArea);
    }

    void drawClips(juce::Rectangle<float> area) {
        if (clipWidgets.size() > 0) {
            for (int i = 0; i < clipWidgets.size(); i++) {
                if (clipIsDrawable(i)) {
                    clipWidgets[i]->setVisible(true);
                    clipWidgets[i]->setBounds(area.getX() + calculateClipX(i), area.getY(), calculateClipPixelWidth(i), area.getHeight());
                }
                else {
                    clipWidgets[i]->setVisible(false);
                    juce::Logger::getCurrentLogger()->writeToLog("Start Time Of Window: " + juce::String(timeRuler->getStartTimeOnScreenInSamples()));
                    juce::Logger::getCurrentLogger()->writeToLog("End Time Of Window: " + juce::String(timeRuler->getWindowSizeInSamples() + timeRuler->getStartTimeOnScreenInSamples()));
                    juce::Logger::getCurrentLogger()->writeToLog("Start Time Of Clip: " + juce::String(clipWidgets[i]->clip->metadata.getStartTimeInSession() + clipWidgets[i]->clip->metadata.getStartTimeInSourceFile()));
                    juce::Logger::getCurrentLogger()->writeToLog("End Time Of Clip: " + juce::String(clipWidgets[i]->clip->metadata.getStartTimeInSession() + clipWidgets[i]->clip->size() - clipWidgets[i]->clip->metadata.getEndTimeInSourceFile()));
                }
                repaint();
            }
        }
    }

    bool clipIsDrawable(int clipIndex)
    {
        if (calculateClipPixelWidth(clipIndex) <= 0)
        {
            return false;
        }
        
        int startTimeOnScreenInSamples = timeRuler->getStartTimeOnScreenInSamples();
        int windowSizeInSamples = timeRuler->getWindowSizeInSamples();
        auto clipWidget = clipWidgets[clipIndex];
        int clipStartTimeInSamples = clipWidget->clip->metadata.getStartTimeInSession() + clipWidget->clip->metadata.getStartTimeInSourceFile();
        int clipEndTimeInSamples = clipWidget->clip->metadata.getStartTimeInSession() + clipWidget->clip->size() - 1 - clipWidget->clip->metadata.getEndTimeInSourceFile();

        return (clipStartTimeInSamples >= startTimeOnScreenInSamples && clipStartTimeInSamples <= startTimeOnScreenInSamples + windowSizeInSamples) ||
               (clipEndTimeInSamples >= startTimeOnScreenInSamples && clipEndTimeInSamples <= startTimeOnScreenInSamples + windowSizeInSamples);
        
    }

    int calculateClipPixelWidth(int clipIndex) {
        return (calculateLastVisibleSampleInClip(clipIndex) - calculateFirstVisibleSampleInClip(clipIndex)) / timeRuler->samplesPerPixel();
    }

    int calculateFirstVisibleSampleInClip(int clipIndex) {
        return max((int)clipWidgets[clipIndex]->clip->metadata.getStartTimeInSession(), timeRuler->getStartTimeOnScreenInSamples());
    }
    // Returns the sample index in absolute time which aligns with the last sample to be drawn in clip
    int calculateLastVisibleSampleInClip(int clipIndex) {
        // start time in clip + length of clip - start time in reference - end time in reference = sample index of last sample to be drawn
        return min((int)(clipWidgets[clipIndex]->clip->metadata.getStartTimeInSession() + (track->clips[clipIndex]->size() - 1) - clipWidgets[clipIndex]->clip->metadata.getStartTimeInSourceFile() - clipWidgets[clipIndex]->clip->metadata.getEndTimeInSourceFile()), timeRuler->getStartTimeOnScreenInSamples() + timeRuler->getWindowSizeInSamples());
    }

    int calculateClipX(int clipIndex) {
        return max(timeRuler->timeInSamplesToXPosition(clipWidgets[clipIndex]->clip->metadata.getStartTimeInSession() + clipWidgets[clipIndex]->clip->metadata.getStartTimeInSourceFile()), 0);
    }

    void mouseDown(const juce::MouseEvent &e) override
    {    
        track->isSelected = true;
        if (e.originalComponent->getX() >= clipArea->getX()) {
            for (int i = 0; i < clipWidgets.size(); i++) {
                if (clipWidgets[i].get() == e.originalComponent) {
                    clipWidgets[i]->isSelected = true;
                    mouseDownWithinClip = e.getMouseDownX();
                }
                else {
                    clipWidgets[i]->isSelected = false;
                }
            }
        }
        repaint();
    }

    void mouseDrag(const juce::MouseEvent& e) override {
        
        //Horizontal clip dragging logic
        if (e.originalComponent->getX() >= clipArea->getX()) {
            int lastDragX = lastMouseDragEvent ? lastMouseDragEvent->x : 0;
            auto eventInTrackScope = e.getEventRelativeTo(this);
            for (int i = 0; i < clipWidgets.size(); i++) {
                if (clipWidgets[i].get() == e.originalComponent) {
                    
                    if (eventInTrackScope.x == lastDragX) {
                        return;
                    }

                    //=== Clip Movement ===//
                    if (clipWidgets[i]->getMouseCursor() == juce::MouseCursor::DraggingHandCursor) { // Check the correct mouse cursor type for clip movement
                        int clipXInClipArea = max(e.getEventRelativeTo(this).getPosition().getX() - mouseDownWithinClip - clipArea->getX(), 0);
                        clipWidgets[i]->clip->metadata.setStartTimeInSession(timeRuler->xPositionToTimeInSamples(clipXInClipArea) - clipWidgets[i]->clip->metadata.getStartTimeInSourceFile());
                        break;
                    }
                    else if (clipWidgets[i]->getMouseCursor() == juce::MouseCursor::LeftEdgeResizeCursor) { // Left Clip Truncation Logic
                        // Need to set start time in source to mouse position to ensure clip is not being time shifted
                        if (eventInTrackScope.x != lastDragX && lastDragX != 0) {
                            int truncationAmountSamples = ((eventInTrackScope.x - lastDragX) * timeRuler->samplesPerPixel());
                            clipWidgets[i]->clip->metadata.setStartTimeInSourceFile(clipWidgets[i]->clip->metadata.getStartTimeInSourceFile() + truncationAmountSamples);                            
                        }
                        break;
                    }
                    else if (clipWidgets[i]->getMouseCursor() == juce::MouseCursor::RightEdgeResizeCursor) { // Right Clip Truncation Logic
                        if (eventInTrackScope.x != lastDragX && lastDragX != 0) {
                            int truncationAmountSamples = -1 * ((eventInTrackScope.x - lastDragX) * timeRuler->samplesPerPixel());
                            clipWidgets[i]->clip->metadata.setEndTimeInSourceFile(clipWidgets[i]->clip->metadata.getEndTimeInSourceFile() + truncationAmountSamples);
                        }
                        break;
                    }
                }
            }
            lastMouseDragEvent = std::make_shared<juce::MouseEvent>(e.getEventRelativeTo(this));
            resized();
        }
    }

    void mouseUp(const juce::MouseEvent& e) override {
        lastMouseDragEvent = nullptr;
    }
};