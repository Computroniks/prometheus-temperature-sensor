// SPDX-FileCopyrightText: 2023-2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#include <sys/stat.h>

#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_spiffs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "config/uart.hpp"
#include "sensor/aht10.hpp"

#define SPIFFS_MAX_FILES 3

static const char *TAG = "main";

void show_startup_info()
{
    // Firmware info
    ESP_LOGI(TAG, "%s %s", PROJECT_NAME, PROJECT_VERSION);
    ESP_LOGI(TAG, "Compiled %s %s", __DATE__, __TIME__);
    ESP_LOGI(TAG, "Repository %s", PROJECT_REPO);

    // IC Info
    esp_chip_info_t chip;
    esp_chip_info(&chip);
    ESP_LOGI(
        TAG, "Model: %s",
        (chip.model == CHIP_ESP8266) ? "ESP8266" : "ESP32");
    ESP_LOGI(TAG, "Silicon revision: %d", chip.revision);
    ESP_LOGI(TAG, "Cores: %d", chip.cores);
    ESP_LOGI(TAG, "Crystal: %dMHz", CRYSTAL_USED);

    // Chip features
    ESP_LOGI(TAG, "Features:");
    ESP_LOGI(
        TAG, "Embedded flash: %s",
        (chip.features & CHIP_FEATURE_EMB_FLASH) ? "Yes" : "No");
    ESP_LOGI(
        TAG, "2.4GHz WiFi: %s",
        (chip.features & CHIP_FEATURE_WIFI_BGN) ? "Yes" : "No");
    ESP_LOGI(
        TAG, "Bluetooth LE: %s",
        (chip.features & CHIP_FEATURE_BLE) ? "Yes" : "No");
    ESP_LOGI(
        TAG, "Bluetooth Classic: %s",
        (chip.features & CHIP_FEATURE_BT) ? "Yes" : "No");

    // MAC addresses
    unsigned char mac[8];
    esp_efuse_mac_get_default(mac);
    ESP_LOGI(
        TAG, "MAC Address: %02x:%02x:%02x:%02x:%02x:%02x",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // Flash Info
    ESP_LOGI(
        TAG, "%dMB %s flash",
        spi_flash_get_chip_size() / (1024 * 1024),
        (chip.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}

esp_err_t ensure_file(char *filename)
{
    struct stat st;
    if (stat(filename, &st) != 0)
    {
        ESP_LOGI(TAG, "%s does not exist. Creating it", filename);
        FILE *f = fopen(filename, "w");
        if (f == NULL)
        {
            ESP_LOGE(TAG, "Failed to create file");
            return ESP_FAIL;
        }
        fclose(f);
    }
    return ESP_OK;
}

esp_err_t init_spiffs()
{
    ESP_LOGI(TAG, "Initialising SPIFFS file system");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = SPIFFS_MAX_FILES,
        .format_if_mount_failed = true,
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK)
    {
        switch (ret)
        {
        case ESP_FAIL:
            ESP_LOGE(TAG, "ESP_FAIL: Failed to mount or format file system");
            break;
        case ESP_ERR_NOT_FOUND:
            ESP_LOGE(TAG, "ESP_ERR_NOT_FOUND: Could not find SPIFFS partition");
            break;
        default:
            ESP_LOGE(TAG, "Failed to initialise SPIFFS (%s)", esp_err_to_name(ret));
            break;
        }
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ret;
    }

    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    ret = ensure_file("/spiffs/wificonfig");
    if (ret != ESP_OK)
    {
        return ret;
    }

    ret = ensure_file("/spiffs/basicauth");
    if (ret != ESP_OK)
    {
        return ret;
    }

    ret = ensure_file("/spiffs/accesscontrol");
    if (ret != ESP_OK)
    {
        return ret;
    }

    return ESP_OK;
}

void uart_task(void *arg)
{
    UART uart = UART(74800);
    uart.Listen();
}

extern "C" void app_main()
{
    show_startup_info();
    init_spiffs();

    AHT10 sensor = AHT10(GPIO_NUM_0, GPIO_NUM_2, I2C_NUM_0, 0x38);

    aht10_measurement_t res = {};
    sensor.Measure(&res);

    xTaskCreate(uart_task, "uart_listen", 2048, NULL, 10, NULL);
}
