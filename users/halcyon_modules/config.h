// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define SPLIT_TRANSACTION_IDS_KB MODULE_SYNC

#include_next <mcuconf.h>

#undef RP_PWM_USE_PWM5
#define RP_PWM_USE_PWM5 TRUE

#define HAL_USE_PWM TRUE

#define SPLIT_POINTING_ENABLE
#define POINTING_DEVICE_COMBINED

#define HLC_BACKLIGHT_TIMEOUT 120000

#define BACKLIGHT_PWM_DRIVER PWMD5
#define BACKLIGHT_LEVELS 10
#define BACKLIGHT_PWM_CHANNEL RP2040_PWM_CHANNEL_B

#if !defined(HLC_TFT_DISPLAY)
    #define BACKLIGHT_PIN NO_PIN
#endif

#if !defined(HLC_CIRQUE_TRACKPAD)
    #define POINTING_DEVICE_CS_PIN GP2 //NOT CONNECTED
#endif

#include_next <halconf.h>

//// Keyboard redefines

// Always the same
#define SPLIT_MODS_ENABLE
#define SPLIT_LED_STATE_ENABLE
#define SPLIT_LAYER_STATE_ENABLE
#ifndef HLC_ENCODER_A
#   define HLC_ENCODER_A NO_PIN
#endif
#ifndef HLC_ENCODER_B
#   define HLC_ENCODER_B NO_PIN
#endif

#define ENABLE_RGB_MATRIX_TYPING_HEATMAP true

// Kyria
#if PRODUCT_ID == 0x7FCE
    #undef ENCODER_A_PINS
    #define ENCODER_A_PINS { GP23, HLC_ENCODER_A }
    #undef ENCODER_B_PINS
    #define ENCODER_B_PINS { GP22, HLC_ENCODER_B }
    #undef MATRIX_ROWS
    #define MATRIX_ROWS 10
    #define LAYOUT_split_3x6_5_hlc(k0G, k0F, k0E, k0D, k0C, k0B, k5B, k5C, k5D, k5E, k5F, k5G, k1G, k1F, k1E, k1D, k1C, k1B, k6B, k6C, k6D, k6E, k6F, k6G, k2G, k2F, k2E, k2D, k2C, k2B, k3D, k2A, k7A, k8D, k7B, k7C, k7D, k7E, k7F, k7G, k3E, k3C, k3B, k3F, k3A, k8A, k8F, k8B, k8C, k8E, k4A, k4B, k4C, k4D, k4E, k9A, k9B, k9C, k9D, k9E) { \
        {KC_NO, k0B, k0C, k0D, k0E, k0F, k0G}, \
        {KC_NO, k1B, k1C, k1D, k1E, k1F, k1G}, \
        {k2A, k2B, k2C, k2D, k2E, k2F, k2G}, \
        {k3A, k3B, k3C, k3D, k3E, k3F, KC_NO}, \
        {k4A, k4B, k4C, k4D, k4E, KC_NO, KC_NO}, \
        {KC_NO, k5B, k5C, k5D, k5E, k5F, k5G}, \
        {KC_NO, k6B, k6C, k6D, k6E, k6F, k6G}, \
        {k7A, k7B, k7C, k7D, k7E, k7F, k7G}, \
        {k8A, k8B, k8C, k8D, k8E, k8F, KC_NO}, \
        {k9A, k9B, k9C, k9D, k9E, KC_NO, KC_NO} \
    }
#endif
