// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"

#include "esp_system.h"
#include "driver/uart.h"

#include "config/uart.hpp"
#include "config/config.hpp"

void UART::Reset()
{
    esp_restart();
}

uart_err_t UART::SetWiFiSSID()
{
    char ssid[33] = "";
    int i = 0;

    bool data = true;
    while (data)
    {
        uint8_t buf[1];
        int len = uart_read_bytes(UART_NUM_0, buf, 1, 20 / portTICK_RATE_MS);
        if (len < 1)
        {
            ESP_LOGD(TAG_, "Failed to read data from UART buffer");
            return UART_ERR_FAIL;
        }

        // Check length of SSID if this is an end of line signal
        if (buf[0] != 0x00 && i >= 32)
        {
            // SSID too long
            return UART_ERR_INVALID_VALUE;
        }

        if (buf[0] == 0x00 && i < 2)
        {
            // SSID too short
            return UART_ERR_INVALID_VALUE;
        }

        switch (buf[0])
        {
        case 0x00:
            config_.SetWiFiSSID(ssid);
            return UART_ERR_OK;

        default:
            ssid[i] = buf[0];
            break;
        }

        i++;
    }
    // Shouldn't get here but if we do, something has gone wrong
    return UART_ERR_FAIL;
}

uart_err_t UART::SetWiFiKey()
{
    char key[64] = "";
    int i = 0;

    bool data = true;
    while (data)
    {
        uint8_t buf[1];
        int len = uart_read_bytes(UART_NUM_0, buf, 1, 20 / portTICK_RATE_MS);
        if (len < 1)
        {
            ESP_LOGD(TAG_, "Failed to read data from UART buffer");
            return UART_ERR_FAIL;
        }

        // Check length of key if this is an end of line signal
        if (buf[0] != 0x00 && i >= 63)
        {
            // Key too long
            return UART_ERR_INVALID_VALUE;
        }

        if (buf[0] == 0x00 && i < 8)
        {
            // Key too short
            return UART_ERR_INVALID_VALUE;
        }

        switch (buf[0])
        {
        case 0x00:
            config_.SetWiFiKey(key);
            return UART_ERR_OK;
        default:
            key[i] = buf[0];
            break;
        }

        i++;
    }

    // Shouldn't get here but if we do, something has gone wrong
    return UART_ERR_FAIL;
}

uart_err_t UART::GetWiFiSSID()
{
    ESP_LOGD(TAG_, "Fetching Wifi SSID");
    config_wifi_t conf;
    config_.GetWiFi(&conf);
    ESP_LOGD(TAG_, "Fetched config");

    if (conf.type == CONFIG_WIFI_DISABLED)
    {
        return UART_ERR_DISABLED;
    }
    uart_write_bytes(UART_NUM_0, conf.ssid, strlen(conf.ssid));
    free(conf.ssid);
    free(conf.key);
    return UART_ERR_OK;
}

UART::UART(int baud)
{
    uart_config_t conf = {
        .baud_rate = baud,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &conf));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0));
}

void UART::Listen()
{
    uint8_t cmd;
    while (1)
    {
        // Poll for a command
        int len = uart_read_bytes(UART_NUM_0, &cmd, 1, 20 / portTICK_RATE_MS);
        if (len == 0)
        {
            continue;
        }

        uart_err_t status[1] = {UART_ERR_OK};

        switch (cmd)
        {
        case UART_CMD_RESET:
            Reset();
            break;

        case UART_CMD_CONFIG_SET_WIFI_SSID:
            status[0] = SetWiFiSSID();
            break;

        case UART_CMD_CONFIG_GET_WIFI_SSID:
            status[0] = GetWiFiSSID();
            break;

        case UART_CMD_CONFIG_SET_WIFI_AUTH:
            status[0] = SetWiFiKey();
            break;

        default:
            status[0] = UART_ERR_INVALID_CMD;
            break;
        }

        uart_write_bytes(UART_NUM_0, (const char *)status, 1);
        cmd = 0;
    }
}
