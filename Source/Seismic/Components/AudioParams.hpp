struct AudioParams {
    AudioParams(unsigned int sampleRate, unsigned int bufferSize, unsigned int numInputChannels, unsigned int numOutputChannels) {
        this->sampleRate = sampleRate;
        this->bufferSize = bufferSize;
        this->numInputChannels = numInputChannels;
        this->numOutputChannels = numOutputChannels;
    }
    unsigned int sampleRate, bufferSize, numInputChannels, numOutputChannels;
};