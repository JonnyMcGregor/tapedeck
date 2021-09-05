#pragma once
#include "../Components/Widget.hpp"
#include <memory>
struct TimeRuler : Widget {
    float cellsPerSecond = 60.0f;
    int cellsPerTimeMarker = 20;
    int startTimeOnScreenInSamples = 0;
    int sampleRate = 0;
    int windowSizeInSamples = 0;
    TimeRuler(int sampleRate) { this->sampleRate = sampleRate; }

    void process(std::vector<KeyPress> &keyboardInput) {}

    void render(Screen &screen) {
        DecoratedWindow border;
        Screen ruler = {screen.width - 2, 1};
        windowSizeInSamples = ruler.width * samplesPerCell();
        ScreenCellStyle time_marker_style, second_marker_style;
        time_marker_style.foregroundColour = Colour(1, 1, 1);
        second_marker_style.foregroundColour = Colour(0.5, 1, 1);

        cellsPerTimeMarker = screen.width/5;
        int cells_per_micro_marker = screen.width / 15;
        for (int x = 0; x < screen.width; x++) {
            if (x % cellsPerTimeMarker == 0) {
                std::string time = to_string((1.0 * x / cellsPerSecond) + (1.0 * startTimeOnScreenInSamples / sampleRate));
                time.resize(time.find(".") + 2);
                ruler.set_style(time_marker_style);
                ruler.draw(Point(x, 0), time);
            } else if (x % cells_per_micro_marker == 0) {
                ruler.set_style(second_marker_style);
                ruler.draw(Point(x, 0), "|");
            }
        }
        border.render(screen);
        screen.draw(Point(1, 1), ruler);
    }

    int samplesPerCell() {
        return (sampleRate / cellsPerSecond);
    }
};