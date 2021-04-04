#include "../components/point.hpp"
#include "../components/widget.hpp"
#include <vector>

struct VBoxLayout : Widget {
    std::vector<Widget*> sub_widgets = {};

    VBoxLayout() {
    }

    void add_sub_widget(Widget &sub_widget) {
        this->sub_widgets.push_back(&sub_widget);
    }

    void process(std::vector<KeyPress> &keyboard_input) {}

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
