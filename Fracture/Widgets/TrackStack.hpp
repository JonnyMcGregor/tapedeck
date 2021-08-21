#pragma once
#include "TrackWidget.hpp"
#include "VboxLayout.hpp"
struct TrackStack : Widget {
    std::vector<std::unique_ptr<TrackWidget>> subWidgets;
    std::shared_ptr<TimeRuler> timeRuler;
    TrackStack(std::shared_ptr<TimeRuler> timeRuler) { this->timeRuler = timeRuler; }

    void createTrackSubWidgets(std::shared_ptr<Track> track) {
        this->subWidgets.push_back(std::make_unique<TrackWidget>(track, timeRuler));
    }

    void process(std::vector<KeyPress> &keyboardInput) {
        for (int i = 0; i < subWidgets.size(); i++) {
            subWidgets[i]->process(keyboardInput);
        }
    }

    void render(Screen &screen) {
        if (this->subWidgets.size() == 0) {
            return;
        }
        u_short widgetY = 0;
        u_short widgetHeight = screen.height / this->subWidgets.size();

        for (u_short i = 0; i < this->subWidgets.size(); i++) {
            Screen subWidgetScreen = Screen(screen.width, widgetHeight);
            this->subWidgets[i]->render(subWidgetScreen);
            screen.draw(Point(0, widgetY), subWidgetScreen);
            widgetY += widgetHeight;
        };
    }
};
