#include "endian.hpp"
#include <fstream>
#include <math.h>

using namespace std;
using namespace little_endian_io;

struct WaveFileGenerator {
    WaveFileGenerator() {}

    void initialise(int sampleRate, int bitDepth, int numChannels) {
        currentSampleRate = sampleRate;
        bitsPerSample = bitDepth;
        numOfChannels = numChannels;
        byteRate = (currentSampleRate * bitsPerSample * numOfChannels) / 8;
        blockAlign = (bitsPerSample * numOfChannels) / 8;
        maxAmplitude = pow(2, bitsPerSample) * 0.5 - 1;
    }

    void openWaveFile(ofstream &wavFile) {
        // Write the file headers
        wavFile << "RIFF----WAVEfmt ";               // (chunk size to be filled in later)
        write_word(wavFile, 16, 4);                  // no extension data
        write_word(wavFile, 1, 2);                   // PCM - integer samples
        write_word(wavFile, numOfChannels, 2);     // two channels (stereo file)
        write_word(wavFile, currentSampleRate, 4); // samples per second (Hz)
        write_word(wavFile, byteRate, 4);           // (Sample Rate * BitsPerSample * Channels) / 8
        write_word(wavFile, blockAlign, 2);         // data block size (size of two integer samples, one for each channel, in bytes)
        write_word(wavFile, bitsPerSample, 2);     // number of bits per sample (use a multiple of 8)

        // Write the data chunk header
        dataChunkPos = wavFile.tellp();
        wavFile << "data----"; // (chunk size to be filled in later)
    }

    void writeInputToFile(ofstream &wavFile, double buffer_data) {
        write_word(wavFile, (int)(maxAmplitude * buffer_data), 2);
    }

    void closeWaveFile(ofstream &wavFile) {
        // (We'll need the final file size to fix the chunk sizes above)
        size_t file_length = wavFile.tellp();

        // Fix the data chunk header to contain the data size
        wavFile.seekp(dataChunkPos + 4);
        write_word(wavFile, file_length - dataChunkPos + 8);

        // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
        wavFile.seekp(0 + 4);
        write_word(wavFile, file_length - 8, 4);
    }

    int getMaxAmplitude() {
        return maxAmplitude;
    }

private:
    int currentSampleRate = 0, bitsPerSample = 0, numOfChannels = 0,
        byteRate = 0, blockAlign = 0, maxAmplitude = 0;

    size_t dataChunkPos = 0;
};
