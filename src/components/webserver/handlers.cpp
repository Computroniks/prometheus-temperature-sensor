// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#include "handlers.hpp"

#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "sys/socket.h"

#include "util.hpp"
#include "sensor/aht10.hpp"

static const char TAG_[] = "webserver_handlers";


esp_err_t webserver_handler_get_metrics(httpd_req_t* req) {
    char ipstr[INET6_ADDRSTRLEN] = "";
    webserver_util_get_client_ip(req, ipstr);

    size_t user_agent_len = 0;
    user_agent_len = httpd_req_get_hdr_value_len(req, "User-Agent") + 1;
    char user_agent[user_agent_len];
    if (user_agent_len > 1) {
        httpd_req_get_hdr_value_str(req, "User-Agent", user_agent, user_agent_len);
    }

    ESP_LOGI(TAG_, "GET /metrics from IP: %s User-Agent: %s", ipstr, user_agent);
    httpd_resp_set_hdr(req, "Content-Type", "text/plain; version=0.0.4");

    aht10_measurement_t measurement;
    esp_err_t err = webserver_util_get_measurement(&measurement);
    if (err != ESP_OK) {
        httpd_resp_send_500(req);
        ESP_LOGW(TAG_, "HTTP 500 caused by %s", esp_err_to_name(err));
        return ESP_FAIL;
    }

    char* resp_buf = webserver_util_format_metrics(&measurement);
    httpd_resp_send(req, resp_buf, strlen(resp_buf));
    free(resp_buf);
    return ESP_OK;
}
