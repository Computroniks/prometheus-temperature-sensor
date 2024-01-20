// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "config/config.hpp"

const char* Config::TAG_ = "config";

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

esp_err_t Config::InitNVS() {
    ESP_LOGI(TAG_, "Initialising NVS");
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGI(TAG_, "Partition truncated. Erasing");

        err = nvs_flash_erase();
        if (err != ESP_OK) {
            ESP_LOGE(TAG_, "Failed to erase NVS partition");
            return err;
        }

        err = nvs_flash_init();
        if (err != ESP_OK) {
            ESP_LOGE(TAG_, "Failed to initialise NVS partition after erasing");
            return err;
        }
    }
    return ESP_OK;
}

esp_err_t Config::EraseWiFiConfig() {
    ESP_LOGI(TAG_, "Clearing WiFi config");
    esp_err_t err = nvs_flash_erase();
    if (err != ESP_OK) {
        ESP_LOGW(TAG_, "Failed to clear clear WiFi configuration: %s", esp_err_to_name(err));
        return err;
    }

    return InitNVS();
}

Config::Config() {
    ESP_ERROR_CHECK(EnsureFile(basic_auth_config_path_));
    ESP_ERROR_CHECK(EnsureFile(access_control_config_path_));
}
