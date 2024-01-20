// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#ifndef SENSOR_AHT10_H_
#define SENSOR_AHT10_H_

#include "driver/i2c.h"
#include "driver/gpio.h"

#define ACK_CHECK_EN 0x1  // Check ack from sensor
#define ACK_CHECK_DIS 0x0 // Don't check ack from sensor

#define AHT10_STATUS_BUSY 0x80
#define AHT10_STATUS_CALIBRATED 0x08

typedef enum {
    AHT10_CMD_CALIBRATE = 0xE1,
    AHT10_CMD_TRIGGER = 0xAC,
    AHT10_CMD_SOFTRESET = 0xBA,
} aht10_command_t;

struct aht10_measurement_t {
    float temperature;
    float humidity;
};

class AHT10 {
private:
    const char TAG_[6] = "AHT10";

    i2c_port_t port_;
    uint8_t addr_;

    /**
     * @brief Read data from sensor
     *
     * @param reg_addr Address to read
     * @param data Data to read
     * @param len Length of data to read
     * @return esp_err_t
     */
    esp_err_t Read(uint8_t* data, size_t len);

    /**
     * @brief Write data to AHT10
     *
     * @param reg_addr Address to write
     * @param data Data to sent to AHT10
     * @param len Length of data being sent
     * @return esp_err_t
     */
    esp_err_t Write(uint8_t* data, size_t len);

    /**
     * @brief Check response code is ESP_OK. If not log warning
     *
     * @param code Code to check
     */
    void CheckResponseCode(esp_err_t code);

    /**
     * @brief Initialise sensor
     */
    esp_err_t Init();

    /**
     * @brief Get the current status of the sensor
     *
     * @return uint8_t 8 bits of status data
     */
    uint8_t GetStatus();

public:
    /**
     * @brief Initialize sensor
     *
     * @param scl I2C SCL pin
     * @param sda I2C SDA pin
     * @param port I2C port to use
     * @param addr Address of AHT10
     */
    AHT10(gpio_num_t scl, gpio_num_t sda, i2c_port_t port, uint8_t addr);

    /**
     * @brief Get the current measurement from the sensor
     *
     * @param result Struct to store result in
     * @return esp_err_t
     */
    esp_err_t Measure(aht10_measurement_t* result);
};

#endif // SENSOR_AHT10_H_
