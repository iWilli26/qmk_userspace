// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include <keycodes.h>
#include <stdbool.h>
#include <unistd.h> // For sleep function

#include "quantum.h"
#include "action_tapping.h"
#include "host.h"
#include "print.h"
#include "process_unicode.h"

enum layers {
    _COLEMAK_DH = 0,
    _SYM,
    _NAV,
    _SELECT,
    _FUNCTION,
    _ADJUST,
    _DVORAK,
};

// Aliases for readability
#define COLEMAK DF(_COLEMAK_DH)
#define DVORAK DF(_DVORAK)

#define SYM MO(_SYM)
#define NAV MO(_NAV)
#define FKEYS MO(_FUNCTION)
#define ADJUST MO(_ADJUST)

#define CTL_ESC MT(MOD_LCTL, KC_ESC)
#define CTL_QUOT MT(MOD_RCTL, KC_QUOTE)
#define CTL_MINS MT(MOD_RCTL, KC_MINUS)
#define ALT_ENT MT(MOD_LALT, KC_ENT)

typedef struct {
    bool swap_ctl_gui;
#ifdef UNICODE_COMMON_ENABLE
    uint8_t unicode_input_mode;
#endif // UNICODE_COMMON_ENABLE
} os_detection_config_t;

bool process_detected_host_os_user(os_variant_t detected_os) {
    if (is_keyboard_master()) {
        os_detection_config_t os_detection_config = {
            .swap_ctl_gui = false,
#ifdef UNICODE_COMMON_ENABLE
            .unicode_input_mode = UNICODE_MODE_WINCOMPOSE,
#endif // UNICODE_COMMON_ENABLE
        };
        switch (detected_os) {
            case OS_UNSURE:
                xprintf("unknown OS Detected\n");
                break;
            case OS_LINUX:
                xprintf("Linux Detected\n");
#ifdef UNICODE_COMMON_ENABLE
                os_detection_config.unicode_input_mode = UNICODE_MODE_LINUX;
#endif // UNICODE_COMMON_ENABLE
                break;
            case OS_WINDOWS:
                xprintf("Windows Detected\n");
#define REDO C(S(KC_Z))
#define UNDO C(KC_Z)
#define CUT C(KC_X)
#define COPY C(KC_C)
#define PASTE C(KC_V)
#define SLCTALL C(KC_A)
#define SAVE C(KC_S)
#define PREV_W C(KC_LEFT)
#define NEXT_W C(KC_RGHT)
#define END_LINE KC_END
#define START_LINE KC_HOME
                break;
            case OS_MACOS:
                xprintf("MacOS Detected\n");
#undef REDO
#undef UNDO
#undef CUT
#undef COPY
#undef PASTE
#undef SLCTALL
#undef SAVE
#undef PREV_W
#undef NEXT_W
#undef END_LINE
#undef START_LINE

#define REDO KC_LGUI(S(KC_Y))
#define UNDO KC_LGUI(KC_Z)
#define CUT KC_LGUI(KC_X)
#define COPY KC_LGUI(KC_C)
#define PASTE KC_LGUI(KC_V)
#define SLCTALL KC_LGUI(KC_A)
#define SAVE KC_LGUI(KC_S)
#define PREV_W C(KC_LEFT)
#define NEXT_W C(KC_RGHT)
#define END_LINE KC_LGUI(KC_RGHT)
#define START_LINE KC_LGUI(KC_LEFT)
                // os_detection_config = (os_detection_config_t){
                //     .swap_ctl_gui = true,

                break;
            default:
                xprintf("Unknown OS Detected\n");
                break;
        }
        keymap_config.swap_lctl_lgui = keymap_config.swap_rctl_rgui = os_detection_config.swap_ctl_gui;
#ifdef UNICODE_COMMON_ENABLE
        set_unicode_input_mode_soft(os_detection_config.unicode_input_mode);
#endif // UNICODE_COMMON_ENABLE
    }

    return true;
}

