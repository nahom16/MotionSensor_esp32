/**************************************************************
 *
 *                           NEC.c
 *
 *     Authors:  Nahom Tsegaye
 *     Date:     5/24/25
 *
 *     Implementation of the NEC.h file
 *
 **************************************************************/

#include "NEC.h"
#include "esp_log.h"
#include "esp_timer.h"

struct NEC_handler_t {
    gpio_num_t gpio_num;
    uint32_t word_read;
    int instruct_num;
    uint64_t last_time;
    bool error;
};

static void NEC_interrupt(void* handler)
{
    NEC_handler_t instance = handler;

    if (instance->instruct_num >= 0 && instance->instruct_num < 32) {
        uint64_t time_since = esp_timer_get_time() - instance->last_time;

        /* If the gap between Pulses is greater than 1.6ms than it's a logical 1 */
        if (time_since > 1600) {
            instance->word_read += 1;
        }
        instance->word_read <<= 1;

        instance->last_time = esp_timer_get_time(); 
        instance->instruct_num++;
    } else if (instance->instruct_num == -1) {
        uint64_t time_since = esp_timer_get_time() - instance->last_time;

        /* 
         * If the total time of the on/off pulse is close to 13.5ms then move on
         * to reading commands. Otherwise this is not the NEC protocol
         */
        if (time_since < 13000|| time_since > 14000) {
            instance->error = true;
            instance->instruct_num = -2;
        } else {
            instance->instruct_num++;
        }

        instance->last_time = esp_timer_get_time(); 
    } else if (instance->instruct_num == -2) {
        instance->last_time = esp_timer_get_time(); 
	    instance->instruct_num++;
    } 
}

NEC_handler_t NEC_read_enable(gpio_num_t gpio_num)
{
    /* Configure GPIO */
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = 1ULL << ((uint64_t) gpio_num);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    /* Create a dynamically allocated struct */
    NEC_handler_t new_handler = malloc(sizeof(*new_handler));
    new_handler->word_read = 0;
    new_handler->instruct_num = -2;
    new_handler->gpio_num = gpio_num;
    new_handler->last_time = 0;
    new_handler->error = false;

    /* Set up interrupt handler */
    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio_num, NEC_interrupt, (void*) new_handler);
    gpio_intr_enable(gpio_num);

    return new_handler;
}

void NEC_read_disable(NEC_handler_t NEC_handler)
{
    if (NEC_handler == NULL) {
        return;
    }
    gpio_reset_pin(NEC_handler->gpio_num);
    free(NEC_handler);
}

bool NEC_read_finished(NEC_handler_t NEC_handler)
{
    if (NEC_handler == NULL) {
        return false;
    }

    if (NEC_handler->instruct_num == 32) {
        return true;
    } else {
        return false;
    }
}


uint32_t get_NEC_address(NEC_handler_t NEC_handler)
{
    if (NEC_handler == NULL) {
        return 0;
    }

    if (!NEC_read_finished(NEC_handler)) {
        NEC_handler->error = true;
        return 0;
    }

    uint32_t address = NEC_handler->word_read >> 16;

    /* Checks if the address is the extended version or the original version */
    if ((address & 0xFF) == ~(address >> 8)) {
        return address & 0xFF;
    } else {
        return address;
    }
    
}

uint32_t get_NEC_command(NEC_handler_t NEC_handler)
{
    if (NEC_handler == NULL) {
        return 0;
    }

    if(!NEC_read_finished(NEC_handler)) {
        NEC_handler->error = true;
        return 0;
    }

    uint32_t command = NEC_handler->word_read & 0xFFFF;

    /* Checks if the command was sent through properly */
    if((command & 0xFF) != ~(command >> 8)) {
        NEC_handler->error = true;
    }
    
    return command & 0xFF;
}

void NEC_read_reset(NEC_handler_t NEC_handler)
{
    if (NEC_handler == NULL) {
        return;
    }

    NEC_handler->instruct_num = -2;
    NEC_handler->error = false;
    NEC_handler->word_read = 0;
    NEC_handler->last_time = 0;
}

bool NEC_is_error(NEC_handler_t NEC_handler)
{
    if (NEC_handler == NULL) {
        return true;
    }

    return NEC_handler->error;
}

