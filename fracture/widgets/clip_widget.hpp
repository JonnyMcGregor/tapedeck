#pragma once
#include "../../seismic/components/Clip.hpp"
#include "../components/point.hpp"
#include "../components/screen.hpp"
#include "../components/widget.hpp"
#include <stdexcept>

struct ClipWidget : Widget {
    std::shared_ptr<Clip> clip;
    std::shared_ptr<TimeRuler> time_ruler; 
    int clip_start_time;
    ClipWidget(std::shared_ptr<Clip> clip, std::shared_ptr<TimeRuler> time_ruler, int clip_start_time) {
        this->clip = clip;
        this->time_ruler = time_ruler;
        this->clip_start_time = clip_start_time;
    }

    void process(std::vector<KeyPress> &keyboard_input) {}

    void render(Screen &screen) {
        int first_sample_to_draw = 0;
        int last_sample_to_draw = (clip->size() - 1);
        if(clip_start_time < time_ruler->start_time_on_screen_in_samples)
        {
            first_sample_to_draw = time_ruler->start_time_on_screen_in_samples - clip_start_time;
        }
        if(clip_start_time + clip->size() - 1 > time_ruler->start_time_on_screen_in_samples + time_ruler->window_size_in_samples)
        {
            last_sample_to_draw =  (time_ruler->start_time_on_screen_in_samples + time_ruler->window_size_in_samples) - (clip_start_time - time_ruler->start_time_on_screen_in_samples);
        }
        u_int bucket_size = (last_sample_to_draw - first_sample_to_draw) / screen.width;
        for (int x = 0; x < screen.width; x++) {
            // The sum of the values of all samples in the bucket
            double sum = 0;
            for (int i = bucket_size * x; i < bucket_size * x; i++) {
                sum += this->clip->get_sample(i).value;
            }
            // the average of all samples in the bucket
            double average = sum / bucket_size;
            int y = screen.height - 1;
            // Number of different height block glyphs
            int number_of_glyphs = 8;

            int view_units = map(average, -1, 1, 0, 1) * screen.height * number_of_glyphs;
            while (view_units > number_of_glyphs) {
                view_units -= number_of_glyphs;
                screen.draw(Point(x, y), ScreenCell(int_to_block_glyph(8)));
                y--;
            }
            screen.draw(Point(x, y), ScreenCell(int_to_block_glyph(view_units)));
        }
        ScreenCellStyle style = ScreenCellStyle();
        style.foreground_colour = Colour(0.1, 1.0, 1.0);
        style.background_colour = Colour(0.4, 0.3, 1.0);

        screen.set_style(style);
    }

private:
    char32_t int_to_block_glyph(int index) {
        switch (index) {
        case 0: return U' ';
        case 1: return U'▁';
        case 2: return U'▂';
        case 3: return U'▃';
        case 4: return U'▄';
        case 5: return U'▅';
        case 6: return U'▆';
        case 7: return U'▇';
        case 8: return U'█';
        default: throw invalid_argument("Glyph index is invalid");
        }
    }
    float map(float value, int min, int max, int new_min, int new_max)
    {
        return (new_min + ((new_max-new_min)* (value-min))/(max-min));
    }
};

