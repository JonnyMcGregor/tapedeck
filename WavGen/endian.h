#include <iostream>

namespace little_endian_io {
    template <typename Word>
    std::ostream &write_word(std::ostream &outs, Word value, unsigned size = sizeof(Word)) {
        for (; size; --size, value >>= 8)
            outs.put(static_cast<char>(value & 0xFF));
        return outs;
    }

    template <typename Word>
    std::istream &read_word(std::istream &ins, Word &value, unsigned size = sizeof(Word)) {
        for (unsigned n = 0, value = 0; n < size; ++n)
            value |= ins.get() << (8 * n);
        return ins;
    }
} // namespace little_endian_io

namespace big_endian_io {
    template <typename Word>
    std::ostream &write_word(std::ostream &outs, Word value, unsigned size = sizeof(Word)) {
        while (size)
            outs.put(static_cast<char>((value >> (8 * --size)) & 0xFF));
        return outs;
    }

    template <typename Word>
    std::istream &read_word(std::istream &ins, Word &value, unsigned size = sizeof(Word)) {
        for (value = 0; size; --size)
            value = (value << 8) | ins.get();
        return ins;
    }
} // namespace big_endian_io