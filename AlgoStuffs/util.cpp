#include "util.h"

std::string readfile(std::string file, bool toHex) {
    int length;
    char* array{};
    std::string s;

    std::ifstream stream;
    stream.open(file, std::ios_base::binary);
    if (stream.good()) {
        length = stream.rdbuf()->pubseekoff(0, std::ios_base::end);
        array = new char[length];
        stream.rdbuf()->pubseekoff(0, std::ios_base::beg);
        stream.read(array, length);
        stream.close();
    }

    if (toHex) {
        char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
        for (int i = 0; i < length; ++i)
        {
            char const byte = array[i];

            s += hex_chars[(byte & 0xF0) >> 4];
            s += hex_chars[(byte & 0x0F) >> 0];
        }
        return s;
    }
    return array;
}