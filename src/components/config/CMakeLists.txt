# SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
# SPDX-License-Identifier: MIT

idf_component_register(SRCS "config.cpp" "uart.cpp" INCLUDE_DIRS "include" PRIV_INDLUDE "include/config" PRIV_REQUIRES nvs_flash)
