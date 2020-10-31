struct Playhead {
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
};
