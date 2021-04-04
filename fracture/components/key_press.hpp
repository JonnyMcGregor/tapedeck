#pragma once
#include "key.hpp"
#include <string>

enum ModifierKey {
    Control = 1 << 0,
    Shift = 1 << 1,
    Alt = 1 << 2,
};

struct KeyPress {
    // A dumb data structure, with basic display methods
    Key key;
    int modifier_keys = 0;

    KeyPress(Key key) {
        this->key = key;
    }

    KeyPress(Key key, int modifier_keys) {
        this->key = key;
        this->modifier_keys = modifier_keys;
    }

    bool operator==(KeyPress other) const {
        if (this->key != other.key) {
            return false;
        }
        if (this->modifier_keys != other.modifier_keys) {
            return false;
        }
        return true;
    }

    bool operator<(KeyPress other) const {
        // Allows this struct to be used as a map key
        // The +1 prevents modifier_keys from being zero, which would cause a bug
        // where non-modified key ranks always evaluate to 0.
        int this_value = static_cast<int>(this->key) * (this->modifier_keys + 1);
        int other_value = static_cast<int>(other.key) * (other.modifier_keys + 1);
        return this_value < other_value;
    }
};

#include "key_maps.hpp"

std::string to_string(KeyPress kp) {
    if (keypress_to_string.count(kp) > 0) {
        return keypress_to_string.at(kp);
    } else {
        return "";
    }
}

std::string to_debug_string(KeyPress kp) {
    std::string output = "";
    if (kp.modifier_keys & ModifierKey::Control) output += "Control+";
    if (kp.modifier_keys & ModifierKey::Shift) output += "Shift+";
    if (kp.modifier_keys & ModifierKey::Alt) output += "Alt+";
    if (key_to_keyname.count(kp.key) > 0) {
        output += key_to_keyname.at(kp.key);
    } else {
        output += "<unknown>";
    }
    return output;
}