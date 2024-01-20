// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#ifndef CONFIG_CONFIG_H_
#define CONFIG_CONFIG_H_

#include "esp_err.h"

typedef enum {
    CONFIG_WIFI_DISABLED = 0,
    CONFIG_WIFI_OPEN = 1,
    CONFIG_WIFI_WPA = 2,
} config_wifi_net_type_t;

struct config_wifi_t {
    config_wifi_net_type_t type;
    char* ssid;
    char* key;
};

class Config {
private:
    static const char* TAG_;
    const char* basic_auth_config_path_ = "/spiffs/basicauth";
    const char* access_control_config_path_ = "/spiffs/accesscontrol";

    /**
     * @brief Ensure that the given file exists
     *
     * @param filename File to check
     * @return esp_err_t
     */
    esp_err_t EnsureFile(const char* filename);

public:

    /**
     * @brief Initialise NVS flash for WiFi config storage
     *
     * @return esp_err_t
     */
    static esp_err_t InitNVS();

    /**
     * @brief Clear NVS flash partition to clear all WiFi config.
     *
     * @return esp_err_t
     */
    esp_err_t EraseWiFiConfig();

    /**
     * @brief Construct a new Config object
     * Ensures that all required files are present. Aborts if files
     * could not be created.
     */
    Config();
};

#endif // CONFIG_CONFIG_H_
