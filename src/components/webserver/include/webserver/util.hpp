// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#ifndef WEBSERVER_UTIL_H_
#define WEBSERVER_UTIL_H_

#include "esp_http_server.h"
#include "sys/socket.h"

#include "sensor/aht10.hpp"

/**
 * @brief Get the IP of the calling client
 *
 * @param req Client HTTP request
 * @param ip Pointer to store IP
 * @return esp_err_t
 */
esp_err_t webserver_util_get_client_ip(httpd_req_t* req, char ip[INET6_ADDRSTRLEN]);

/**
 * @brief Format the metrics string
 *
 * @return char*
 */
char* webserver_util_format_metrics();

/**
 * @brief Set the sensor for the webserver to use
 *
 * @param sensor Sensor to use
 */
void webserver_util_set_sensor(AHT10* sensor);


#endif // WEBSERVER_UTIL_H_
