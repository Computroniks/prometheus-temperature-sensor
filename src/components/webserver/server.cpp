// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#include "server.hpp"

#include "esp_err.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"

#include "handlers.hpp"
#include "sensor/aht10.hpp"
#include "util.hpp"

static const char TAG_[] = "webserver";

static AHT10* sensor_;
static uint16_t port_;

httpd_handle_t server_ = NULL;

esp_err_t webserver_register_handlers_() {
    ESP_LOGI(TAG_, "Registering URI handlers");
    esp_err_t err;
    httpd_uri_t metrics = {
        .uri = "/metrics",
        .method = HTTP_GET,
        .handler = webserver_handler_get_metrics,
        .user_ctx = NULL
    };
    ESP_LOGD(TAG_, "Registering GET /metrics");
    err = httpd_register_uri_handler(server_, &metrics);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_, "Failed to register handler for GET /metrics (%s)", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

void webserver_stop(httpd_handle_t server) {
    // Stop the httpd server
    httpd_stop(server);
}

static httpd_handle_t server = NULL;

static void disconnect_handler(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data) {
    httpd_handle_t* server = (httpd_handle_t*)arg;
    if (*server) {
        ESP_LOGI(TAG_, "Stopping webserver");
        webserver_stop(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data) {
    httpd_handle_t* server = (httpd_handle_t*)arg;
    if (*server == NULL) {
        ESP_LOGI(TAG_, "Starting webserver");
        webserver_start(port_, sensor_);
    }
}

esp_err_t webserver_start(uint16_t port, AHT10* sensor) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = port;

    port_ = port;
    sensor_ = sensor;

    webserver_util_set_sensor(sensor);

    ESP_LOGI(TAG_, "Starting server on port %d", config.server_port);
    esp_err_t err = httpd_start(&server_, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_, "Failed to start server (%s)", esp_err_to_name(err));
        return err;
    }

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server_));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server_));

    err = webserver_register_handlers_();
    if (err != ESP_OK) {
        ESP_LOGE(TAG_, "Failed to register URI handlers (%s)", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}
