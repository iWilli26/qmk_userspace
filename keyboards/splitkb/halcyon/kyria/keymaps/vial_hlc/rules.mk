ENCODER_MAP_ENABLE = yes

# This adds module functionality to your keyboard (files found in users/halcyon_modules)
USER_NAME := halcyon_modules
CAPS_WORD_ENABLE = yes
OS_DETECTION_ENABLE = yes
CONSOLE_ENABLE = yes
TAP_DANCE_ENABLE = yes
COMBO_ENABLE = yes
MOUSEKEY_ENABLE = yes

ifdef HLC_TFT_DISPLAY
    SRC += graphics/din40.qff.c
    SRC += graphics/din54.qff.c
endif
