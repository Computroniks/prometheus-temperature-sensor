# SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
# SPDX-License-Identifier: MIT

idf_component_register(SRCS "server.cpp" "util.cpp" "handlers.cpp" INCLUDE_DIRS "include" PRIV_INDLUDE "include/webserver" PRIV_REQUIRES esp_http_server sensor)
