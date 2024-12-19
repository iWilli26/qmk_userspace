// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include <keycodes.h>
#include <stdbool.h>
#include "os_detection.h"
#include "quantum.h"

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
#define COLEMAK  DF(_COLEMAK_DH)
#define DVORAK   DF(_DVORAK)

#define SYM      MO(_SYM)
#define NAV      MO(_NAV)
#define FKEYS    MO(_FUNCTION)
#define ADJUST   MO(_ADJUST)

#define CTL_ESC  MT(MOD_LCTL, KC_ESC)
#define CTL_QUOT MT(MOD_RCTL, KC_QUOTE)
#define CTL_MINS MT(MOD_RCTL, KC_MINUS)
#define ALT_ENT  MT(MOD_LALT, KC_ENT)


// bool process_detected_host_os_kb(os_variant_t a) {
//     os_variant_t detected_os = detected_host_os();

//     if (!process_detected_host_os_user(detected_os)) {
//         return false;
//     }
//     switch (detected_os) {
//       case OS_WINDOWS:
//       case OS_LINUX:
//       case OS_UNSURE:
//         #undef REDO
//         #undef UNDO
//         #undef CUT
//         #undef COPY
//         #undef PASTE
//         #undef SLCTALL
//         #undef SAVE
//         #undef PREV_W
//         #undef NEXT_W
//         #undef OS_CTL
//         #undef OS_GUI
//         #define REDO C(KC_Y)
//         #define UNDO C(KC_Z)
//         #define CUT C(KC_X)
//         #define COPY C(KC_C)
//         #define PASTE C(KC_V)
//         #define SLCTALL C(KC_A)
//         #define SAVE C(KC_S)
//         #define PREV_W C(KC_LEFT)
//         #define NEXT_W C(KC_RGHT)
//         #define OS_CTL KC_LCTL
//         #define OS_GUI KC_LGUI
//         break;
//       case OS_MACOS:
//       case OS_IOS:
//         #undef REDO
//         #undef UNDO
//         #undef CUT
//         #undef COPY
//         #undef PASTE
//         #undef SLCTALL
//         #undef SAVE
//         #undef PREV_W
//         #undef NEXT_W
//         #undef OS_CTL
//         #undef OS_GUI
//         #define REDO S(G(KC_Z))
//         #define UNDO G(KC_Z)
//         #define CUT G(KC_X)
//         #define COPY G(KC_C)
//         #define PASTE G(KC_V)
//         #define SLCTALL G(KC_A)
//         #define SAVE G(KC_S)
//         #define PREV_W A(KC_LEFT)
//         #define NEXT_W A(KC_RGHT)
//         #define OS_CTL KC_LGUI
//         #define OS_GUI KC_LCTL
//         break;
//       }
//     return true;
//   }