enum custom_keycodes { DOT_DASH = SAFE_RANGE };

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint16_t my_hash_timer;
    switch (keycode) {
        case DOT_DASH:
            if (record->event.pressed) {
                my_hash_timer = timer_read();
            } else {
                if (timer_elapsed(my_hash_timer) < 200) {
                    tap_code16(KC_COMMA);
                } else {
                    tap_code16(KC_MINS);
                }
            }
            break;
    }
    return true;
}

enum combos {
    E_AIG,
    E_GRV,
    E_CIR,
};

const uint16_t PROGMEM es_combo[] = {CTL_T(KC_E), CTL_T(KC_S), COMBO_END};
const uint16_t PROGMEM et_combo[] = {CTL_T(KC_E), SFT_T(KC_T), COMBO_END};
const uint16_t PROGMEM er_combo[] = {CTL_T(KC_E), LALT_T(KC_R), COMBO_END};

combo_t key_combos[] = {
    [E_AIG] = COMBO_ACTION(es_combo),
    [E_GRV] = COMBO_ACTION(et_combo),
    [E_CIR] = COMBO_ACTION(er_combo),
};

void process_combo_event(uint16_t combo_index, bool pressed) {
    switch (combo_index) {
        case E_AIG:
            if (pressed) {
                const uint8_t mods = get_mods();
                del_mods(MOD_MASK_SHIFT);
                tap_code16(KC_QUOT);
                set_mods(mods);
                tap_code16(KC_E);
            }
            break;
        case E_GRV:
            if (pressed) {
                const uint8_t mods = get_mods();
                del_mods(MOD_MASK_SHIFT);
                tap_code16(KC_GRV);
                set_mods(mods);
                tap_code16(KC_E);
            }
            break;
        case E_CIR:
            if (pressed) {
                const uint8_t mods = get_mods();
                del_mods(MOD_MASK_SHIFT);
                tap_code16(KC_CIRC);
                set_mods(mods);
                tap_code16(KC_E);
            }
            break;
    }
}

enum {
    TD_MAC_WIN,
};

