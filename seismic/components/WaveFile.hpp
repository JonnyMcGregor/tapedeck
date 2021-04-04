#include <fstream>
#include <string>

struct WaveFile {
    std::string file_path;
    std::ifstream file;

    WaveFile(std::string file_path) {
        this->file_path = file_path;
    }

    int16_t read_byte_pair(int position) {
        char first_byte, second_byte;

        file.open(file_path, std::ios::binary);
        file.seekg(position, std::ios::beg);
        file.read(&first_byte, 1);
        file.read(&second_byte, 1);
        return bytes_to_int(first_byte, second_byte);
    }

    int16_t bytes_to_int(unsigned char first_byte, unsigned char second_byte) {
        // convert two bytes to one 16-bit int (little endian)
        int16_t output = (second_byte << 8) | first_byte;
        return output;
    }
};
