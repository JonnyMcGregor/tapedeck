#pragma once
#include <stdexcept>

struct Sample {
    float value;

    Sample() {
        this->value = 0;
    }

    Sample(double value) {
        set_value(value);
    }

    void set_value(double new_value) {
        this->value = new_value;
        this->clamp(); // ensure value is between -1 and 1;
    }

    void operator+=(const Sample &other) {
        this->value += other.value;
        this->clamp(); // ensure value is between -1 and 1;
    }

    void clamp()
    {
        if(!is_sample_in_bounds(value))
        {
            if(value <= -1) { value = -0.999; }
            else { value = 0.999; }
        }
    }

private:

    bool is_sample_in_bounds(double sample) {
        if (-1 <= sample && sample <= 1) {
            return true;
        }
        return false;
    }
};