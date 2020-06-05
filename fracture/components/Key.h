#pragma once
#include "TermControl.h"
#include <map>
#include <string>
using namespace std;

enum struct KeyCode {
    K_Null = -1,
    K_Escape = 1,
    K_F1,
    K_F2,
    K_F3,
    K_F4,
    K_F5,
    K_F6,
    K_F7,
    K_F8,
    K_F9,
    K_F10,
    K_F11,
    K_F12,
    K_PauseBreak,
    K_Insert,
    K_Delete,

    K_Tab,
    K_Grave,
    K_1,
    K_2,
    K_3,
    K_4,
    K_5,
    K_6,
    K_7,
    K_8,
    K_9,
    K_0,
    K_Minus = 45,
    K_Equal = 43,
    K_Backspace = 127,

    K_Tilde = K_Grave,
    K_Exclamation = K_1,
    K_At = K_2,
    K_Hash = K_3,
    K_Dollar = K_4,
    K_Percent = K_5,
    K_Caret = K_6,
    K_Ampersand = K_7,
    K_Asterisk = K_8,
    K_ParanthesisOpen = K_9,
    K_ParenthesisClose = K_0,
    K_Underscore = K_Minus,
    K_Plus = K_Equal,

    K_A = 65,
    K_B,
    K_C,
    K_D,
    K_E,
    K_F,
    K_G,
    K_H,
    K_I,
    K_J,
    K_K,
    K_L,
    K_M,
    K_N,
    K_O,
    K_P,
    K_Q,
    K_R,
    K_S,
    K_T,
    K_U,
    K_V,
    K_W,
    K_X,
    K_Y,
    K_Z,

    K_Home,
    K_PageUp,
    K_PageDown,
    K_End,

    K_UpArrow,
    K_DownArrow,
    K_LeftArrow,
    K_RightArrow,

    K_BracketOpen,
    K_BracketClose,
    K_BackSlash,
    K_Semicolon,
    K_Quote,
    K_Comma,
    K_Period,
    K_ForwardSlash,

    K_Enter = 13,
    K_Space = 32,
};

std::map<KeyCode, string> key_to_char{
    {KeyCode::K_A, "a"},
    {KeyCode::K_B, "b"},
    {KeyCode::K_C, "c"},
    {KeyCode::K_D, "d"},
    {KeyCode::K_E, "e"},
    {KeyCode::K_F, "f"},
    {KeyCode::K_G, "g"},
    {KeyCode::K_H, "h"},
    {KeyCode::K_I, "i"},
    {KeyCode::K_J, "j"},
    {KeyCode::K_K, "k"},
    {KeyCode::K_L, "l"},
    {KeyCode::K_M, "m"},
    {KeyCode::K_N, "n"},
    {KeyCode::K_O, "o"},
    {KeyCode::K_P, "p"},
    {KeyCode::K_Q, "q"},
    {KeyCode::K_R, "r"},
    {KeyCode::K_S, "s"},
    {KeyCode::K_T, "t"},
    {KeyCode::K_U, "u"},
    {KeyCode::K_V, "v"},
    {KeyCode::K_W, "w"},
    {KeyCode::K_X, "x"},
    {KeyCode::K_Y, "y"},
    {KeyCode::K_Z, "z"},
};

string getKeyName(KeyCode keycode) {
    return key_to_char.find(keycode)->second;
}

enum ModifierKey {
    Control = 1,
    Shift = 2,
    Alt = 4,
};

struct KeyCombo {
    KeyCode keycode;
    int modifier_keys = 0;

    KeyCombo(KeyCode keycode, int modifier_keys = 0) {
        this->keycode = keycode;
        this->modifier_keys = modifier_keys;
    }

    string getText() {
        string output = "";
        if (modifier_keys & ModifierKey::Control) output += "Control+";
        if (modifier_keys & ModifierKey::Shift) output += "Shift+";
        if (modifier_keys & ModifierKey::Alt) output += "Alt+";
        output += getKeyName(keycode);
        return output;
    }
};

KeyCode num_to_keycode[26] = {
    KeyCode::K_A,
    KeyCode::K_B,
    KeyCode::K_C,
    KeyCode::K_D,
    KeyCode::K_E,
    KeyCode::K_F,
    KeyCode::K_G,
    KeyCode::K_H,
    KeyCode::K_I,
    KeyCode::K_J,
    KeyCode::K_K,
    KeyCode::K_L,
    KeyCode::K_M,
    KeyCode::K_N,
    KeyCode::K_O,
    KeyCode::K_P,
    KeyCode::K_Q,
    KeyCode::K_R,
    KeyCode::K_S,
    KeyCode::K_T,
    KeyCode::K_U,
    KeyCode::K_V,
    KeyCode::K_W,
    KeyCode::K_X,
    KeyCode::K_Y,
    KeyCode::K_Z,
};

// Calls getch, returns KeyCombo. If nothing waiting in stdin, returns null KeyCombo.
KeyCombo
getKeyCombo() {
    int keycode = TermControl::getch();
    if (keycode == 27) {
        keycode = TermControl::getch();
    }

    if (1 <= keycode && keycode <= 26) {
        return KeyCombo(num_to_keycode[keycode - 1], ModifierKey::Control);
    }
    if (65 <= keycode && keycode <= 90) {
        return KeyCombo(num_to_keycode[keycode - 65], ModifierKey::Shift);
    }
    if (97 <= keycode && keycode <= 122) {
        return KeyCombo(num_to_keycode[keycode - 97]);
    }

    switch (keycode) {
    };
    return KeyCombo(KeyCode::K_Null);
}
