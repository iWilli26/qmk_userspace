// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "quantum.h"
#include "action_tapping.h"
#include "host.h"
#include "print.h"
#include "process_unicode.h"
#include "keycodes.h"
#ifdef HLC_TFT_DISPLAY
#    include "halcyon.h"
#    include "hlc_tft_display/hlc_tft_display.h"
#endif

enum layers { _COLEMAK_DH = 0, _SYM, _NAV, _SELECT, _FUNCTION, _LOL, _GAME };

#define COLEMAK DF(_COLEMAK_DH)

#define SYM MO(_SYM)
#define NAV MO(_NAV)
#define FKEYS MO(_FUNCTION)
#define ADJUST MO(_LOL)

#define CTL_ESC MT(MOD_LCTL, KC_ESC)
#define CTL_QUOT MT(MOD_RCTL, KC_QUOTE)
#define CTL_MINS MT(MOD_RCTL, KC_MINUS)
#define ALT_ENT MT(MOD_LALT, KC_ENT)

enum custom_keycodes { TEST = SAFE_RANGE, STAB_NEXT, STAB_PREV, REDO, CTL_CLICK, UNDO, CUT, COPY, PASTE, SELECT_ALL, GUI_SAVE, PREV_W, NEXT_W, END_LINE, START_LINE, DOT_DASH, SLC_NEXT_WORD, SLC_PREV_WORD, SLC_END_LINE, SLC_START_LINE, GOTOLINE, GO_BACK, GO_FORWARD, GO_NEXT_ERROR, GO_PREV_ERROR, SCREENSHOT, FIND, REPLACE, OS_SWAP, GAME_OF_LIFE };

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

static uint16_t find_key       = C(KC_F);
static uint16_t replace_key    = C(KC_H);

static uint16_t end_line_key   = KC_END;
static uint16_t start_line_key = KC_HOME;

// Sticky tab variables
static uint16_t tab_modifier = KC_LALT; // Alt for Windows/Linux, Cmd for macOS

// NOTE: The fall-through behavior in this handler is intentional per user comment.
static os_variant_t current_os = OS_UNSURE;

static void apply_os_settings(os_variant_t detected_os) {
    current_os = detected_os;
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
                go_back            = LALT(KC_LEFT);
                go_forward         = LALT(KC_RGHT);
                go_next_error      = LALT(KC_F8);
                go_prev_error      = LALT(S(KC_F8));
                tab_modifier       = KC_LALT; // Use Alt for Windows
                find_key           = LCTL(KC_F);
                replace_key        = LCTL(KC_H);
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
                find_key           = LCTL(KC_F);
                replace_key        = LCTL(KC_H);
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
                find_key                         = LGUI(KC_F);
                replace_key                      = LGUI(LALT(KC_F));
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
                find_key                         = LGUI(KC_F);
                replace_key                      = LGUI(LALT(KC_F));
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
}

bool process_detected_host_os_user(os_variant_t detected_os) {
    apply_os_settings(detected_os);
    return true;
}

#ifdef HLC_TFT_DISPLAY
#    define OS_ICON_SIZE 36
#    define OS_ICON_X (LCD_WIDTH - OS_ICON_SIZE - 5)
static uint16_t os_icon_y = 0;
#    define OS_ICON_Y os_icon_y

static bool game_on    = false;
static bool ui_dirty   = false;
static bool icon_dirty = true;
#    define HSV_WINDOWS_BLUE 147, 255, 255

static void draw_windows_icon(void) {
    const uint16_t pane = 17;
    const uint16_t step = pane + 2;
    for (uint16_t row = 0; row < 2; row++) {
        for (uint16_t col = 0; col < 2; col++) {
            uint16_t left = OS_ICON_X + col * step;
            uint16_t top  = OS_ICON_Y + row * step;
            qp_rect(lcd_surface, left, top, left + pane - 1, top + pane - 1, HSV_WINDOWS_BLUE, true);
        }
    }
}

