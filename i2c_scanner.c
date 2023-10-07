//I2C scanner code for esp idf , works for all boards including esp32h2

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO 8       // I2C clock pin
#define I2C_MASTER_SDA_IO 9        // I2C data pin
#define I2C_MASTER_NUM I2C_NUM_0    // I2C port number
#define I2C_MASTER_FREQ_HZ 100000   // I2C master clock frequency

void i2c_scanner() {
    int32_t scan_result;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(I2C_MASTER_NUM, &conf);

    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);

    printf("Scanning I2C bus...\n");
    printf("Found devices:\n");

    for (uint8_t i = 1; i < 128; i++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);

        scan_result = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);

        if (scan_result == ESP_OK) {
            printf("- Device found at address 0x%02X\n", i);
        }

        i2c_cmd_link_delete(cmd);
    }

    i2c_driver_delete(I2C_MASTER_NUM);
    vTaskDelete(NULL);
}

void app_main() {
    xTaskCreate(i2c_scanner, "i2c_scanner", 2048, NULL, 0, NULL);
}
