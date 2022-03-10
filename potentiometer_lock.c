
#include "potentiometer_lock.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

static const char *TAG = "POTENTIOMETER_LOCK";

// code and input_code are not null terminated
#define MAX_CODE_LENGTH 24
// code and input_code are not null terminated
static code_char code[MAX_CODE_LENGTH+1];
static size_t code_length;
static size_t current_code_index = 1; // First code_char always set

static void (*on_success_callback) (void) = NULL;
static void (*on_failure_callback) (void) = NULL;


// Setters
int potentiometer_lock_set_code(code_char *code_p, size_t code_length_p)
{
    // Generating code
    code_length = code_length_p+1;
    if (code_length > MAX_CODE_LENGTH)
        return -1;

    code[0] = 0; // First code_char is always set
    memcpy(code+1, code_p, code_length * sizeof(code_char));
    return 0;
}
void potentiometer_lock_set_on_success_callback(void (*callback) (void))
{
    on_success_callback = callback;
}
void potentiometer_lock_set_on_failure_callback(void (*callback) (void))
{
    on_failure_callback = callback;
}


// BEGIN adc inputs

#define ADC_DEFAULT_VREF    1100    // Use adc2_vref_to_gpio() to obtain a better estimate
#define ADC_SAMPLES_NB      64      // Multisampling

static void init_adc_input(adc_channel_t channel)
{
    // Configure ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, ADC_ATTEN_DB_11);

    // Characterize ADC
    esp_adc_cal_characteristics_t *adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, ADC_DEFAULT_VREF, adc_chars);
}

uint32_t get_adc_reading(adc_channel_t channel) {
    uint32_t adc_reading = 0;
    //Multisampling
    for (int i = 0; i < ADC_SAMPLES_NB; i++)
        adc_reading += adc1_get_raw((adc1_channel_t)channel);

    return adc_reading / ADC_SAMPLES_NB;
}


// END adc inputs



// Task
static void potentiometer_lock_task(void* arg)
{
    ESP_LOGI(TAG, "Ready to take inputs");

    while(1) {
        code_char value = (get_adc_reading(CONFIG_POTENTIOMETER_ADC_CHANNEL) * CONFIG_POSITIONS_NB) / 4096;
        ESP_LOGI(TAG, "Current code value: %u", value);
        if ((current_code_index%2 == 1 && value < code[current_code_index-1])
                || (current_code_index%2 == 0 && value > code[current_code_index-1])) {
            // The user moved the cursor too far to the left
            current_code_index = 1;
            // Giving signs of failure in this callback would allow anyone to crack the code easily
            // (every failure means the right code is just before so finding the right code would only take code_length tries)
            on_failure_callback();
        }
        else if (value == code[current_code_index])
            current_code_index++;

        if (current_code_index == code_length) {
            current_code_index = 1;
            on_success_callback();
        }

        vTaskDelay(pdMS_TO_TICKS(CONFIG_READ_VALUE_DELTA));
    }
}

int init_potentiometer_lock(code_char *code, size_t code_length)
{
    if (potentiometer_lock_set_code(code, code_length) < 0)
        return -1;

    init_adc_input(CONFIG_POTENTIOMETER_ADC_CHANNEL);

    xTaskCreate(potentiometer_lock_task, "potentiometer_lock_task", 2048, NULL, 10, NULL);

    return 0;
}

