// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"

#include "config/config.hpp"

esp_err_t Config::SetWiFi(char *ssid)
{
    ESP_LOGD(TAG_, "Setting SSID to %s", ssid);
    FILE *f = fopen(wifi_config_path_, "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG_, "Failed to open file %s for writing", wifi_config_path_);
        return ESP_FAIL;
    }
    fprintf(f, ssid);
    fclose(f);
    return ESP_OK;
}

esp_err_t Config::SetWiFi(char *ssid, char *key)
{
    ESP_LOGD(TAG_, "Setting SSID to %s and key to %s", ssid, key);
    FILE *f = fopen(wifi_config_path_, "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG_, "Failed to open file %s for writing", wifi_config_path_);
        return ESP_FAIL;
    }
    fprintf(f, "%s\n%s", ssid, key);
    fclose(f);
    return ESP_OK;
}

esp_err_t Config::GetWiFi(config_wifi_t *config)
{
    ESP_LOGD(TAG_, "Retrieving WiFi configuration");
    FILE *f = fopen(wifi_config_path_, "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG_, "Failed to open file %s for writing", wifi_config_path_);
        return ESP_FAIL;
    }

    char ssid[33];
    if (fgets(ssid, 33, f) == NULL)
    {
        config->type = CONFIG_WIFI_DISABLED;
        fclose(f);
        return ESP_OK;
    }
    config->ssid = strdup(ssid);

    char key[64];
    if (fgets(key, 64, f) == NULL)
    {
        config->type = CONFIG_WIFI_OPEN;
    }
    else
    {
        config->type = CONFIG_WIFI_WPA;
    }
    config->key = strdup(key);

    fclose(f);
    return ESP_OK;
}
