# SPDX-FileCopyrightText: 2023-2024 Sidings Media <contact@sidingsmedia.com>
# SPDX-License-Identifier: MIT

idf_component_register(SRCS "main.cpp" "wlan.cpp" INCLUDE_DIRS ".")

set(PRJ_VERSION_MAJOR 0)
set(PRJ_VERSION_MINOR 1)
set(PRJ_VERSION_PATCH 0)
set(PRJ_VERSION ${PRJ_VERSION_MAJOR}.${PRJ_VERSION_MINOR}.${PRJ_VERSION_PATCH})
set(PRJ_NAME "temperature_sensor")

# We have to have this before the project call else the definitions are
# not accessible by project
add_compile_definitions(
    PROJECT_VERSION_MAJOR=${PRJ_VERSION_MAJOR}
    PROJECT_VERSION_MINOR=${PRJ_VERSION_MINOR}
    PROJECT_VERSION_PATCH=${PRJ_VERSION_PATCH}
    PROJECT_VERSION="${PRJ_VERSION}"
    PROJECT_NAME="${PRJ_NAME}"
    PROJECT_REPO="https://github.com/SidingsMedia/prometheus-temperature-sensor.git"
)