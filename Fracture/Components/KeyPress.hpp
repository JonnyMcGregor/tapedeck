#pragma once
#include "Key.hpp"
#include <string>

enum ModifierKey {
    Control = 1 << 0,
    Shift = 1 << 1,
    Alt = 1 << 2,
};

struct KeyPress {
    // A dumb data structure, with basic display methods
    Key key;
    int modifierKeys = 0;

    KeyPress(Key key) {
        this->key = key;
    }

    KeyPress(Key key, int modifierKeys) {
        this->key = key;
        this->modifierKeys = modifierKeys;
    }

    bool operator==(KeyPress other) const {
        if (this->key != other.key) {
            return false;
        }
        if (this->modifierKeys != other.modifierKeys) {
            return false;
        }
        return true;
    }

    bool operator<(KeyPress other) const {
        // Allows this struct to be used as a map key
        // The +1 prevents modifierKeys from being zero, which would cause a bug
        // where non-modified key ranks always evaluate to 0.
        int thisValue = static_cast<int>(this->key) * (this->modifierKeys + 1);
        int otherValue = static_cast<int>(other.key) * (other.modifierKeys + 1);
        return thisValue < otherValue;
    }
};

/** Debuggery Helpers */
#include "KeyMaps.hpp"
std::string to_string(KeyPress kp) {
    if (keypressToString.count(kp) > 0) {
        return keypressToString.at(kp);
    } else {
        return "";
    }
}

std::string to_debug_string(KeyPress kp) {
    std::string output = "";
    if (kp.modifierKeys & ModifierKey::Control) output += "Control+";
    if (kp.modifierKeys & ModifierKey::Shift) output += "Shift+";
    if (kp.modifierKeys & ModifierKey::Alt) output += "Alt+";
    if (keyToKeyname.count(kp.key) > 0) {
        output += keyToKeyname.at(kp.key);
    } else {
        output += "<unknown>";
    }
    return output;
}