/**************************************************************
 *
 *                           main.c
 *
 *     Authors:  Nahom Tsegaye
 *     Date:     5/24/25
 *
 *     The main file for implementing the motion sensor. The
 *     motion sensor can detect someone entering a room and
 *     send a command through the NEC protocol. When someone 
 *     leaves a different command will be sent through the
 *     NEC protocol.
 *
 **************************************************************/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "NEC.h"
#include "configs_sensor.h"

static const char *TAG = "main";

#define GPIO_OUTPUT_PIN_SEL  (1ULL<<GPIO_OUTPUT_0)
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_INPUT_0)
#define ESP_INTR_FLAG_DEFAULT 0

void configureGPIO();

void app_main(void)
{
    configureGPIO();
    NEC_handler_t handler = NEC_read_enable(GPIO_INPUT_0);

    while (1) {
        ESP_LOGI(TAG, "Don't worry about me...");

        if (NEC_read_finished(handler)) {
            ESP_LOGI(TAG, "Reading finished!\nAddress: %x\nCommand: %x\n",
get_NEC_address(handler), get_NEC_command(handler));
            NEC_read_reset(handler);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    NEC_read_disable(handler);
}

void configureGPIO()
{
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

