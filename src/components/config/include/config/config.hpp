// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#ifndef CONFIG_CONFIG_H_
#define CONFIG_CONFIG_H_

#include "esp_err.h"

typedef enum
{
    CONFIG_WIFI_DISABLED = 0,
    CONFIG_WIFI_OPEN = 1,
    CONFIG_WIFI_WPA = 2,
} config_wifi_net_type_t;

struct config_wifi_t
{
    config_wifi_net_type_t type;
    char *ssid;
    char *key;
};

class Config
{
private:
    const char *TAG_ = "config";
    const char *ssid_path_ = "/spiffs/ssid";
    const char *wpa_key_path_ = "/spiffs/wpa_key";
    const char *basic_auth_config_path_ = "/spiffs/basicauth";
    const char *access_control_config_path_ = "/spiffs/accesscontrol";

    /**
     * @brief Ensure that the given file exists
     *
     * @param filename File to check
     * @return esp_err_t
     */
    esp_err_t EnsureFile(const char *filename);

public:
    /**
     * @brief Set SSID for open WiFi network
     *
     * @param ssid SSID to connect to
     * @return esp_err_t
     */
    esp_err_t SetWiFiSSID(char *ssid);

    /**
     * @brief Set key for WPA Personal WiFi network
     *
     * @param key Key to use for authentication
     * @return esp_err_t
     */
    esp_err_t SetWiFiKey(char *key);

    /**
     * @brief Retrieve WiFi config
     *
     * @param config Struct to populate with config
     * @return esp_err_t
     */
    esp_err_t GetWiFi(config_wifi_t *config);

    Config();
};

#endif // CONFIG_CONFIG_H_
