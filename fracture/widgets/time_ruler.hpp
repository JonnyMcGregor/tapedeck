#pragma once
#include "../components/widget.hpp"
#include <memory>
struct TimeRuler : Widget {
    int cells_per_second = 60;
    int cells_per_time_marker = 20;
    int start_time_on_screen_in_samples = 0;
    int sample_rate = 0;
    int window_size_in_samples = 0;
    TimeRuler(int sample_rate) { this->sample_rate = sample_rate; }

    void process(std::vector<KeyPress> &keyboard_input) {}

    void render(Screen &screen) {
        DecoratedWindow border;
        Screen ruler = {screen.width - 2, 1};
        window_size_in_samples = ruler.width * samples_per_cell();
        ScreenCellStyle time_marker_style, second_marker_style;
        time_marker_style.foreground_colour = Colour(1, 1, 1);
        second_marker_style.foreground_colour = Colour(0.5, 1, 1);
        for (int x = 0; x < screen.width; x++) {
            if (x % cells_per_time_marker == 0) {

                std::string time = to_string((1.0 * x / cells_per_second) + (1.0 * start_time_on_screen_in_samples / sample_rate));
                time.resize(time.find(".") + 2);
                ruler.set_style(time_marker_style);
                ruler.draw(Point(x, 0), time);
            } else if (x % (cells_per_second / 5) == 0) {
                ruler.set_style(second_marker_style);
                ruler.draw(Point(x, 0), "|");
            }
        }
        border.render(screen);
        screen.draw(Point(1, 1), ruler);
    }

    int samples_per_cell() {
        return (sample_rate / cells_per_second);
    }
};