static void draw_apple_icon(void) {
    const uint16_t x = OS_ICON_X + 6;
    const uint16_t y = OS_ICON_Y + 6;
    qp_circle(lcd_surface, x + 7, y + 15, 7, HSV_WHITE, true);
    qp_circle(lcd_surface, x + 16, y + 15, 7, HSV_WHITE, true);
    qp_circle(lcd_surface, x + 12, y + 18, 6, HSV_WHITE, true);
    qp_circle(lcd_surface, x + 12, y + 8, 2, HSV_BLACK, true);
    qp_circle(lcd_surface, x + 22, y + 12, 3, HSV_BLACK, true);
    qp_circle(lcd_surface, x + 15, y + 3, 3, HSV_WHITE, true);
}

bool display_module_housekeeping_task_user(bool second_display) {
    static os_variant_t drawn_os = OS_UNSURE;

    if (second_display || game_on || ui_dirty) {
        return true;
    }

    const os_variant_t shown_os = current_os != OS_UNSURE ? current_os : detected_host_os();

    if (icon_dirty || drawn_os != shown_os) {
        qp_rect(lcd_surface, OS_ICON_X, OS_ICON_Y, OS_ICON_X + OS_ICON_SIZE - 1, OS_ICON_Y + OS_ICON_SIZE - 1, HSV_BLACK, true);
        if (shown_os == OS_WINDOWS) {
            draw_windows_icon();
        } else if (shown_os == OS_MACOS) {
            draw_apple_icon();
        }
        drawn_os   = shown_os;
        icon_dirty = false;
    }
    return true;
}

#    include "hlc_tft_display/graphics/fonts/Retron2000-27.qff.h"
#    include "hlc_tft_display/graphics/fonts/Retron2000-underline-27.qff.h"
#    include "graphics/din40.qff.h"
#    include "graphics/din54.qff.h"

extern int color_value;
uint32_t   get_random_32bit(void);

#    define HSV_MODS 145, 150, 255

static painter_font_handle_t font           = NULL;
static painter_font_handle_t font_underline = NULL;
static painter_font_handle_t font_layer     = NULL;
static painter_font_handle_t font_layer_big = NULL;
static uint16_t              layer_y        = 0;
static uint16_t              caps_y         = 0;

bool module_post_init_user(void) {
    font           = qp_load_font_mem(font_Retron2000_27);
    font_underline = qp_load_font_mem(font_Retron2000_underline_27);
    font_layer     = qp_load_font_mem(font_din40);
    font_layer_big = qp_load_font_mem(font_din54);
    layer_y        = LCD_HEIGHT - font_layer_big->line_height - 5;
    caps_y         = layer_y - font->line_height - 6;
    os_icon_y      = layer_y - OS_ICON_SIZE - 5;
    return true;
}

typedef struct {
    uint16_t    code;
    const char *label;
} key_label_t;

