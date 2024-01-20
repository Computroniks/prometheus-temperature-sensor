// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#ifndef WEBSERVER_SERVER_H_
#define WEBSERVER_SERVER_H_

#include "esp_err.h"
#include "esp_http_server.h"
#include "sys/socket.h"

#include "sensor/aht10.hpp"

/**
 * @brief Register the request handlers for the server
 *
 * @return esp_err_t
 */
static esp_err_t webserver_register_handlers_();

/**
 * @brief Start the web server
 *
 * @param port Port to start server on
 * @param sensor AHT10 sensor to use
 * @return esp_err_t
 */
esp_err_t webserver_start(uint16_t port, AHT10* sensor);

#endif // WEBSERVER_SERVER_H_
