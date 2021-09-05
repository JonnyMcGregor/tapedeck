#pragma once
#include "../../Seismic/Components/Clip.hpp"
#include "../Components/Point.hpp"
#include "../Components/Screen.hpp"
#include "../Components/Widget.hpp"
#include <stdexcept>

struct ClipWidget : Widget {
    std::shared_ptr<Clip> clip;
    std::shared_ptr<TimeRuler> timeRuler; 
    u_int* clipStartTime;
    ClipWidget(std::shared_ptr<Clip> clip, std::shared_ptr<TimeRuler> timeRuler, u_int &clipStartTime) {
        this->clip = clip;
        this->timeRuler = timeRuler;
        this->clipStartTime = &clipStartTime;
        selectedWidgetStyle.foregroundColour = Colour(0.1, 1.0, 1.0);
        selectedWidgetStyle.backgroundColour = Colour(0.4, 0.3, 1.0);

    }

    void process(std::vector<KeyPress> &keyboardInput) {}

    void render(Screen &screen) {
        int firstSampleToDraw = max(0, (int)timeRuler->startTimeOnScreenInSamples - (int)*clipStartTime);
        int lastSampleToDraw = min((int)(clip->size() - 1), (timeRuler->startTimeOnScreenInSamples + timeRuler->windowSizeInSamples - (int)*clipStartTime));
        
        u_int bucketSize = (lastSampleToDraw - firstSampleToDraw) / screen.width;
        for (int x = 0; x < screen.width; x++) {
            // The sum of the values of all samples in the bucket
            double sum = 0;
            for (int i = bucketSize * x; i < bucketSize * (x + 1); i++) {
                sum += this->clip->getSample(i + firstSampleToDraw).value;
            }
            // the average of all samples in the bucket
            double average = sum / bucketSize;
            int y = screen.height - 1;
            // Number of different height block glyphs
            int numberOfGlyphs = 8;

            int viewUnits = map(average, -1, 1, 0, 1) * screen.height * numberOfGlyphs;
            while (viewUnits > numberOfGlyphs) {
                viewUnits -= numberOfGlyphs;
                screen.draw(Point(x, y), ScreenCell(intToBlockGlyph(8)));
                y--;
            }
            screen.draw(Point(x, y), ScreenCell(intToBlockGlyph(viewUnits)));
        }
        ScreenCellStyle style = ScreenCellStyle();
        style.foregroundColour = Colour(1.0, 1.0, 1.0);
        style.backgroundColour = Colour(0.3, 0.3, 0.3);
        if(is_selected)
        {
            screen.set_style(selectedWidgetStyle);
        }
        else{
            screen.set_style(style);
        }
    }

private:
    char32_t intToBlockGlyph(int index) {
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
    float map(float value, int min, int max, int newMin, int newMax)
    {
        return (newMin + ((newMax-newMin)* (value-min))/(max-min));
    }
};