static const key_label_t key_labels[] = {
    {KC_ENT, "Ent"},     {KC_ESC, "Esc"},      {KC_BSPC, "Bksp"},   {KC_TAB, "Tab"},      {KC_SPC, "Spc"},      {KC_MINS, "-"},       {KC_EQL, "="},        {KC_LBRC, "["},      {KC_RBRC, "]"},     {KC_BSLS, "\\"},    {KC_NUBS, "\\"},   {KC_SCLN, ";"},    {KC_QUOT, "'"},      {KC_GRV, "`"},      {KC_COMM, ","},    {KC_DOT, "."},     {KC_SLSH, "/"},
    {KC_CAPS, "Caps"},   {KC_DEL, "Del"},      {KC_INS, "Ins"},     {KC_HOME, "Home"},    {KC_END, "End"},      {KC_PGUP, "PgUp"},    {KC_PGDN, "PgDn"},    {KC_RGHT, "Right"},  {KC_LEFT, "Left"},  {KC_DOWN, "Down"},  {KC_UP, "Up"},     {KC_APP, "Menu"},  {KC_PSCR, "PrtSc"},  {KC_SCRL, "ScrLk"}, {KC_PAUS, "Pause"},
    {KC_MUTE, "Mute"},   {KC_VOLU, "Vol+"},   {KC_VOLD, "Vol-"},   {KC_MPRV, "Prev"},    {KC_MPLY, "Play"},    {KC_MNXT, "Next"},    {KC_PPLS, "+"},       {KC_PAST, "*"},      {MS_BTN1, "LClick"}, {MS_BTN2, "RClick"}, {MS_BTN3, "MClick"}, {MS_UP, "Mouse"}, {MS_DOWN, "Mouse"}, {MS_LEFT, "Mouse"}, {MS_RGHT, "Mouse"}, {MS_WHLU, "WhlUp"}, {MS_WHLD, "WhlDn"},
    {CW_TOGG, "CapsWd"}, {KC_EXLM, "!"},       {KC_AT, "@"},        {KC_HASH, "#"},       {KC_DLR, "$"},        {KC_PERC, "%"},       {KC_CIRC, "^"},       {KC_AMPR, "&"},      {KC_LPRN, "("},     {KC_RPRN, ")"},     {KC_UNDS, "_"},    {KC_LCBR, "{"},    {KC_RCBR, "}"},      {KC_PIPE, "|"},     {KC_COLN, ":"},    {KC_DQUO, "\""},   {KC_LABK, "<"},      {KC_RABK, ">"},     {KC_QUES, "?"},    {KC_TILD, "~"},
};

#    define OUTPUT_IGNORE 0xFFFF

static char    output_key[12] = "";
static uint8_t output_mods    = 0;
static bool    output_dirty   = true;
static bool    game_started   = false;

static void toggle_game(void) {
    game_on      = !game_on;
    game_started = false;
    ui_dirty     = true;
    icon_dirty   = true;
    output_dirty = true;
}

static const char *find_label(uint16_t code) {
    for (size_t i = 0; i < ARRAY_SIZE(key_labels); i++) {
        if (key_labels[i].code == code) {
            return key_labels[i].label;
        }
    }
    return NULL;
}

static uint8_t unpack_mods(uint8_t packed) {
    return (packed & 0x10) ? (packed & 0x0F) << 4 : packed & 0x0F;
}

static void append_label(char *out, size_t size, const char *label, const char *separator) {
    if (out[0] != '\0') {
        strncat(out, separator, size - strlen(out) - 1);
    }
    strncat(out, label, size - strlen(out) - 1);
}

static void build_mods_text(uint8_t mods, bool spaced, char *out, size_t size) {
    const char *separator = spaced ? " " : "";
    out[0]                = '\0';
    if (mods & MOD_MASK_CTRL) append_label(out, size, "Ctl", separator);
    if (mods & MOD_MASK_ALT) append_label(out, size, "Alt", separator);
    if (mods & MOD_MASK_GUI) append_label(out, size, current_os == OS_MACOS ? "Cmd" : "Win", separator);
    if (mods & MOD_MASK_SHIFT) append_label(out, size, "Sft", separator);
}

static void show_output(uint16_t code, uint8_t mods) {
    char        key[sizeof(output_key)] = "";
    const char *label                   = find_label(code);

    if (label == NULL && code <= QK_MODS_MAX) {
        mods |= unpack_mods(QK_MODS_GET_MODS(code));
        code  = QK_MODS_GET_BASIC_KEYCODE(code);
        label = find_label(code);
    }

    if (label != NULL) {
        snprintf(key, sizeof(key), "%s", label);
    } else if (code >= KC_A && code <= KC_Z) {
        const bool shifted = (mods & MOD_MASK_SHIFT) != 0;
        const bool others  = (mods & ~MOD_MASK_SHIFT) != 0;
        const bool caps    = host_keyboard_led_state().caps_lock || is_caps_word_on();
        const bool upper   = others || (shifted != caps);
        key[0]             = (upper ? 'A' : 'a') + (code - KC_A);
        if (!others) {
            mods &= ~MOD_MASK_SHIFT;
        }
    } else if (code >= KC_1 && code <= KC_0) {
        key[0] = code == KC_0 ? '0' : '1' + (code - KC_1);
    } else if (code >= KC_F1 && code <= KC_F12) {
        snprintf(key, sizeof(key), "F%u", (unsigned)(code - KC_F1 + 1));
    } else if (code >= KC_LCTL && code <= KC_RGUI) {
        mods |= MOD_BIT(code);
    } else if (code != KC_NO) {
        return;
    }

    snprintf(output_key, sizeof(output_key), "%s", key);
    output_mods  = mods;
    output_dirty = true;
}

