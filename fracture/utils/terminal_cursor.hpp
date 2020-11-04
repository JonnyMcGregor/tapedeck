#pragma once
#include "terminal.hpp"
#include "terminal_info.hpp"
#include <string>

namespace terminal {
    namespace cursor {
        // See https://en.wikipedia.org/wiki/ANSI_escape_code#Terminal_output_sequences
        void carriage_return() {
            send_control_code("0E");
        }
        void line_feed() {
            send_control_code("1S");
        }
        void move_to_top_left() {
            send_control_code("H");
        }
        void hide() {
            send_control_code("?25l");
        }
        void show() {
            send_control_code("?25h");
        }

    } // namespace cursor
} // namespace terminal