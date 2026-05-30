// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// Turn off RGB matrix when the host suspends (PC off / sleep)
#define RGB_MATRIX_SLEEP

// 6 bytes: caps_h, caps_s, caps_v, scrl_h, scrl_s, scrl_v
#define EECONFIG_USER_DATA_SIZE 6