tap_dance_action_t tap_dance_actions[] = {
    [TD_MAC_WIN] = ACTION_TAP_DANCE_DOUBLE(CG_LNRM, CG_LSWP),
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/*
 * Base Layer: Colemak DH
 *
 * ,-------------------------------------------.                                                                    ,-------------------------------------------.
 * |  Esc   |   Q  |   W  |   F  |   P  |   B  |                                                                    |   J  |   L  |   U  |   Y  | ;  : |  Bksp  |
 * |--------+------+------+------+------+------|                                                                    |------+------+------+------+------+--------|
 * |LShift  |   A  |   R  |   S  |   T  |   G  |                                                                    |   M  |   N  |   E  |   I  |   O  |Ctrl/' "|
 * |--------+------+------+------+------+------+-------------.                                        ,-------------+------+------+------+------+------+--------|
 * |  Ctl   |   Z  |   X  |   C  |   D  |   V  | KC_MINS|CapsLk|                                       |F-keys|  ] } |   K  |   H  | ,  < | . >  | /  ? | RShift |
 * `----------------------+------+------+------+------+------|                                       |------+------+------+------+------+----------------------'
 *                        |Switch| LGUI | LAlt/| _SELECT| Nav  |                                       | Sym  | SYM  | AltGr| RGUI | Menu |
 *                        |GUI CTL|      | Enter| Space  | Tab  |                                       |      | BkSpc|      |      |      |
 *                        `----------------------------------'                                       `----------------------------------
 */
    [_COLEMAK_DH] = LAYOUT_split_3x6_5_hlc(
    KC_ESCAPE, KC_Q ,  KC_W   ,  KC_F   ,   KC_P ,   KC_B ,                                                                                             KC_J  ,   KC_L ,   KC_U ,   KC_Y ,KC_SCLN, KC_BSPC,
     KC_LSFT , KC_A ,  LALT_T(KC_R)   ,  CTL_T(KC_S)   ,   SFT_T(KC_T) ,   KC_G ,                                                 KC_M  ,   SFT_T(KC_N) ,   CTL_T(KC_E) ,   LALT_T(KC_I) ,  KC_O , KC_RSFT,
     KC_LCTL , KC_Z ,  KC_X   ,  KC_C   ,   KC_D ,   KC_V , CW_TOGG, KC_CAPS,                                                     FKEYS  ,     KC_RBRC, KC_K  ,   KC_H , DOT_DASH, KC_DOT ,KC_SLSH, CTL_QUOT,
                          TD(TD_MAC_WIN) , LT(_FUNCTION, KC_ESCAPE), LT(_SELECT, KC_SPACE) , LT(_NAV, KC_TAB),  KC_LGUI           ,KC_RALT , KC_ENT    , LT(_SYM, KC_BSPC), KC_RGUI, KC_APP,

         KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ), 


/*
 * Nav Layer: Media, navigation
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |      |      |      |                              | PgUp | Home |   ↑  | End  | VolUp| Delete |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  GUI |  Alt | Ctrl | Shift|      |                              | PgDn |  ←   |   ↓  |   →  | VolDn| Insert |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |ScLck |  |      |      | Pause|M Prev|M Play|M Next|VolMut| PrtSc  |
 * ----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                            ----------------------------------  ----------------------------------
 */
    [_NAV] = LAYOUT_split_3x6_5_hlc(
      _______, _______, PREV_W, KC_UP, NEXT_W , _______,                                     KC_PGUP, MS_WHLU, MS_UP,   MS_WHLD,  KC_VOLU, KC_DEL,
      _______, START_LINE, KC_LEFT, KC_DOWN, KC_RIGHT, END_LINE,                                     KC_PGDN, MS_LEFT, MS_DOWN, MS_RGHT, KC_VOLD, KC_INS,
      _______, _______, _______, _______, _______, _______, _______, KC_SCRL, _______, _______,KC_PAUSE, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_PSCR,
                                 _______, _______, _______, _______, _______,_______, MS_BTN3, MS_BTN1, MS_BTN2, _______,

        KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO


    ),

/*
 * Sym Layer: Numbers and symbols
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |    `   |  @   |  #   |  (   |  )   |  $   |                              |   +  |  7   |  8   |  9   |  *   |   =    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |  LShift|  !   |  ^   |  '"   |  `   |  ; : |                              |   -  |  4   |  5   |  6   |  /   | Enter  |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |    |   |  &  |  %   |  ?   |  !    |  \|   |     |      |  |      |   }  |   0  |  1   |  2   |  3   |  .   |   ?    |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |    { | Space|   }  |  |      |      |      |      |      |
 *                        |      |      |   [  |      |   ]  |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_SYM] = LAYOUT_split_3x6_5_hlc(
      KC_GRV ,   KC_AT ,   S(KC_3) ,   S(KC_9) ,   S(KC_0) ,   KC_DLR ,                                       KC_PPLS ,   KC_7 ,   KC_8 ,   KC_9 ,   KC_PAST , KC_PEQL ,
     KC_LSFT , KC_EXLM ,  KC_CIRC     , KC_QUOT,  KC_GRV    , KC_SCLN,                                     KC_CIRC,    KC_4,    KC_5,    KC_6, KC_SLSH, KC_ENT,
     KC_NUBS , KC_AMPR , KC_PERC    , KC_QUES, KC_EXLM   , KC_BSLS, _______ , _______, _______, _______, KC_0,       KC_1,    KC_2,    KC_3, KC_DOT, KC_QUES,
                                       _______, KC_LBRC, KC_SPC , KC_RBRC,_______, _______, _______, _______, _______, _______,

                                            KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO

    ),

        [_SELECT] = LAYOUT_split_3x6_5_hlc(
     KC_TAB  , COPY ,      S(KC_LEFT)  ,   S(KC_UP)      ,   S(KC_RIGHT)    , SLCTALL   ,                                                 KC_Y,   KC_U ,  KC_I ,   KC_O ,  KC_P , KC_BSPC,
     CTL_ESC , PASTE ,  C(S(KC_LEFT))   ,  S(KC_DOWN)     ,   C(S(KC_RIGHT)) , UNDO   ,                                                   KC_H,   KC_J ,  KC_K ,   KC_L ,KC_SCLN,CTL_QUOT,
     KC_LSFT , CUT ,    KC_LEFT ,  KC_C  ,   KC_RIGHT ,      KC_B ,       KC_LBRC,        KC_CAPS  , REDO  ,           KC_RBRC, KC_N,   KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_RSFT,
                                ADJUST , KC_LGUI, ALT_ENT, KC_SPC , NAV     , SYM    ,                                    KC_SPC ,KC_RALT, KC_RGUI, KC_APP,

                                     KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO

    ), 

/*
 * Function Layer: Function keys
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |  F9  | F10  | F11  | F12  |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  F5  |  F6  |  F7  |  F8  |      |                              |      | Shift| Ctrl |  Alt |  GUI |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |  F1  |  F2  |  F3  |  F4  |      |      |      |  |      |      |      |      |      |      |      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_FUNCTION] = LAYOUT_split_3x6_5_hlc(
      BL_TOGG,  KC_F9 ,  KC_F10,  KC_F11,  KC_F12, UG_NEXT,                                     _______, _______, _______, _______, _______, _______,
      BL_STEP,  KC_F5 ,  KC_F6 ,  KC_F7 ,  KC_F8 , UG_PREV,                                     _______, KC_RSFT, KC_RCTL, KC_LALT, KC_RGUI, _______,
      _______,  KC_F1 ,  KC_F2 ,  KC_F3 ,  KC_F4 , _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,

                                      KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO

    ),




/*
 * Base Layer: QWERTY
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Tab   |   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |  Bksp  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Ctrl/Esc|   A  |   S  |   D  |   F  |   G  |                              |   H  |   J  |   K  |   L  | ;  : |Ctrl/' |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  | [ {  |CapsLk|  |F-keys|  ] } |   N  |   M  | ,  < | . >  | /  ? | RShift |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| LGUI | LAlt/| Space| Nav  |  | Sym  | Space| AltGr| RGUI | Menu |
 *                        |      |      | Enter|      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * | MUTE | ____ | _____ | ____ | ____ |                                              | MUTE | ____ | _____ | ____ | ____ |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [_QWERTY] = LAYOUT_split_3x6_5_hlc(
     KC_TAB  , KC_Q ,  KC_W   ,  KC_E  ,   KC_R ,   KC_T ,                                        KC_Y,   KC_U ,  KC_I ,   KC_O ,  KC_P , KC_BSPC,
     KC_TAB , KC_A ,  KC_S   ,  KC_D  ,   KC_F ,   KC_G ,                                        KC_H,   KC_J ,  KC_K ,   KC_L ,KC_SCLN,CTL_QUOT,
     KC_LSFT , KC_Z ,  KC_X   ,  KC_C  ,   KC_V ,   KC_B , KC_LBRC,KC_CAPS,     FKEYS  , KC_RBRC, KC_N,   KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_RSFT,
                                ADJUST , KC_LGUI, ALT_ENT, KC_SPC , NAV   ,     SYM    , KC_SPC ,KC_RALT, KC_RGUI, KC_APP,
     KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),





/*
 * Adjust Layer: Default layer settings, RGB
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |QWERTY|      |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      |      |Dvorak|      |      |                              | TOG  | SAI  | HUI  | VAI  | MOD  |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |Colmak|      |      |      |      |  |      |      |      | SAD  | HUD  | VAD  | RMOD |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_ADJUST] = LAYOUT_split_3x6_5_hlc(
      _______, _______, _______, _SELECT , _______, _______,                                    _______, _______, _______, _______, _______, _______,
      _______, _______, _______, DVORAK , _______, _______,                                    RM_TOGG, RM_SATU, RM_HUEU, RM_VALU, RM_NEXT, _______,
      _______, _______, _______, COLEMAK, _______, _______,_______, _______, _______, _______, _______, RM_SATD, RM_HUED, RM_VALD, RM_PREV, _______,
                                 _______, _______, _______,_______, _______, _______, _______, _______, _______, _______,

        KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO

    ),

// /*
//  * Layer template
//  *
//  * ,-------------------------------------------.                              ,-------------------------------------------.
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        `----------------------------------'  `----------------------------------'
//  */
//     [_LAYERINDEX] = LAYOUT_split_3x6_5_hlc(
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
//     ),
//
};