if (host_get_os() == OS_MACOS) {
        #define COPY G(KC_C)
} else if (host_get_os() == OS_WINDOWS) {
          #define COPY C(KC_C)

    // Windows-specific behavior
} 


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
 *                        |Adjust| LGUI | LAlt/| _SELECT| Nav  |                                       | Sym  | SYM  | AltGr| RGUI | Menu |
 *                        |      |      | Enter| Space  | Tab  |                                       |      | BkSpc|      |      |      |
 *                        `----------------------------------'                                       `----------------------------------'
 */
    [_COLEMAK_DH] = LAYOUT_split_3x6_5_hlc(
    KC_ESCAPE, KC_Q ,  KC_W   ,  KC_F   ,   KC_P ,   KC_B ,                                                                    KC_J  ,   KC_L ,   KC_U ,   KC_Y ,KC_SCLN, KC_BSPC,
     KC_LSFT , KC_A ,  LALT_T(KC_R)   ,  CTL_T(KC_S)   ,   SFT_T(KC_T) ,   KC_G ,                                              KC_M  ,   SFT_T(KC_N) ,   CTL_T(KC_E) ,   LALT_T(KC_I) ,  KC_O , CTL_QUOT,
     KC_LCTL , KC_Z ,  KC_X   ,  KC_C   ,   KC_D ,   KC_V , CW_TOGG, KC_CAPS,                             FKEYS  ,     KC_RBRC, KC_K  ,   KC_H , KC_COMM, KC_DOT ,KC_SLSH, KC_RSFT,
                                 AC_TOGG, KC_LGUI, ALT_ENT, LT(_SELECT, KC_SPACE) , LT(_NAV, KC_TAB)   ,  KC_ENT    , LT(_SYM, KC_BSPC) ,KC_RALT , KC_RGUI, KC_APP,

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
      _______, _______, KC_LEFT, KC_DOWN, KC_RIGHT, _______,                                     KC_PGDN, MS_LEFT, MS_DOWN, MS_RGHT, KC_VOLD, KC_INS,
      _______, _______, _______, _______, _______, _______, _______, KC_SCRL, _______, _______,KC_PAUSE, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_PSCR,
                                 _______, _______, _______, _______, _______, MS_BTN3, MS_BTN1, MS_BTN2, _______, _______,

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
      KC_GRV ,   KC_AT ,   S(KC_3) ,   S(KC_9) ,   S(KC_0) ,   KC_DLR ,                                       KC_6 ,   KC_7 ,   KC_8 ,   KC_9 ,   KC_PAST , KC_PEQL ,
     KC_LSFT , KC_EXLM ,  KC_AT     , KC_QUOT,  KC_GRV    , KC_SCLN,                                     KC_CIRC,    KC_4,    KC_5,    KC_6, KC_SLSH, KC_ENT,
     KC_NUBS , KC_AMPR , KC_PERC    , KC_QUES, KC_EXLM   , KC_BSLS, _______ , _______, _______, _______, KC_0,       KC_1,    KC_2,    KC_3, KC_DOT, KC_QUES,
                                       _______, _______, KC_LBRC, KC_SPC , KC_RBRC, _______, _______, _______, _______, _______,

                                            KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO

    ),

        [_SELECT] = LAYOUT_split_3x6_5_hlc(
     KC_TAB  , COPY ,      S(KC_LEFT)  ,   S(KC_UP)      ,   S(KC_RIGHT)    , SLCTALL   ,                                       KC_Y,   KC_U ,  KC_I ,   KC_O ,  KC_P , KC_BSPC,
     CTL_ESC , PASTE ,  C(S(KC_LEFT))   ,  S(KC_DOWN)     ,   C(S(KC_RIGHT)) , UNDO   ,                                        KC_H,   KC_J ,  KC_K ,   KC_L ,KC_SCLN,CTL_QUOT,
     KC_LSFT , CUT ,  KC_X   ,  KC_C  ,   KC_V ,   KC_B , KC_LBRC,KC_CAPS  , REDO  ,                         KC_RBRC, KC_N,   KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_RSFT,
                                ADJUST , KC_LGUI, ALT_ENT, KC_SPC , NAV     , SYM    ,                               KC_SPC ,KC_RALT, KC_RGUI, KC_APP,

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
      _______,  KC_F9 ,  KC_F10,  KC_F11,  KC_F12, _______,                                     _______, _______, _______, _______, _______, _______,
      _______,  KC_F5 ,  KC_F6 ,  KC_F7 ,  KC_F8 , _______,                                     _______, KC_RSFT, KC_RCTL, KC_LALT, KC_RGUI, _______,
      _______,  KC_F1 ,  KC_F2 ,  KC_F3 ,  KC_F4 , _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,

                                      KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO

    ),




/*
 * Base Layer: Dvorak
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Tab   | ' "  | , <  | . >  |   P  |   Y  |                              |   F  |   G  |   C  |   R  |   L  |  Bksp  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Ctrl/Esc|   A  |   O  |   E  |   U  |   I  |                              |   D  |   H  |   T  |   N  |   S  |Ctrl/- _|
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift | ; :  |   Q  |   J  |   K  |   X  | [ {  |CapsLk|  |F-keys|  ] } |   B  |   M  |   W  |   V  |   Z  | RShift |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| LGUI | LAlt/| Space| Nav  |  | Sym  | Space| AltGr| RGUI | Menu |
 *                        |      |      | Enter|      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_DVORAK] = LAYOUT_split_3x6_5_hlc(
     KC_TAB  ,KC_QUOTE,KC_COMM,  KC_DOT,   KC_P ,   KC_Y ,                                        KC_F,   KC_G ,  KC_C ,   KC_R ,  KC_L , KC_BSPC,
     CTL_ESC , KC_A ,  KC_O   ,  KC_E  ,   KC_U ,   KC_I ,                                        KC_D,   KC_H ,  KC_T ,   KC_N ,  KC_S , CTL_MINS,
     KC_LSFT ,KC_SCLN, KC_Q   ,  KC_J  ,   KC_K ,   KC_X , KC_LBRC,KC_CAPS,     FKEYS  , KC_RBRC, KC_B,   KC_M ,  KC_W ,   KC_V ,  KC_Z , KC_RSFT,
                                 ADJUST, KC_LGUI, ALT_ENT, KC_SPC , NAV   ,     SYM    , KC_SPC ,KC_RALT, KC_RGUI, KC_APP,

                                      KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,     KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO

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