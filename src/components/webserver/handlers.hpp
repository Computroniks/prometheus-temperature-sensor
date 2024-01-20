// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#ifndef WEBSERVER_HANDLERS_H_
#define WEBSERVER_HANDLERS_H_

#include "esp_err.h"
#include "esp_http_server.h"

/**
 * @brief Handler for the /metrics URL
 *
 * @param req HTTP request
 * @return esp_err_t
 */
esp_err_t webserver_handler_get_metrics(httpd_req_t* req);

#endif // WEBSERVER_HANDLERS_H_