static uint16_t resolve_output(uint16_t keycode, keyrecord_t *record, uint8_t *mods) {
    if (IS_QK_MOD_TAP(keycode)) {
        if (record->tap.count == 0) {
            *mods |= unpack_mods(QK_MOD_TAP_GET_MODS(keycode));
            return KC_NO;
        }
        return QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
    }
    if (IS_QK_LAYER_TAP(keycode)) {
        return record->tap.count == 0 ? OUTPUT_IGNORE : QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
    }
    switch (keycode) {
        case FIND:
            return find_key;
        case REPLACE:
            return replace_key;
        case UNDO:
            return undo_key;
        case REDO:
            return redo_key;
        case CUT:
            return cut_key;
        case COPY:
            return copy_key;
        case PASTE:
            return paste_key;
        case SELECT_ALL:
            return select_all_key;
        case GUI_SAVE:
            return save_key;
        case PREV_W:
            return prev_word_key;
        case NEXT_W:
            return next_word_key;
        case END_LINE:
            return end_line_key;
        case START_LINE:
            return start_line_key;
        case SLC_NEXT_WORD:
            return slc_next_word_key;
        case SLC_PREV_WORD:
            return slc_prev_word_key;
        case SLC_END_LINE:
            return slc_end_line_key;
        case SLC_START_LINE:
            return slc_start_line_key;
        case GOTOLINE:
            return goto_line_key;
        case GO_BACK:
            return go_back;
        case GO_FORWARD:
            return go_forward;
        case GO_NEXT_ERROR:
            return go_next_error;
        case GO_PREV_ERROR:
            return go_prev_error;
        case SCREENSHOT:
            return screenshot_key;
        case STAB_NEXT:
            *mods |= MOD_BIT(tab_modifier);
            return KC_TAB;
        case CTL_CLICK:
            *mods |= MOD_BIT(click_modifier);
            return MS_BTN1;
        default:
            return keycode;
    }
}

static void capture_output(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return;
    }
    if (keycode == OS_SWAP) {
        snprintf(output_key, sizeof(output_key), "%s", current_os == OS_MACOS ? "macOS" : "Windows");
        output_mods  = 0;
        output_dirty = true;
        return;
    }
    uint8_t  mods = get_mods() | get_weak_mods() | get_oneshot_mods();
    uint16_t code = resolve_output(keycode, record, &mods);
    if (code != OUTPUT_IGNORE) {
        show_output(code, mods);
    }
}

static const char *const layer_names[] = {"BASE", "SYM", "NAV", "SELECT", "FUNCTION", "LOL", "GAME"};

static const uint8_t layer_hues[] = {145, 20, 85, 0, 195, 40, 120, 225};

static void clear_line(uint16_t y, uint16_t height) {
    qp_rect(lcd_surface, 0, y, LCD_WIDTH - 1, y + height, HSV_BLACK, true);
}

static void draw_layer_name(uint8_t layer) {
    const char                  *name = layer_names[layer];
    const painter_font_handle_t  face = qp_textwidth(font_layer_big, name) <= LCD_WIDTH - 10 ? font_layer_big : font_layer;
    const uint16_t               y    = layer_y + font_layer_big->line_height - face->line_height;
    uint16_t                     x    = 5;

    clear_line(layer_y, font_layer_big->line_height);
    for (size_t i = 0; name[i] != '\0'; i++) {
        const char glyph[2] = {name[i], '\0'};
        qp_drawtext_recolor(lcd_surface, x, y, face, glyph, (uint8_t)(layer_hues[layer] + i * 20), 255, 255, HSV_BLACK);
        x += qp_textwidth(face, glyph);
    }
}

