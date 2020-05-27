#include <fstream>
#include <math.h>
#include "endian.h"

using namespace std;
using namespace little_endian_io;

struct WaveFileGenerator
{
    WaveFileGenerator() {}

    void initialise(int sample_rate, int bit_depth, int num_channels)
    {
        current_sample_rate = sample_rate;
        bits_per_sample = bit_depth;
        num_of_channels = num_channels;
        byte_rate = (current_sample_rate * bits_per_sample * num_of_channels) / 8;
        block_align = (bits_per_sample * num_of_channels) / 8;
        max_amplitude = pow(2, bits_per_sample) * 0.5 - 1;
    }

    void openWaveFile(ofstream &wav_file)
    {
        // Write the file headers
        wav_file << "RIFF----WAVEfmt ";               // (chunk size to be filled in later)
        write_word(wav_file, 16, 4);                  // no extension data
        write_word(wav_file, 1, 2);                   // PCM - integer samples
        write_word(wav_file, num_of_channels, 2);     // two channels (stereo file)
        write_word(wav_file, current_sample_rate, 4); // samples per second (Hz)
        write_word(wav_file, byte_rate, 4);           // (Sample Rate * BitsPerSample * Channels) / 8
        write_word(wav_file, block_align, 2);         // data block size (size of two integer samples, one for each channel, in bytes)
        write_word(wav_file, bits_per_sample, 2);     // number of bits per sample (use a multiple of 8)

        // Write the data chunk header
        data_chunk_pos = wav_file.tellp();
        wav_file << "data----"; // (chunk size to be filled in later)
    }

    void writeInputToFile(ofstream &wav_file, double buffer_data)
    {
        write_word(wav_file, (int)(max_amplitude * buffer_data), 2);
    }

    void closeWaveFile(ofstream &wav_file)
    {
        // (We'll need the final file size to fix the chunk sizes above)
        size_t file_length = wav_file.tellp();

        // Fix the data chunk header to contain the data size
        wav_file.seekp(data_chunk_pos + 4);
        write_word(wav_file, file_length - data_chunk_pos + 8);

        // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
        wav_file.seekp(0 + 4);
        write_word(wav_file, file_length - 8, 4);
    }

private:
    int current_sample_rate = 0, bits_per_sample = 0, num_of_channels = 0,
        byte_rate = 0, block_align = 0, max_amplitude = 0;

    size_t data_chunk_pos = 0;
};
