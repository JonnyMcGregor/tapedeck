#pragma once
#include "../../Seismic/Components/Track.hpp"
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

    bool isSelected = false;
    TrackWidget(std::shared_ptr<Track> track, std::shared_ptr<TimeRuler> timeRuler) {
        this->track = track;
        for (int i = 0; i < track->clips.size(); i++) {
            clipWidgets.push_back(std::make_shared<ClipWidget>(track->clips[i], timeRuler, track->clipMetadata[i].startTime));
            addAndMakeVisible(clipWidgets.back().get());
        }
        this->timeRuler = timeRuler;
        trackBar = std::make_unique<DecoratedWindow>(track->name, getWidth() * 0.1, getHeight());
        clipArea = std::make_unique<DecoratedWindow>(getWidth() * 0.9, getHeight());

        but_recordArm = std::make_unique<TrackToggleButton>();
        but_recordArm->setButtonText("R");
        but_recordArm->setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::red);
        but_solo = std::make_unique<TrackToggleButton>();
        but_solo->setButtonText("S");
        but_solo->setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::red);
        but_mute = std::make_unique<TrackToggleButton>();
        but_mute->setButtonText("M");
        but_mute->setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::red);

        addAndMakeVisible(trackBar.get());
        addAndMakeVisible(clipArea.get());
        addAndMakeVisible(but_recordArm.get());
        addAndMakeVisible(but_solo.get());
        addAndMakeVisible(but_mute.get());
    }

    void updateClipWidgets() {
        clipWidgets.clear();
        for (int i = 0; i < track->clips.size(); i++) {
            clipWidgets.emplace_back(std::make_shared<ClipWidget>(track->clips[i], timeRuler, track->clipMetadata[i].startTime));
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

    void paint(juce::Graphics &screen) override {}

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
                    clipWidgets[i]->setBounds(area.getX() + calculateClipX(i), area.getY(), calculateClipWidth(i), area.getHeight());
                    clipWidgets[i]->repaint();
                }
            }
        }
    }

    bool clipIsDrawable(int clipIndex) {
        if (timeRuler->startTimeOnScreenInSamples + timeRuler->windowSizeInSamples <= track->clipMetadata[clipIndex].startTime ||
            track->clipMetadata[clipIndex].startTime + track->clips[clipIndex]->size() <= timeRuler->startTimeOnScreenInSamples) {
            return false;
        } else if (calculateClipWidth(clipIndex) == 0) {
            return false;
        } else {
            return true;
        }
    }
    int calculateClipWidth(int clipIndex) {
        return (calculateClipEndSample(clipIndex) - calculateClipStartSample(clipIndex)) / timeRuler->samplesPerCell();
    }

    int calculateClipStartSample(int clipIndex) {
        return max((int)track->clipMetadata[clipIndex].startTime, timeRuler->startTimeOnScreenInSamples);
    }
    int calculateClipEndSample(int clipIndex) {
        return min((int)(track->clipMetadata[clipIndex].startTime + track->clips[clipIndex]->size()), timeRuler->startTimeOnScreenInSamples + timeRuler->windowSizeInSamples);
    }

    int calculateClipX(int clipIndex) {
        return max((int)(track->clipMetadata[clipIndex].startTime - timeRuler->startTimeOnScreenInSamples) / timeRuler->samplesPerCell(), 0);
    }
};