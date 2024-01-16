// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#ifndef CONFIG_UART_H_
#define CONFIG_UART_H_

#include <stdint.h>

#include "config.hpp"

#define BUF_SIZE (1024)

typedef enum
{
    UART_CMD_RESET = 0x01,
    UART_CMD_CONFIG_SET_WIFI = 0x11,
    UART_CMD_CONFIG_GET_WIFI_SSID = 0x12,
} uart_cmd_t;

typedef enum
{
    UART_ERR_OK = 0x00,
    UART_ERR_FAIL = 0x01,
    UART_ERR_DISABLED = 0x02,
    UART_ERR_INVALID_CMD = 0x03,
    UART_ERR_INVALID_VALUE = 0x04,
} uart_err_t;

class UART
{
private:
    const char *TAG_ = "UART";
    Config config_;

    /**
     * @brief Handler for the UART_CMD_RESET command.
     *
     * Basically just resets the MCU
     */
    void Reset();

    /**
     * @brief Set the WiFi SSID in config
     *
     * @return uart_err_t
     */
    uart_err_t SetWifiSSID();

    /**
     * @brief Get the currently configured WiFi SSID
     *
     * @return uart_err_t
     */
    uart_err_t GetWifiSSID();

public:
    /**
     * @brief Construct a new UART object
     *
     * @param baud Baudrate to listen and transmit at
     */
    UART(int baud);

    /**
     * @brief Start listening for commands
     */
    void Listen();
};

#endif // CONFIG_UART_H_