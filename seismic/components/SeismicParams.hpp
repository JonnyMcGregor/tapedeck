struct SeismicParams {
    SeismicParams(unsigned int sample_rate, unsigned int buffer_size, unsigned int num_input_channels, unsigned int num_output_channels) {
        this->sample_rate = sample_rate;
        this->buffer_size = buffer_size;
        this->num_input_channels = num_input_channels;
        this->num_output_channels = num_output_channels;
    }
    unsigned int sample_rate, buffer_size, num_input_channels, num_output_channels;
};