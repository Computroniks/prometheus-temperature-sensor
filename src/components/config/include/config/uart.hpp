// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#ifndef CONFIG_UART_H_
#define CONFIG_UART_H_

#include <stdint.h>

#include "config.hpp"
#include "sensor/aht10.hpp"

#define BUF_SIZE (1024)

typedef enum {
    UART_CMD_RESET = 0x01,
    UART_CMD_CONFIG_SET_WIFI_SSID = 0x11,
    UART_CMD_CONFIG_GET_WIFI_SSID = 0x12,
    UART_CMD_CONFIG_SET_WIFI_AUTH = 0x13,
    UART_CMD_CONFIG_CLEAR_WIFI = 0x14,
    UART_CMD_SENSOR_GET_TEMP = 0x20,
    UART_CMD_SENSOR_GET_HUMIDITY = 0x21,
} uart_cmd_t;

typedef enum {
    UART_ERR_OK = 0x00,
    UART_ERR_FAIL = 0x01,
    UART_ERR_DISABLED = 0x02,
    UART_ERR_INVALID_CMD = 0x03,
    UART_ERR_INVALID_VALUE = 0x04,
    UART_ERR_NOT_IMPLEMENTED = 0x05,
} uart_err_t;

class UART {
private:
    const char* TAG_ = "UART";
    Config config_;
    AHT10* sensor_;

    /**
     * @brief Handler for the UART_CMD_RESET command.
     *
     * Basically just resets the MCU
     */
    void Reset();

    /**
     * @brief Handler for resetting WiFi configuration
     *
     * @return uart_err_t
     */
    uart_err_t ResetWiFiConf();

    /**
     * @brief Get the current temperature measurement
     *
     * @return uart_err_t
     */
    uart_err_t GetTemp();

    /**
     * @brief Get the current humidity measurement
     *
     * @return uart_err_t
     */
    uart_err_t GetHumidity();

public:
    /**
     * @brief Construct a new UART object
     *
     * @param baud Baudrate to listen and transmit at
     */
    UART(int baud, AHT10* sensor);

    /**
     * @brief Start listening for commands
     */
    void Listen();
};

#endif // CONFIG_UART_H_