static void run_game(void) {
    static uint32_t last_draw         = 0;
    static uint32_t previous_activity = 0;

    if (!game_started) {
        srand(get_random_32bit());
        init_grid();
        color_value = rand() % 8;
        qp_rect(lcd_surface, 0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, HSV_BLACK, true);
        previous_activity = last_matrix_activity_time();
        game_started      = true;
    }

    if (timer_elapsed32(last_draw) >= 100) {
        draw_grid();
        update_grid();
        if (previous_activity != last_matrix_activity_time()) {
            color_value = rand() % 8;
            add_cell_cluster();
            previous_activity = last_matrix_activity_time();
        }
        last_draw = timer_read32();
    }
}

void update_display(void) {
    static uint8_t drawn_layer = UINT8_MAX;
    static int8_t  drawn_caps  = -1;

    if (font == NULL) {
        return;
    }

    if (game_on) {
        run_game();
        return;
    }

    if (ui_dirty) {
        qp_rect(lcd_surface, 0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, HSV_BLACK, true);
        drawn_layer = UINT8_MAX;
        drawn_caps  = -1;
        ui_dirty    = false;
    }

    const uint16_t height = font->line_height;
    const uint16_t mods_y = 5;
    const uint16_t key_y  = mods_y + height + 4;

    if (output_dirty) {
        char mods_text[24];
        build_mods_text(output_mods, true, mods_text, sizeof(mods_text));
        if (qp_textwidth(font, mods_text) > LCD_WIDTH - 10) {
            build_mods_text(output_mods, false, mods_text, sizeof(mods_text));
        }
        clear_line(mods_y, height);
        clear_line(key_y, height);
        qp_drawtext_recolor(lcd_surface, 5, mods_y, font, mods_text, HSV_MODS, HSV_BLACK);
        qp_drawtext_recolor(lcd_surface, 5, key_y, font, output_key, HSV_WHITE, HSV_BLACK);
        output_dirty = false;
    }

    const uint8_t layer = get_highest_layer(layer_state | default_layer_state);
    if (layer != drawn_layer && layer < ARRAY_SIZE(layer_names)) {
        draw_layer_name(layer);
        drawn_layer = layer;
    }

    const bool caps = host_keyboard_led_state().caps_lock || is_caps_word_on();
    if ((int8_t)caps != drawn_caps) {
        qp_rect(lcd_surface, 0, caps_y, OS_ICON_X - 1, caps_y + height, HSV_BLACK, true);
        if (caps) {
            qp_drawtext_recolor(lcd_surface, 5, caps_y, font_underline, "Caps", HSV_CAPS_ON, HSV_BLACK);
        } else {
            qp_drawtext_recolor(lcd_surface, 5, caps_y, font, "Caps", HSV_CAPS_OFF, HSV_BLACK);
        }
        drawn_caps = caps;
    }
}
#endif

static bool     is_sticky_tab_active = false;
static uint16_t sticky_tab_timer     = 0;

#define STICKY_TAB_TIMEOUT 1000 // 1 second timeout

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LSFT_T(KC_T):
        case SFT_T(KC_N):
            return true;
        default:
            return false;
    }
}

