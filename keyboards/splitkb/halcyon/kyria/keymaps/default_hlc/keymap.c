// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "quantum.h"
#include "action_tapping.h"
#include "host.h"
#include "print.h"
#include "process_unicode.h"
#include "keycodes.h"
#define MASTER_LEFT

// Features expected enabled in rules.mk:
// ENCODER_MAP_ENABLE = yes
// CAPS_WORD_ENABLE = yes
// OS_DETECTION_ENABLE = yes
// CONSOLE_ENABLE = yes
// TAP_DANCE_ENABLE = yes
// COMBO_ENABLE = yes

#define OS_DETECTION_DEBOUNCE 500
#define OS_DETECTION_KEYBOARD_RESET

enum layers { _COLEMAK_DH = 0, _SYM, _NAV, _SELECT, _FUNCTION, _LOL, _GAME, _WAKFU };

#define COLEMAK DF(_COLEMAK_DH)

#define SYM MO(_SYM)
#define NAV MO(_NAV)
#define FKEYS MO(_FUNCTION)
#define ADJUST MO(_LOL)

#define CTL_ESC MT(MOD_LCTL, KC_ESC)
#define CTL_QUOT MT(MOD_RCTL, KC_QUOTE)
#define CTL_MINS MT(MOD_RCTL, KC_MINUS)
#define ALT_ENT MT(MOD_LALT, KC_ENT)

enum custom_keycodes { TEST = SAFE_RANGE, STAB_NEXT, STAB_PREV, REDO, CTL_CLICK, UNDO, CUT, COPY, PASTE, SELECT_ALL, GUI_SAVE, PREV_W, NEXT_W, END_LINE, START_LINE, DOT_DASH, SLC_NEXT_WORD, SLC_PREV_WORD, SLC_END_LINE, SLC_START_LINE, GOTOLINE, GO_BACK, GO_FORWARD, GO_NEXT_ERROR, GO_PREV_ERROR, SCREENSHOT };

typedef struct {
    bool swap_ctl_gui;
#ifdef UNICODE_COMMON_ENABLE
    uint8_t unicode_input_mode;
#endif
} os_detection_config_t;

// Use uint16_t for keycodes to be compatible with QMK keycode types
static uint16_t click_modifier     = KC_LCTL; // For ctrl-click functionality
static uint16_t undo_key           = C(KC_Z);
static uint16_t redo_key           = C(KC_Y);
static uint16_t cut_key            = C(KC_X);
static uint16_t copy_key           = C(KC_C);
static uint16_t paste_key          = C(KC_V);
static uint16_t select_all_key     = C(KC_A);
static uint16_t save_key           = C(KC_S);
static uint16_t prev_word_key      = C(KC_LEFT);
static uint16_t next_word_key      = C(KC_RGHT);
static uint16_t slc_next_word_key  = C(S(KC_RGHT));
static uint16_t slc_prev_word_key  = C(S(KC_LEFT));
static uint16_t slc_start_line_key = S(KC_HOME);
static uint16_t slc_end_line_key   = S(KC_END);
static uint16_t goto_line_key      = C(KC_G);
static uint16_t go_back            = LALT(KC_LEFT);
static uint16_t go_forward         = LALT(KC_RGHT);
static uint16_t go_next_error      = LALT(KC_F8);
static uint16_t go_prev_error      = LALT(S(KC_F8));
static uint16_t screenshot_key     = RGUI(S(KC_S));

static uint16_t end_line_key   = KC_END;
static uint16_t start_line_key = KC_HOME;

// Sticky tab variables
static uint16_t tab_modifier = KC_LALT; // Alt for Windows/Linux, Cmd for macOS

