#pragma once

struct Playhead {
    
    Playhead(int sample_rate)
    {
        this->sample_rate = sample_rate;
    }

    int current_time_in_samples = 0,
        tempo = 120,
        time_sig_num = 4,
        time_sig_denom = 4;

    void movePlayhead(int time_in_samples) {
        if ((current_time_in_samples + time_in_samples) < 0) {
            current_time_in_samples = 0;
        } else {
            current_time_in_samples += time_in_samples;
        }
    }
    void movePlayhead(float time_in_seconds) {
        if ((current_time_in_samples + seconds_to_samples(time_in_seconds)) < 0) {
            current_time_in_samples = 0;
        } else {
            current_time_in_samples += seconds_to_samples(time_in_seconds);
        }
    }

    int seconds_to_samples(float time_in_seconds)
    {
        return time_in_seconds * sample_rate;
    }
    private:
    int sample_rate = 0;
};
