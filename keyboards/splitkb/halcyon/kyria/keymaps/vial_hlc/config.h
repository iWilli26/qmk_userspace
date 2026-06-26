// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// Split keyboard pointing device configuration
#define SPLIT_POINTING_ENABLE
#define POINTING_DEVICE_COMBINED

// Specify that the pointing device is on the right side (slave)
#define POINTING_DEVICE_RIGHT

// Ensure proper communication for trackpad data
#define SPLIT_TRANSPORT_MIRROR
#define SPLIT_TRANSACTION_IDS_KB MODULE_SYNC

// Master-slave hand detection - ensure left is master
#define MASTER_LEFT

// Enable additional split features for better communication
#define SPLIT_MODS_ENABLE
#define SPLIT_LED_STATE_ENABLE
#define SPLIT_LAYER_STATE_ENABLE

// Trackpad-specific optimizations
#define POINTING_DEVICE_TASK_THROTTLE_MS 8
#define POINTING_DEVICE_AUTO_MOUSE_ENABLE