// NOTE: The fall-through behavior in this handler is intentional per user comment.
bool process_detected_host_os_user(os_variant_t detected_os) {
    if (is_keyboard_master()) {
        os_detection_config_t os_detection_config = {
            .swap_ctl_gui = false,
#ifdef UNICODE_COMMON_ENABLE
            .unicode_input_mode = UNICODE_MODE_WINCOMPOSE,
#endif
        };

        switch (detected_os) {
            case OS_UNSURE:
                xprintf("Unknown OS Detected\n");
                // intentional fallthrough
            case OS_WINDOWS:
                xprintf("Windows Detected\n");
                click_modifier     = KC_LCTL; // Use Ctrl for Windows
                undo_key           = LCTL(KC_Z);
                redo_key           = LCTL(KC_Y);
                cut_key            = LCTL(KC_X);
                copy_key           = LCTL(KC_C);
                paste_key          = LCTL(KC_V);
                select_all_key     = LCTL(KC_A);
                save_key           = LCTL(KC_S);
                prev_word_key      = LCTL(KC_LEFT);
                next_word_key      = LCTL(KC_RGHT);
                slc_next_word_key  = LCTL(S(KC_RGHT));
                slc_prev_word_key  = LCTL(S(KC_LEFT));
                slc_end_line_key   = S(KC_END);
                slc_start_line_key = S(KC_HOME);
                end_line_key       = KC_END;
                start_line_key     = KC_HOME;
                goto_line_key      = LCTL(KC_G);
                go_back            = LALT(KC_MINS);
                go_forward         = LALT(S(KC_MINS));
                go_next_error      = LALT(KC_F8);
                go_prev_error      = LALT(S(KC_F8));
                tab_modifier       = KC_LALT; // Use Alt for Windows
                break;
            case OS_LINUX:
                xprintf("Linux Detected\n");
                click_modifier     = KC_LCTL; // Use Ctrl for Linux
                undo_key           = LCTL(KC_Z);
                redo_key           = LCTL(KC_Y);
                cut_key            = LCTL(KC_X);
                copy_key           = LCTL(KC_C);
                paste_key          = LCTL(KC_V);
                select_all_key     = LCTL(KC_A);
                save_key           = LCTL(KC_S);
                prev_word_key      = LCTL(KC_LEFT);
                next_word_key      = LCTL(KC_RGHT);
                slc_next_word_key  = LCTL(S(KC_RGHT));
                slc_prev_word_key  = LCTL(S(KC_LEFT));
                slc_end_line_key   = LCTL(S(KC_END));
                slc_start_line_key = LCTL(S(KC_HOME));
                goto_line_key      = LCTL(KC_G);
                end_line_key       = KC_END;
                start_line_key     = KC_HOME;
                go_back            = LCTL(LALT(KC_MINUS));
                go_forward         = LCTL(S(KC_MINUS));
                tab_modifier       = KC_LALT; // Use Alt for Linux
                break;
            case OS_MACOS:
                xprintf("MacOS Detected\n");
                click_modifier                   = KC_LGUI; // Use Cmd for macOS
                undo_key                         = LGUI(KC_Z);
                redo_key                         = LSG(KC_Z);
                cut_key                          = LGUI(KC_X);
                copy_key                         = LGUI(KC_C);
                paste_key                        = LGUI(KC_V);
                select_all_key                   = LGUI(KC_A);
                save_key                         = LGUI(KC_S);
                prev_word_key                    = LALT(KC_LEFT);
                next_word_key                    = LALT(KC_RGHT);
                slc_next_word_key                = LALT(S(KC_RGHT));
                slc_prev_word_key                = LALT(S(KC_LEFT));
                slc_end_line_key                 = LGUI(S(KC_RGHT));
                slc_start_line_key               = LGUI(S(KC_LEFT));
                end_line_key                     = LGUI(KC_RGHT);
                start_line_key                   = LGUI(KC_LEFT);
                goto_line_key                    = C(KC_G);
                go_back                          = C(KC_MINS);
                go_forward                       = C(S(KC_MINS));
                screenshot_key                   = LGUI(S(KC_5));
                tab_modifier                     = KC_LGUI; // Use Cmd for macOS
                os_detection_config.swap_ctl_gui = true;
                break;

            default:
                xprintf("Unknown OS Detected\n");
                click_modifier                   = KC_LGUI; // Default to Cmd for unknown OS
                undo_key                         = LGUI(KC_Z);
                redo_key                         = LSG(KC_Z);
                cut_key                          = LGUI(KC_X);
                copy_key                         = LGUI(KC_C);
                paste_key                        = LGUI(KC_V);
                select_all_key                   = LGUI(KC_A);
                save_key                         = LGUI(KC_S);
                prev_word_key                    = LALT(KC_LEFT);
                next_word_key                    = LALT(KC_RGHT);
                slc_next_word_key                = LALT(S(KC_RGHT));
                slc_prev_word_key                = LALT(S(KC_LEFT));
                slc_end_line_key                 = LGUI(S(KC_RGHT));
                slc_start_line_key               = LGUI(S(KC_LEFT));
                end_line_key                     = LGUI(KC_RGHT);
                start_line_key                   = LGUI(KC_LEFT);
                goto_line_key                    = LGUI(KC_G);
                tab_modifier                     = KC_LGUI; // Default to Cmd for unknown OS
                os_detection_config.swap_ctl_gui = true;
                break;
        }

        // Apply swap config to keymap_config so QMK's dynamic swap works
        keymap_config.swap_lctl_lgui = keymap_config.swap_rctl_rgui = os_detection_config.swap_ctl_gui;

        // Unicode input selection — prefer UNICODEMAP/UCIS where available; fall back to common mode if compiled that way
#ifdef UNICODEMAP_ENABLE
        // If using the unicode map subsystem, we leave the map as-is. Some users may want to switch modes explicitly.
        xprintf("Using UNICODEMAP input subsystem\n");
#elif defined(UNICODE_ENABLE)
        // Basic Unicode: nothing extra needed here for mode selection
        xprintf("Using Basic UNICODE input subsystem\n");
#elif defined(UNICODE_COMMON_ENABLE)
        set_unicode_input_mode_soft(os_detection_config.unicode_input_mode);
        xprintf("Using UNICODE_COMMON with mode %u\n", os_detection_config.unicode_input_mode);
#endif
    }

    return true;
}
static bool     is_sticky_tab_active = false;
static uint16_t sticky_tab_timer     = 0;