bool get_chordal_hold(uint16_t tap_hold_keycode, keyrecord_t *tap_hold_record, uint16_t other_keycode, keyrecord_t *other_record) {
    if (IS_QK_LAYER_TAP(tap_hold_keycode)) {
        return true;
    }
    switch (other_keycode) {
        case KC_MINS:
        case KC_DOT:
        case KC_SLSH:
        case KC_RBRC:
        case CTL_QUOT:
            return true;
        default:
            return get_chordal_hold_default(tap_hold_record, other_record);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint16_t my_hash_timer;
#ifdef HLC_TFT_DISPLAY
    capture_output(keycode, record);
#endif
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
        case OS_SWAP:
            if (record->event.pressed) {
                apply_os_settings(current_os == OS_MACOS ? OS_WINDOWS : OS_MACOS);
            }
            return false;
        case GAME_OF_LIFE:
            if (record->event.pressed) {
#ifdef HLC_TFT_DISPLAY
                toggle_game();
#endif
            }
            return false;
        case FIND:
            if (record->event.pressed) {
                tap_code16(find_key);
            }
            return false;
        case REPLACE:
            if (record->event.pressed) {
                tap_code16(replace_key);
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
            return false;
        }
        default:
            // Any other key pressed → stop sticky tab
            if (is_sticky_tab_active && record->event.pressed) {
                unregister_code(tab_modifier);
                is_sticky_tab_active = false;
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
    [_COLEMAK_DH] = LAYOUT(
    REPLACE  , KC_Q ,  KC_W   ,  KC_F   ,   KC_P ,   KC_B ,                                                                                           KC_J  ,   KC_L ,   KC_U ,   KC_Y ,KC_MINS, KC_DEL,
     FIND   , KC_A ,  LALT_T(KC_R)   ,  CTL_T(KC_S)   ,   LSFT_T(KC_T) ,   KC_G ,                                               KC_M  ,   SFT_T(KC_N) ,   CTL_T(KC_E) ,   LALT_T(KC_I) ,  KC_O , KC_MINS,
     OS_SWAP , KC_Z ,  KC_X   ,  KC_C   ,   KC_D ,   KC_V , CW_TOGG, KC_CAPS,                                                 FKEYS  ,     KC_RBRC, KC_K  ,   KC_H , DOT_DASH, KC_DOT ,KC_SLSH, CTL_QUOT,
                          TO(_GAME) , LT(_FUNCTION, KC_ESCAPE), LT(_SELECT, KC_SPACE) , LT(_NAV, KC_TAB),  STAB_NEXT           ,KC_RALT , LT(_SELECT, KC_ENT)    , LT(_SYM, KC_BSPC), KC_RGUI, TO(_LOL)
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
    [_NAV] = LAYOUT(
      _______, _______, PREV_W, KC_UP, NEXT_W , GOTOLINE,                                            GO_FORWARD, MS_WHLU, MS_UP,   MS_WHLD,  GO_PREV_ERROR, KC_VOLU,
      _______, START_LINE, KC_LEFT, KC_DOWN, KC_RIGHT, END_LINE,                                     GO_BACK, MS_LEFT, MS_DOWN, MS_RGHT, GO_NEXT_ERROR, KC_VOLD,
      _______, _______, S(KC_F4), KC_F4, KC_F12, _______, _______, KC_SCRL, _______, _______,KC_PAUSE, KC_MPRV, KC_MPLY, KC_MNXT, SCREENSHOT, KC_MUTE,
                                 _______, _______, MS_BTN1, MS_BTN2, _______,MS_BTN3, CTL_CLICK, MS_BTN1, MS_BTN2, _______


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
    [_SYM] = LAYOUT(
      KC_GRV ,   KC_AT ,   S(KC_3) ,   S(KC_9) ,   S(KC_0) ,   KC_DLR ,                                       KC_PPLS ,   KC_7 ,   KC_8 ,   KC_9 ,   KC_PAST , KC_EQL ,
     KC_LSFT , KC_EXLM ,  KC_CIRC     , KC_QUOT,  KC_GRV    , KC_SCLN,                                     KC_CIRC,    KC_4,    KC_5,    KC_6, KC_SLSH, KC_ENT,
     KC_NUBS , KC_AMPR , KC_PERC    , KC_QUES, KC_EXLM   , KC_BSLS, _______ , _______, _______, _______, KC_0,       KC_1,    KC_2,    KC_3, KC_DOT, KC_QUES,
                                       _______, KC_LBRC, KC_SPC , KC_RBRC,_______, _______, _______, _______, _______, _______

    ),

        [_SELECT] = LAYOUT(
     KC_TAB  , COPY ,      S(KC_LEFT)  ,   S(KC_UP)      ,   S(KC_RIGHT)    , SELECT_ALL   ,                                              _______,  PREV_W, KC_UP, NEXT_W , _______,_______, 
     KC_LSFT , PASTE ,  SLC_PREV_WORD   ,  S(KC_DOWN)     ,   SLC_NEXT_WORD , UNDO   ,                                                  START_LINE, KC_LEFT, KC_DOWN, KC_RIGHT, END_LINE,KC_H,
     CTL_ESC , CUT ,    SLC_START_LINE ,  KC_C  ,           SLC_END_LINE ,      REDO ,       REDO,        KC_CAPS  , REDO  ,           KC_RBRC, KC_N,   KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_RSFT,
                                ADJUST , KC_LGUI, ALT_ENT, KC_SPC , NAV     ,                                                            SYM    ,KC_ENT ,KC_BSPC, KC_RGUI, KC_APP

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
    [_FUNCTION] = LAYOUT(
      BL_TOGG,  KC_F9 ,  KC_F10,  KC_F11,  KC_F12, UG_NEXT,                                     _______, _______, _______, _______, _______, _______,
      BL_STEP,  KC_F5 ,  KC_F6 ,  KC_F7 ,  KC_F8 , UG_PREV,                                     _______, KC_RSFT, KC_RCTL, KC_LALT, KC_RGUI, _______,
      GAME_OF_LIFE,  KC_F1 ,  KC_F2 ,  KC_F3 ,  KC_F4 , _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______

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
    [_LOL] = LAYOUT(
     KC_ESCAPE  ,KC_ESCAPE, KC_1 ,  KC_2   ,  KC_3  ,   KC_4  ,                                        KC_Y,   KC_U ,  KC_I ,   KC_O ,  KC_P , KC_BSPC,
     KC_LSFT , KC_TAB, KC_Q ,  KC_W   ,  KC_E  ,   KC_R  ,                                        KC_H,   KC_J ,  KC_K ,   KC_L ,KC_SCLN,CTL_QUOT,
     KC_LCTL , KC_LSFT,KC_G ,  KC_B   ,  KC_D  ,   KC_F  ,  KC_V  ,  KC_H  ,     FKEYS  , KC_RBRC, KC_N,   KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_RSFT,
                                TO(_COLEMAK_DH), LT(_FUNCTION, KC_ESCAPE), LT(_SELECT, KC_SPACE) ,  KC_SPACE,  STAB_NEXT           ,KC_RALT , LT(_NAV, KC_ENT)    , LT(_SYM, KC_BSPC), KC_RGUI, TO(_COLEMAK_DH)
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
    [_GAME] = LAYOUT(
     KC_ESCAPE  , KC_LALT   ,  KC_Q   ,  KC_W  ,   KC_E ,   KC_R ,                                          KC_U ,  KC_I ,   KC_O ,  KC_P , KC_BSPC, KC_T , 
     KC_LSFT , KC_TAB ,  KC_A   ,  KC_S  ,   KC_D ,   KC_F ,                                         KC_J ,  KC_K ,   KC_L ,KC_SCLN,CTL_QUOT, KC_T , 
     KC_LCTL , KC_LSFT ,  KC_Z   ,  KC_X  ,   KC_C ,   KC_V , KC_LBRC,KC_CAPS,     FKEYS  , KC_RBRC,   KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_RSFT, KC_T , 
                                TO(_COLEMAK_DH), TD(TD_1) , TD(TD_3) , KC_SPACE,  STAB_NEXT           ,KC_RALT , LT(_NAV, KC_ENT)    , LT(_SYM, KC_BSPC), KC_RGUI, KC_APP

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
//     [_LAYERINDEX] = LAYOUT(
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
