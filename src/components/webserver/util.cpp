// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#include "util.hpp"

#include "string.h"

#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "sys/socket.h"

#include "sensor/aht10.hpp"

AHT10* sensor_ = NULL;
static const char TAG_[] = "webserver_util";


esp_err_t webserver_util_get_client_ip(httpd_req_t* req, char ip[INET6_ADDRSTRLEN]) {
    int sock = httpd_req_to_sockfd(req);
    struct sockaddr_in6 addr;
    socklen_t addr_size = sizeof(addr);

    if (getpeername(sock, (struct sockaddr*)&addr, &addr_size) < 0) {
        ESP_LOGW(TAG_, "Could not get IP address of client");
        return ESP_FAIL;
    }
    else {
        inet_ntop(AF_INET6, &addr.sin6_addr, ip, INET6_ADDRSTRLEN);
    }
    return ESP_OK;
}

esp_err_t webserver_util_get_measurement(aht10_measurement_t* measurement) {
    return sensor_->Measure(measurement);
}

char* webserver_util_format_metrics(aht10_measurement_t* measurement) {
    const char metrics_template[] =
        "# HELP environment_temperature_celsius Current temperature\n"
        "# TYPE environment_temperature_celsius gauge\n"
        "environment_temperature_celsius %f\n"
        "# HELP environment_humidity_percent Current humidity\n"
        "# TYPE environment_humidity_percent gauge\n"
        "environment_humidity_percent %f\n"
        "# HELP device_uptime_seconds Uptime of device in seconds\n"
        "# TYPE device_uptime_seconds counter\n"
        "device_uptime_seconds %.0f\n"
        "# HELP device_free_heap_bytes Number of bytes free on heap\n"
        "# TYPE device_free_heap_bytes gauge\n"
        "device_free_heap_bytes %d\n";

    const double uptime = (double)esp_timer_get_time() / 1000000;
    const uint32_t heap = esp_get_free_heap_size();

    char* buf;
    size_t len = snprintf(
        NULL,
        0,
        metrics_template,
        measurement->temperature,
        measurement->humidity,
        uptime,
        heap
    );
    buf = (char*)malloc(len);
    sprintf(
        buf,
        metrics_template,
        measurement->temperature,
        measurement->humidity,
        uptime,
        heap
    );
    return buf;
}

void webserver_util_set_sensor(AHT10* sensor) {
    sensor_ = sensor;
}
