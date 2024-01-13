// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#include "aht10.hpp"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/i2c.h"
#include "driver/gpio.h"

esp_err_t AHT10::Read(uint8_t *data, size_t len)
{
    ESP_LOGD(TAG_, "Reading from AHT10 at address %x", addr_);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr_ << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
    i2c_master_read(cmd, data, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    int ret = i2c_master_cmd_begin(port_, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    ESP_LOGD(TAG_, "Finished reading data. Got response %s", esp_err_to_name(ret));
    CheckResponseCode(ret);
    return ret;
}

esp_err_t AHT10::Write(uint8_t *data, size_t len)
{
    ESP_LOGD(TAG_, "Writing to AHT10 at address %x", addr_);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr_ << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write(cmd, data, len, ACK_CHECK_EN);
    i2c_master_stop(cmd);

    int ret = i2c_master_cmd_begin(port_, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    ESP_LOGD(TAG_, "Finished writing. Response code was %s", esp_err_to_name(ret));
    CheckResponseCode(ret);
    return ret;
}

void AHT10::CheckResponseCode(esp_err_t code)
{
    if (code != ESP_OK)
    {
        ESP_LOGW(TAG_, "Response code was not ESP_OK, got %s", esp_err_to_name(code));
    }
}

esp_err_t AHT10::Init()
{
    // 20ms to allow power up
    vTaskDelay(20 / portTICK_PERIOD_MS);

    ESP_LOGD(TAG_, "Soft resetting sensor");
    uint8_t cmd[3];
    cmd[0] = AHT10_CMD_SOFTRESET;
    ESP_ERROR_CHECK(Write(cmd, 1))

    vTaskDelay(20 / portTICK_PERIOD_MS);
    while (GetStatus() & AHT10_STATUS_BUSY)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    ESP_LOGD(TAG_, "Calibrating sensor");
    cmd[0] = AHT10_CMD_CALIBRATE;
    cmd[1] = 0x08;
    cmd[2] = 0x00;
    ESP_ERROR_CHECK(Write(cmd, 3));

    if (!(GetStatus() & AHT10_STATUS_CALIBRATED))
    {
        ESP_LOGE(TAG_, "Failed to calibrate sensor");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG_, "Finished configuring sensor");
    return ESP_OK;
}

uint8_t AHT10::GetStatus()
{
    uint8_t ret;
    ESP_ERROR_CHECK(Read(&ret, 1));
    return ret;
}

AHT10::AHT10(gpio_num_t scl, gpio_num_t sda, i2c_port_t port, uint8_t addr)
{
    port_ = port;
    addr_ = addr;

    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = scl;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.clk_stretch_tick = 300;

    ESP_ERROR_CHECK(i2c_driver_install(port_, conf.mode));
    ESP_ERROR_CHECK(i2c_param_config(port_, &conf));

    ESP_ERROR_CHECK(Init());

    ESP_LOGD(TAG_, "Setup I2C for AHT10. SCL: %d SDA: %d", scl, sda);
}

esp_err_t AHT10::Measure(aht10_measurement_t *result)
{
    ESP_LOGD(TAG_, "Triggering read");
    uint8_t cmd[3] = {AHT10_CMD_TRIGGER, 0x33, 0x00};
    ESP_ERROR_CHECK(Write(cmd, 3));

    // Wait for the result to be ready
    while (GetStatus() & AHT10_STATUS_BUSY)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    ESP_LOGD(TAG_, "Data ready, reading...");

    // Read our data from the sensor
    uint8_t data[6];
    ESP_ERROR_CHECK(Read(data, 6));

    uint32_t h_data = data[1];
    h_data <<= 8;
    h_data |= data[2];
    h_data <<= 4;
    h_data |= data[3] >> 4;
    result->humidity = ((float)h_data * 100) / 0x100000;

    uint32_t t_data = data[3] & 0x0F;
    t_data <<= 8;
    t_data |= data[4];
    t_data <<= 8;
    t_data |= data[5];
    result->temperature = ((float)t_data * 200 / 0x100000) - 50;

    ESP_LOGI(TAG_, "Read data from sensor. Humidity: %f Temperature: %f", result->humidity, result->temperature);
    return ESP_OK;
}
