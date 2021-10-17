#pragma once
#include <iostream>
#include <string>

void send_control_code(std::string code) {
    std::cout << "\e[" << code;
}
