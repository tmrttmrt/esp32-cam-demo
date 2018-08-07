/* Example

   For other examples please check:
   https://github.com/espressif/esp-iot-solution/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <stdio.h>

// void led_on()
// {
//     gpio_set_direction(14, GPIO_MODE_OUTPUT);
//     gpio_set_level(14, 0);
// }

// void led_off()
// {
//     gpio_set_direction(14, GPIO_MODE_INPUT);
//     gpio_set_pull_mode(14, GPIO_FLOATING);
// }


void reset_onoff()
{
    // Initialize NVS
    esp_err_t err;
    // esp_err_t err = nvs_flash_init();
    // if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
    //     // NVS partition was truncated and needs to be erased
    //     // Retry nvs_flash_init
    //     nvs_flash_erase();
    //     err = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK( err );

    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        // printf("Done\n");

        // Read
        // printf("Reading restart counter from NVS ... ");
        uint8_t reset_cnt = 0; // value will default to 0, if not set yet in NVS
        err = nvs_get_u8(my_handle, "reset_cnt", &reset_cnt);
        // switch (err) {
        //     case ESP_OK:
        //         // printf("Done\n");
        //         printf("Restart counter = %d\n", reset_cnt);
        //         break;
        //     case ESP_ERR_NVS_NOT_FOUND:
        //         printf("The value is not initialized yet!\n");
        //         break;
        //     default :
        //         printf("Error (%s) reading!\n", esp_err_to_name(err));
        // }

        if (reset_cnt > 0) {
            // led_off();
            nvs_set_u8(my_handle, "reset_cnt", 0);
            err = nvs_commit(my_handle);
            // printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

            // Close
            nvs_close(my_handle);

            // esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
            printf("Sleep!\n");
            vTaskDelay(100 / portTICK_PERIOD_MS);
            esp_deep_sleep_start();

        } else {
            // Write
            // printf("Updating restart counter in NVS ... ");
            reset_cnt++;
            err = nvs_set_u8(my_handle, "reset_cnt", reset_cnt);
            // printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

            // Commit written value.
            // After setting any values, nvs_commit() must be called to ensure changes are written
            // to flash storage. Implementations may write to storage at other times,
            // but this is not guaranteed.
            // printf("Committing updates in NVS ... ");
            err = nvs_commit(my_handle);
            // printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

            // Wait
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            nvs_set_u8(my_handle, "reset_cnt", 0);
            err = nvs_commit(my_handle);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

            // Close
            nvs_close(my_handle);
        }
    }
}
