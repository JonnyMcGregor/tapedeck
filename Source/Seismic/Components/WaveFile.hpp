#include <fstream>
#include <string>

struct WaveFile {
    std::string file_path;
    std::ifstream file;

    WaveFile(std::string file_path) {
        this->file_path = file_path;
    }

    int16_t readBytePair(int position) {
        char firstByte, secondByte;

        file.open(file_path, std::ios::binary);
        file.seekg(position, std::ios::beg);
        file.read(&firstByte, 1);
        file.read(&secondByte, 1);
        return bytesToInt(firstByte, secondByte);
    }

    int16_t bytesToInt(unsigned char firstByte, unsigned char secondByte) {
        // convert two bytes to one 16-bit int (little endian)
        int16_t output = (secondByte << 8) | firstByte;
        return output;
    }
};
