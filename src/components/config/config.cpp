// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "esp_err.h"
#include "esp_log.h"

#include "config/config.hpp"

esp_err_t Config::EnsureFile(const char* filename) {
    struct stat st;
    if (stat(filename, &st) != 0) {
        ESP_LOGI(TAG_, "%s does not exist. Creating it", filename);
        FILE* f = fopen(filename, "w");
        if (f == NULL) {
            ESP_LOGE(TAG_, "Failed to create file");
            return ESP_FAIL;
        }
        fclose(f);
    }
    return ESP_OK;
}

esp_err_t Config::SetWiFiSSID(char* ssid) {
    ESP_LOGD(TAG_, "Setting SSID to %s", ssid);
    FILE* f = fopen(ssid_path_, "w");
    if (f == NULL) {
        ESP_LOGE(TAG_, "Failed to open file %s for writing", ssid_path_);
        return ESP_FAIL;
    }
    fprintf(f, ssid);
    fclose(f);
    return ESP_OK;
}

esp_err_t Config::SetWiFiKey(char* key) {
    ESP_LOGD(TAG_, "Setting key to %s", key);
    FILE* f = fopen(wpa_key_path_, "w");
    if (f == NULL) {
        ESP_LOGE(TAG_, "Failed to open file %s for writing", wpa_key_path_);
        return ESP_FAIL;
    }
    fprintf(f, key);
    fclose(f);
    return ESP_OK;
}

esp_err_t Config::GetWiFi(config_wifi_t* config) {
    ESP_LOGD(TAG_, "Retrieving WiFi configuration");
    FILE* f = fopen(ssid_path_, "r");
    if (f == NULL) {
        ESP_LOGE(TAG_, "Failed to open file %s for writing", ssid_path_);
        return ESP_FAIL;
    }

    char ssid[33];
    if (fgets(ssid, 33, f) == NULL) {
        config->type = CONFIG_WIFI_DISABLED;
        fclose(f);
        return ESP_OK;
    }
    config->ssid = strdup(ssid);
    fclose(f);

    f = fopen(wpa_key_path_, "r");
    if (f == NULL) {
        ESP_LOGE(TAG_, "Failed to open file %s for writing", wpa_key_path_);
        return ESP_FAIL;
    }
    char key[64];
    if (fgets(key, 64, f) == NULL) {
        config->type = CONFIG_WIFI_OPEN;
    }
    else {
        config->type = CONFIG_WIFI_WPA;
    }
    config->key = strdup(key);

    fclose(f);
    return ESP_OK;
}

Config::Config() {
    ESP_ERROR_CHECK(EnsureFile(ssid_path_));
    ESP_ERROR_CHECK(EnsureFile(wpa_key_path_));
    ESP_ERROR_CHECK(EnsureFile(basic_auth_config_path_));
    ESP_ERROR_CHECK(EnsureFile(access_control_config_path_));
}