#define STICKY_TAB_TIMEOUT 1000 // 1 second timeout

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint16_t my_hash_timer;
    switch (keycode) {
        case STAB_NEXT:
            if (record->event.pressed) {
                if (!is_sticky_tab_active) {
                    register_code(tab_modifier);
                    is_sticky_tab_active = true;
                }
                tap_code(KC_TAB);
                sticky_tab_timer = timer_read();
            }
            return false;

        case LSFT_T(KC_T):
            if (record->event.pressed) {
                sticky_tab_timer = timer_read();
                if (is_sticky_tab_active) {
                    tap_code16(S(KC_TAB));
                    sticky_tab_timer = timer_read();
                    return false;
                }
                return true;
            } else {
                return true;
            }
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
            return false;
        case REDO:
            if (record->event.pressed) {
                tap_code16(redo_key);
            }
            return false;
        case UNDO:
            if (record->event.pressed) {
                tap_code16(undo_key);
            }
            return false;
        case CUT:
            if (record->event.pressed) {
                tap_code16(cut_key);
            }
            return false;
        case COPY:
            if (record->event.pressed) {
                tap_code16(copy_key);
            }
            return false;
        case PASTE:
            if (record->event.pressed) {
                tap_code16(paste_key);
            }
            return false;
        case SELECT_ALL:
            if (record->event.pressed) {
                tap_code16(select_all_key);
            }
            return false;
        case GUI_SAVE:
            if (record->event.pressed) {
                tap_code16(save_key);
            }
            return false;
        case PREV_W:
            if (record->event.pressed) {
                tap_code16(prev_word_key);
            }
            return false;
        case NEXT_W:
            if (record->event.pressed) {
                tap_code16(next_word_key);
            }
            return false;
        case SLC_NEXT_WORD:
            if (record->event.pressed) {
                tap_code16(slc_next_word_key);
            }
            return false;
        case SLC_PREV_WORD:
            if (record->event.pressed) {
                tap_code16(slc_prev_word_key);
            }
            return false;
        case SLC_END_LINE:
            if (record->event.pressed) {
                tap_code16(slc_end_line_key);
            }
            return false;
        case SLC_START_LINE:
            if (record->event.pressed) {
                tap_code16(slc_start_line_key);
            }
            return false;
        case END_LINE:
            if (record->event.pressed) {
                tap_code16(end_line_key);
            }
            return false;
        case START_LINE:
            if (record->event.pressed) {
                tap_code16(start_line_key);
            }
            return false;
        case GOTOLINE:
            if (record->event.pressed) {
                tap_code16(goto_line_key);
            }
            return false;
        case GO_BACK:
            if (record->event.pressed) {
                tap_code16(go_back);
            }
            return false;
        case GO_FORWARD:
            if (record->event.pressed) {
                tap_code16(go_forward);
            }
            return false;
        case GO_NEXT_ERROR:
            if (record->event.pressed) {
                tap_code16(go_next_error);
            }
            return false;
        case GO_PREV_ERROR:
            if (record->event.pressed) {
                tap_code16(go_prev_error);
            }
            return false;
        case SCREENSHOT:
            if (record->event.pressed) {
                tap_code16(screenshot_key);
            }
            return false;
        case CTL_CLICK: {
            if (record->event.pressed) {
                register_code(click_modifier);   // Press and hold Ctrl/Cmd based on OS
                wait_ms(10);                     // Small delay to ensure modifier is registered
                tap_code16(MS_BTN1);             // Tap Left Click
                wait_ms(10);                     // Small delay before releasing
                unregister_code(click_modifier); // Release Ctrl/Cmd
            }
            break;
        }
        default:
            // Any other key pressed → stop sticky tab
            if (is_sticky_tab_active && record->event.pressed) {
                unregister_code(tab_modifier);
                is_sticky_tab_active = false;
                return false;
            }
            return true;
    }
    return true;
}

