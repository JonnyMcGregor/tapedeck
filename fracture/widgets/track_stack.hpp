#include "track_widget.hpp"
#include "vbox_layout.hpp"
struct TrackStack : Widget {
    std::vector<std::unique_ptr<TrackWidget>> sub_widgets;
    std::shared_ptr<TimeRuler> time_ruler;
    TrackStack(std::shared_ptr<TimeRuler> time_ruler) { this->time_ruler = time_ruler; }

    void create_track_sub_widget(std::shared_ptr<Track> track) {
        this->sub_widgets.push_back(std::make_unique<TrackWidget>(track, time_ruler));
    }

    void process(std::vector<KeyPress> &keyboard_input) {
        for (int i = 0; i < sub_widgets.size(); i++) {
            sub_widgets[i]->process(keyboard_input);
        }
    }

    void render(Screen &screen) {
        if (this->sub_widgets.size() == 0) {
            return;
        }
        u_short widget_y = 0;
        u_short widget_height = screen.height / this->sub_widgets.size();

        for (u_short i = 0; i < this->sub_widgets.size(); i++) {
            Screen sub_widget_screen = Screen(screen.width, widget_height);
            this->sub_widgets[i]->render(sub_widget_screen);
            screen.draw(Point(0, widget_y), sub_widget_screen);
            widget_y += widget_height;
        };
    }
};
