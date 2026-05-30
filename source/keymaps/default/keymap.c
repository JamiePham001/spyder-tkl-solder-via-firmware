// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "usb_util.h"

#ifdef VIA_ENABLE
#    include "via.h"
#endif

// ---------------------------------------------------------------------------
// Indicator colors + per-indicator brightness stored in EEPROM.
// Uses the user datablock API (EECONFIG_USER_DATA_SIZE = 6).
// ---------------------------------------------------------------------------
typedef struct {
    uint8_t caps_h; // CapsLock hue         (0-255, maps to 0-360°)
    uint8_t caps_s; // CapsLock saturation  (0-255)
    uint8_t caps_v; // CapsLock brightness  (0-255)
    uint8_t scrl_h; // ScrollLock hue
    uint8_t scrl_s; // ScrollLock saturation
    uint8_t scrl_v; // ScrollLock brightness
} user_config_t;

user_config_t user_config;

void eeconfig_init_user_datablock(void) {
    user_config.caps_h = 0;   // white (any hue, sat = 0)
    user_config.caps_s = 0;
    user_config.caps_v = 255;
    user_config.scrl_h = 21;  // orange (~30° mapped to 0-255 range)
    user_config.scrl_s = 255;
    user_config.scrl_v = 255;
    eeconfig_update_user_datablock(&user_config, 0, sizeof(user_config));
}

void keyboard_post_init_user(void) {
    if (eeconfig_is_user_datablock_valid()) {
        eeconfig_read_user_datablock(&user_config, 0, sizeof(user_config));
    } else {
        eeconfig_init_user_datablock();
    }
}

// ---------------------------------------------------------------------------
// VIA custom menu — channel/value IDs must match via_definition.json
// ---------------------------------------------------------------------------
#define INDICATOR_CHANNEL  1
#define ID_CAPS_COLOR      1
#define ID_SCRL_COLOR      2
#define ID_CAPS_BRIGHTNESS 3
#define ID_SCRL_BRIGHTNESS 4

#ifdef VIA_ENABLE
void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    uint8_t  command  = data[0];
    uint8_t  channel  = data[1];
    uint8_t  value_id = data[2];
    uint8_t *value    = &data[3];

    if (channel != INDICATOR_CHANNEL) {
        return;
    }

    switch (command) {
        case id_custom_set_value:
            switch (value_id) {
                case ID_CAPS_COLOR:
                    user_config.caps_h = value[0];
                    user_config.caps_s = value[1];
                    break;
                case ID_SCRL_COLOR:
                    user_config.scrl_h = value[0];
                    user_config.scrl_s = value[1];
                    break;
                case ID_CAPS_BRIGHTNESS:
                    user_config.caps_v = value[0];
                    break;
                case ID_SCRL_BRIGHTNESS:
                    user_config.scrl_v = value[0];
                    break;
            }
            break;

        case id_custom_get_value:
            switch (value_id) {
                case ID_CAPS_COLOR:
                    value[0] = user_config.caps_h;
                    value[1] = user_config.caps_s;
                    break;
                case ID_SCRL_COLOR:
                    value[0] = user_config.scrl_h;
                    value[1] = user_config.scrl_s;
                    break;
                case ID_CAPS_BRIGHTNESS:
                    value[0] = user_config.caps_v;
                    break;
                case ID_SCRL_BRIGHTNESS:
                    value[0] = user_config.scrl_v;
                    break;
            }
            break;

        case id_custom_save:
            eeconfig_update_user_datablock(&user_config, 0, sizeof(user_config));
            break;
    }
}
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * ┌───┐   ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┐
     * │Esc│   │F1 │F2 │F3 │F4 │ │F5 │F6 │F7 │F8 │ │F9 │F10│F11│F12│ │PSc│Scr│Pse│
     * └───┘   └───┴───┴───┴───┘ └───┴───┴───┴───┘ └───┴───┴───┴───┘ └───┴───┴───┘
     * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───────┐ ┌───┬───┬───┐
     * │ ` │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │ Backsp│ │Ins│Hom│PgU│
     * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤ ├───┼───┼───┤
     * │ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ [ │ ] │  \  │ │Del│End│PgD│
     * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┤ └───┴───┴───┘
     * │ Caps │ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ ' │  Enter │
     * ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────────┤     ┌───┐
     * │ Shift  │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │    Shift │     │ ↑ │
     * ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬────┬────┤ ┌───┼───┼───┐
     * │Ctrl│GUI │Alt │                        │ Alt│ GUI│Menu│Ctrl│ │ ← │ ↓ │ → │
     * └────┴────┴────┴────────────────────────┴────┴────┴────┴────┘ └───┴───┴───┘
     */
    [0] = LAYOUT_tkl_ansi(
        KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,     KC_PSCR, KC_SCRL, KC_PAUS,

        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,    KC_INS,  KC_HOME, KC_PGUP,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,    KC_DEL,  KC_END,  KC_PGDN,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,             KC_UP,
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, KC_RGUI, KC_APP,  KC_RCTL,    KC_LEFT, KC_DOWN, KC_RGHT
    )
};

#ifdef RGB_MATRIX_ENABLE
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    led_t led_state = host_keyboard_led_state();

    if (led_state.caps_lock) {
        HSV hsv = {user_config.caps_h, user_config.caps_s, user_config.caps_v};
        RGB rgb = hsv_to_rgb(hsv);
        RGB_MATRIX_INDICATOR_SET_COLOR(0, rgb.r, rgb.g, rgb.b);
    } else {
        RGB_MATRIX_INDICATOR_SET_COLOR(0, 0, 0, 0); // off: excluded from effect
    }

    if (led_state.scroll_lock) {
        HSV hsv = {user_config.scrl_h, user_config.scrl_s, user_config.scrl_v};
        RGB rgb = hsv_to_rgb(hsv);
        RGB_MATRIX_INDICATOR_SET_COLOR(1, rgb.r, rgb.g, rgb.b);
    } else {
        RGB_MATRIX_INDICATOR_SET_COLOR(1, 0, 0, 0); // off: excluded from effect
    }

    return true;
}

// Detect USB disconnect (PC full shutdown with 5V standby keeping the board
// powered). RGB_MATRIX_SLEEP handles the suspend path; this handles the
// disconnect path so both sleep AND hard shutdown turn the LEDs off.
void housekeeping_task_user(void) {
    static bool was_connected = true;
    bool        connected     = usb_connected_state();
    if (was_connected && !connected) {
        rgb_matrix_set_suspend_state(true);
    } else if (!was_connected && connected) {
        rgb_matrix_set_suspend_state(false);
    }
    was_connected = connected;
}
#endif