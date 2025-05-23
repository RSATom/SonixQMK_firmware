#include QMK_KEYBOARD_H

enum {
    DEFAULT_LAYER,
} Layers;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [DEFAULT_LAYER] = LAYOUT_72us(
        /*L1*/KC_ESC,          KC_1,     KC_2,    KC_3,         KC_4,            KC_5,             KC_6,         KC_GRAVE/*M1*/,     XXXXXXX,             XXXXXXX,            XXXXXXX, // C3
        /*R_*/XXXXXXX,         XXXXXXX,  XXXXXXX, XXXXXXX,      XXXXXXX,         XXXXXXX,          XXXXXXX,      XXXXXXX,            KC_GRAVE/*M6*/,      XXXXXXX,            XXXXXXX,

        /*L2*/KC_TAB,          KC_Q,     KC_W,    KC_E,         KC_R,            KC_T,             KC_T,         XXXXXXX/*M3*/,      XXXXXXX,             XXXXXXX,            KC_LWIN, // C5
        /*R1*/KC_7,            KC_8,     KC_9,    KC_0,         KC_MINUS,        KC_EQUAL,         KC_BACKSPACE, XXXXXXX,            XXXXXXX/*M7*/,       XXXXXXX/*Fn*/,      XXXXXXX,

        /*L3*/KC_CAPS_LOCK,    KC_A,     KC_S,    KC_D,         KC_F,            KC_G,             KC_G,         KC_LEFT_CTRL/*M4*/, XXXXXXX,             XXXXXXX,            XXXXXXX, // C7
        /*R_*/XXXXXXX,         XXXXXXX,  XXXXXXX, XXXXXXX,      XXXXXXX,         XXXXXXX,          KC_Y,         XXXXXXX,            XXXXXXX/*M8*/,       XXXXXXX,            XXXXXXX,

        /*L4*/KC_LEFT_SHIFT,   KC_Z,     KC_X,    KC_C,         KC_V,            KC_B,             KC_B,         XXXXXXX/*M5*/,      XXXXXXX,             XXXXXXX,            XXXXXXX, // C9
        /*R2*/KC_U,            KC_I,     KC_O,    KC_P,         KC_LEFT_BRACKET, KC_RIGHT_BRACKET, KC_BACKSLASH, XXXXXXX,            XXXXXXX,             XXXXXXX,            XXXXXXX,

        /*L5*/XXXXXXX,         XXXXXXX,  XXXXXXX, XXXXXXX,      XXXXXXX,         XXXXXXX,          XXXXXXX,      XXXXXXX/*M2*/,      XXXXXXX,             XXXXXXX,            XXXXXXX, // C4
        /*R_*/XXXXXXX,         XXXXXXX,  XXXXXXX, XXXXXXX,      XXXXXXX,         XXXXXXX,          KC_H,         XXXXXXX,            KC_RIGHT_CTRL/*M9*/, KC_SPACE,           XXXXXXX,

        /*L6*/XXXXXXX,         XXXXXXX,  XXXXXXX, XXXXXXX,      XXXXXXX,         XXXXXXX,          XXXXXXX,      XXXXXXX,            XXXXXXX,             XXXXXXX,            KC_LALT, // C6
        /*R3*/KC_J,            KC_K,     KC_L,    KC_SEMICOLON, KC_QUOTE,        KC_ENTER,         KC_6,         XXXXXXX,            KC_RIGHT_CTRL,       XXXXXXX,            XXXXXXX,

        /*L7*/XXXXXXX,         XXXXXXX,  XXXXXXX, XXXXXXX,      XXXXXXX,         XXXXXXX,          XXXXXXX,      XXXXXXX,            XXXXXXX,             KC_SPACE,           XXXXXXX, // C8
        /*R_*/XXXXXXX,         XXXXXXX,  XXXXXXX, XXXXXXX,      XXXXXXX,         XXXXXXX,          KC_N,         XXXXXXX,            XXXXXXX/*M10*/,      KC_RIGHT_ALT,       XXXXXXX,

        /*L8*/XXXXXXX,         XXXXXXX,  XXXXXXX, XXXXXXX,      XXXXXXX,         XXXXXXX,          XXXXXXX,      XXXXXXX,            KC_LCTL,             XXXXXXX,            XXXXXXX, // C10
        /*R4*/KC_M,            KC_COMMA, KC_DOT,  KC_SLASH,     KC_RIGHT_SHIFT,  XXXXXXX,          XXXXXXX,      XXXXXXX,            XXXXXXX,             KC_RWIN,            XXXXXXX
    )
#if 0
    [EMPTY_LAYER] = LAYOUT_72us(
        /*L1*/_______, _______, _______, _______, _______, _______, _______, _______/*M1*/, _______,        _______,       _______,
        /*R_*/_______, _______, _______, _______, _______, _______, _______, _______,       _______/*M6*/,  _______,       _______,

        /*L2*/_______, _______, _______, _______, _______, _______, _______, _______/*M3*/, _______,        _______,       _______,
        /*R1*/_______, _______, _______, _______, _______, _______, _______, _______,       _______/*M7*/,  _______/*Fn*/, _______,

        /*L3*/_______, _______, _______, _______, _______, _______, _______, _______/*M4*/, _______,        _______,       _______,
        /*R_*/_______, _______, _______, _______, _______, _______, _______, _______,       _______/*M8*/,  _______,       _______,

        /*L4*/_______, _______, _______, _______, _______, _______, _______, _______/*M5*/, _______,        _______,       _______,
        /*R2*/_______, _______, _______, _______, _______, _______, _______, _______,       _______,        _______,       _______,

        /*L5*/_______, _______, _______, _______, _______, _______, _______, _______/*M2*/, _______,        _______,       _______,
        /*R_*/_______, _______, _______, _______, _______, _______, _______, _______,       _______/*M9*/,  _______,       _______,

        /*L6*/_______, _______, _______, _______, _______, _______, _______, _______,       _______,        _______,       _______,
        /*R3*/_______, _______, _______, _______, _______, _______, _______, _______,       _______,        _______,       _______,

        /*L7*/_______, _______, _______, _______, _______, _______, _______, _______,       _______,        _______,       _______,
        /*R_*/_______, _______, _______, _______, _______, _______, _______, _______,       _______/*M10*/, _______,       _______,

        /*L8*/_______, _______, _______, _______, _______, _______, _______, _______,       _______,        _______,       _______,
        /*R4*/_______, _______, _______, _______, _______, _______, _______, _______,       _______,        _______,       _______
    )
#endif
};