void matrix_scan_user(void) {
    // Check for sticky tab timeout
    if (is_sticky_tab_active && timer_elapsed(sticky_tab_timer) > STICKY_TAB_TIMEOUT) {
        unregister_code(tab_modifier);
        is_sticky_tab_active = false;
    }
}

// ---- Combos ----
// Use plain base keycodes in combo definitions where possible to avoid fragile behavior caused by
// mods/wrapper keycodes like CTL_T(...) inside the combo definition.
enum combos {
    E_AIG,
    E_GRV,
    E_CIR,
    A_GRV,
    U_GRV,
    O_GRV,
};

const uint16_t PROGMEM es_combo[] = {CTL_T(KC_E), CTL_T(KC_S), COMBO_END};
const uint16_t PROGMEM et_combo[] = {CTL_T(KC_E), LSFT_T(KC_T), COMBO_END};
const uint16_t PROGMEM er_combo[] = {CTL_T(KC_E), LALT_T(KC_R), COMBO_END};
const uint16_t PROGMEM ae_combo[] = {KC_A, CTL_T(KC_E), COMBO_END};
const uint16_t PROGMEM ug_combo[] = {KC_U, SFT_T(KC_T), COMBO_END};
const uint16_t PROGMEM og_combo[] = {KC_O, SFT_T(KC_T), COMBO_END};

combo_t key_combos[] = {
    [E_AIG] = COMBO_ACTION(es_combo), [E_GRV] = COMBO_ACTION(et_combo), [E_CIR] = COMBO_ACTION(er_combo), [A_GRV] = COMBO_ACTION(ae_combo), [U_GRV] = COMBO_ACTION(ug_combo), [O_GRV] = COMBO_ACTION(og_combo),
};

void process_combo_event(uint16_t combo_index, bool pressed) {
    if (!pressed) return;
    const uint8_t mods = get_mods();
    switch (combo_index) {
        case A_GRV:
            del_mods(MOD_MASK_SHIFT);
            tap_code16(KC_GRV);
            set_mods(mods);
            tap_code16(KC_A);
            break;
        case E_AIG:
            del_mods(MOD_MASK_SHIFT);
            tap_code16(KC_QUOT);
            set_mods(mods);
            tap_code16(KC_E);
            break;
        case E_GRV:
            del_mods(MOD_MASK_SHIFT);
            tap_code16(KC_GRV);
            set_mods(mods);
            tap_code16(KC_E);
            break;
        case O_GRV:
            del_mods(MOD_MASK_SHIFT);
            tap_code16(KC_GRV);
            set_mods(mods);
            tap_code16(KC_O);
            break;
        case U_GRV:
            del_mods(MOD_MASK_SHIFT);
            tap_code16(KC_GRV);
            set_mods(mods);
            tap_code16(KC_U);
            break;
        case E_CIR:
            del_mods(MOD_MASK_SHIFT);
            tap_code16(KC_CIRC);
            set_mods(mods);
            tap_code16(KC_E);
            break;
    }
}

