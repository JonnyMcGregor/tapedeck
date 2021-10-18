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
        if(is_sample_in_bounds(new_value))
        {
            this->value = new_value;
        }
    }

    void operator+=(const Sample &other) {
        this->value += other.value;
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
    void assert_is_sample_in_bounds(double sample) {
        if (!is_sample_in_bounds(sample)) {
            throw std::invalid_argument("Sample must be between -1 and 1 (inclusive)");
        }
    }
    bool is_sample_in_bounds(double sample) {
        if (-1 <= sample && sample <= 1) {
            return true;
        }
        return false;
    }
};