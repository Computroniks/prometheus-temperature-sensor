// SPDX-FileCopyrightText: 2023-2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_spiffs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "wlan.hpp"
#include "config/uart.hpp"
#include "sensor/aht10.hpp"
#include "webserver/server.hpp"

#define SPIFFS_MAX_FILES 4

static const char* TAG_ = "main";

void show_startup_info() {
    // Firmware info
    ESP_LOGI(TAG_, "%s %s", PROJECT_NAME, PROJECT_VERSION);
    ESP_LOGI(TAG_, "Compiled %s %s", __DATE__, __TIME__);
    ESP_LOGI(TAG_, "Repository %s", PROJECT_REPO);

    // IC Info
    esp_chip_info_t chip;
    esp_chip_info(&chip);
    ESP_LOGI(
        TAG_, "Model: %s",
        (chip.model == CHIP_ESP8266) ? "ESP8266" : "ESP32");
    ESP_LOGI(TAG_, "Silicon revision: %d", chip.revision);
    ESP_LOGI(TAG_, "Cores: %d", chip.cores);
    ESP_LOGI(TAG_, "Crystal: %dMHz", CRYSTAL_USED);

    // Chip features
    ESP_LOGI(TAG_, "Features:");
    ESP_LOGI(
        TAG_, "Embedded flash: %s",
        (chip.features & CHIP_FEATURE_EMB_FLASH) ? "Yes" : "No");
    ESP_LOGI(
        TAG_, "2.4GHz WiFi: %s",
        (chip.features & CHIP_FEATURE_WIFI_BGN) ? "Yes" : "No");
    ESP_LOGI(
        TAG_, "Bluetooth LE: %s",
        (chip.features & CHIP_FEATURE_BLE) ? "Yes" : "No");
    ESP_LOGI(
        TAG_, "Bluetooth Classic: %s",
        (chip.features & CHIP_FEATURE_BT) ? "Yes" : "No");

    // MAC addresses
    unsigned char mac[8];
    esp_efuse_mac_get_default(mac);
    ESP_LOGI(
        TAG_, "MAC Address: %02x:%02x:%02x:%02x:%02x:%02x",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // Flash Info
    ESP_LOGI(
        TAG_, "%dMB %s flash",
        spi_flash_get_chip_size() / (1024 * 1024),
        (chip.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}

esp_err_t init_spiffs() {
    ESP_LOGI(TAG_, "Initialising SPIFFS file system");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = SPIFFS_MAX_FILES,
        .format_if_mount_failed = true,
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        switch (ret) {
        case ESP_FAIL:
            ESP_LOGE(TAG_, "ESP_FAIL: Failed to mount or format file system");
            break;
        case ESP_ERR_NOT_FOUND:
            ESP_LOGE(TAG_, "ESP_ERR_NOT_FOUND: Could not find SPIFFS partition");
            break;
        default:
            ESP_LOGE(TAG_, "Failed to initialise SPIFFS (%s)", esp_err_to_name(ret));
            break;
        }
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG_, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ret;
    }

    {
        ESP_LOGI(TAG_, "Partition size: total: %d, used: %d", total, used);
    }

    return ESP_OK;
}

void uart_task(void* arg) {
    AHT10* sensor = (AHT10*)arg;
    UART uart = UART(74800, sensor);
    uart.Listen();
}

extern "C" void app_main() {
    show_startup_info();
    init_spiffs();

    AHT10 sensor = AHT10(GPIO_NUM_0, GPIO_NUM_2, I2C_NUM_0, 0x38);
    // Start UART command handler first after initial startup
    xTaskCreate(uart_task, "uart_listen", 2048, &sensor, 10, NULL);

    network_init();
    // Server server = Server(80, &sensor);
    webserver_start(80, &sensor);
    // server.Listen();
}