// ---- Tap dance ----
enum {
    TD_1,
    TD_3,
};

// Keep the tap_dance_actions array visible in keymap.c (some builds require it to be introspectable)
tap_dance_action_t tap_dance_actions[] = {
    // Single tap: send 1, Double tap: send 2 (game layout behavior)
    [TD_1] = ACTION_TAP_DANCE_DOUBLE(KC_1, KC_2),
    // Single tap: send 3, Double tap: send 4 (game layout behavior)
    [TD_3] = ACTION_TAP_DANCE_DOUBLE(KC_3, KC_4),
};

// ---- Keymaps ----
// (kept unchanged except for minor formatting & ensuring custom keycodes are used consistently)

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
    KC_ESCAPE, KC_Q ,  KC_W   ,  KC_F   ,   KC_P ,   KC_B ,                                                                                             KC_J  ,   KC_L ,   KC_U ,   KC_Y ,KC_MINS, KC_BSPC,
     KC_LSFT , KC_A ,  LALT_T(KC_R)   ,  CTL_T(KC_S)   ,   LSFT_T(KC_T) ,   KC_G ,                                                 KC_M  ,   SFT_T(KC_N) ,   CTL_T(KC_E) ,   LALT_T(KC_I) ,  KC_O , KC_MINS,
     KC_LCTL , KC_Z ,  KC_X   ,  KC_C   ,   KC_D ,   KC_V , CW_TOGG, KC_CAPS,                                                     FKEYS  ,     KC_RBRC, KC_K  ,   KC_H , DOT_DASH, KC_DOT ,KC_SLSH, CTL_QUOT,
                          TO(_GAME) , LT(_FUNCTION, KC_ESCAPE), LT(_SELECT, KC_SPACE) , LT(_NAV, KC_TAB),  STAB_NEXT           ,KC_RALT , LT(_SELECT, KC_ENT)    , LT(_SYM, KC_BSPC), KC_RGUI, TO(_LOL),

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
      _______, _______, PREV_W, KC_UP, NEXT_W , GOTOLINE,                                            GO_FORWARD, MS_WHLU, MS_UP,   MS_WHLD,  GO_PREV_ERROR, KC_VOLU,
      _______, START_LINE, KC_LEFT, KC_DOWN, KC_RIGHT, END_LINE,                                     GO_BACK, MS_LEFT, MS_DOWN, MS_RGHT, GO_NEXT_ERROR, KC_VOLD,
      _______, _______, S(KC_F4), KC_F4, KC_F12, _______, _______, KC_SCRL, _______, _______,KC_PAUSE, KC_MPRV, KC_MPLY, KC_MNXT, SCREENSHOT, KC_MUTE,
                                 _______, _______, _______, _______, _______,MS_BTN3, CTL_CLICK, MS_BTN1, MS_BTN2, _______,

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
      KC_GRV ,   KC_AT ,   S(KC_3) ,   S(KC_9) ,   S(KC_0) ,   KC_DLR ,                                       KC_PPLS ,   KC_7 ,   KC_8 ,   KC_9 ,   KC_PAST , KC_EQL ,
     KC_LSFT , KC_EXLM ,  KC_CIRC     , KC_QUOT,  KC_GRV    , KC_SCLN,                                     KC_CIRC,    KC_4,    KC_5,    KC_6, KC_SLSH, KC_ENT,
     KC_NUBS , KC_AMPR , KC_PERC    , KC_QUES, KC_EXLM   , KC_BSLS, _______ , _______, _______, _______, KC_0,       KC_1,    KC_2,    KC_3, KC_DOT, KC_QUES,
                                       _______, KC_LBRC, KC_SPC , KC_RBRC,_______, _______, _______, _______, _______, _______,

                                            KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO

    ),

        [_SELECT] = LAYOUT_split_3x6_5_hlc(
     KC_TAB  , COPY ,      S(KC_LEFT)  ,   S(KC_UP)      ,   S(KC_RIGHT)    , SELECT_ALL   ,                                              _______,  PREV_W, KC_UP, NEXT_W , _______,_______, 
     KC_LSFT , PASTE ,  SLC_PREV_WORD   ,  S(KC_DOWN)     ,   SLC_NEXT_WORD , UNDO   ,                                                  START_LINE, KC_LEFT, KC_DOWN, KC_RIGHT, END_LINE,KC_H,
     CTL_ESC , CUT ,    SLC_START_LINE ,  KC_C  ,           SLC_END_LINE ,      REDO ,       REDO,        KC_CAPS  , REDO  ,           KC_RBRC, KC_N,   KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_RSFT,
                                ADJUST , KC_LGUI, ALT_ENT, KC_SPC , NAV     ,                                                            SYM    ,KC_ENT ,KC_BSPC, KC_RGUI, KC_APP,

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
    [_LOL] = LAYOUT_split_3x6_5_hlc(
     KC_ESCAPE  ,KC_ESCAPE, KC_1 ,  KC_2   ,  KC_3  ,   KC_4  ,                                        KC_Y,   KC_U ,  KC_I ,   KC_O ,  KC_P , KC_BSPC,
     KC_LSFT , KC_TAB, KC_Q ,  KC_W   ,  KC_E  ,   KC_R  ,                                        KC_H,   KC_J ,  KC_K ,   KC_L ,KC_SCLN,CTL_QUOT,
     KC_LCTL , KC_LSFT,KC_G ,  KC_B   ,  KC_D  ,   KC_F  , KC_LBRC,KC_CAPS,     FKEYS  , KC_RBRC, KC_N,   KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_RSFT,
                                TO(_COLEMAK_DH), LT(_FUNCTION, KC_ESCAPE), LT(_SELECT, KC_SPACE) ,  KC_SPACE,  STAB_NEXT           ,KC_RALT , LT(_NAV, KC_ENT)    , LT(_SYM, KC_BSPC), KC_RGUI, TO(_COLEMAK_DH),

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
    [_GAME] = LAYOUT_split_3x6_5_hlc(
     KC_ESCAPE  , KC_LALT   ,  KC_Q   ,  KC_W  ,   KC_E ,   KC_R ,                                          KC_U ,  KC_I ,   KC_O ,  KC_P , KC_BSPC, KC_T , 
     KC_LSFT , KC_TAB ,  KC_A   ,  KC_S  ,   KC_D ,   KC_F ,                                         KC_J ,  KC_K ,   KC_L ,KC_SCLN,CTL_QUOT, KC_T , 
     KC_LCTL , KC_LSFT ,  KC_Z   ,  KC_X  ,   KC_C ,   KC_V , KC_LBRC,KC_CAPS,     FKEYS  , KC_RBRC,   KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_RSFT, KC_T , 
                                TO(_WAKFU), TD(TD_1) , TD(TD_3) , KC_SPACE,  STAB_NEXT           ,KC_RALT , LT(_NAV, KC_ENT)    , LT(_SYM, KC_BSPC), KC_RGUI, KC_APP,

     KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO

    ),

    [_WAKFU] = LAYOUT_split_3x6_5_hlc(
     KC_D  , KC_E   ,  KC_F   ,  KC_G  ,   KC_H ,   KC_J ,                                          KC_U ,  KC_I ,   KC_O ,  KC_P , KC_BSPC, KC_T , 
     KC_A , KC_B ,  KC_P   ,  KC_S  ,   KC_I ,   KC_M ,                                         KC_J ,  KC_K ,   KC_L ,KC_SCLN,CTL_QUOT, KC_T , 
     KC_5 , KC_6 ,  KC_1   ,  KC_2  ,   KC_3 ,   KC_4 , KC_LBRC,KC_CAPS,     FKEYS  , KC_RBRC,   KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_RSFT, KC_T , 
                                TO(_COLEMAK_DH), KC_L , KC_SPACE , KC_LSFT,  STAB_NEXT           ,KC_RALT , LT(_NAV, KC_ENT)    , LT(_SYM, KC_BSPC), KC_RGUI, KC_APP,

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

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU),  ENCODER_CCW_CW(KC_VOLD, KC_VOLU),  ENCODER_CCW_CW(KC_PGUP, KC_PGDN),  ENCODER_CCW_CW(KC_PGUP, KC_PGDN)  },
    [1] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
    [2] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
    [3] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
    [4] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
    [5] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
    [6] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
    [7] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
};
#endif

// EOF
