/**************************************************************
 *
 *                           NEC.h
 *
 *     Authors:  Nahom Tsegaye
 *     Date:     5/24/25
 *
 *     Interface for reading in the NEC protocol through a GPIO
 *     pin. The pin must be connected to a IR reciever that
 *     a digital signal rather than the raw 38kHZ input.
 *
 **************************************************************/
#pragma once
#include "driver/gpio.h"
#include <stdbool.h>

typedef struct NEC_handler_t *NEC_handler_t;

/* * * * * * * * * * * * * * NEC_read_enable  * * * * * * * * * * * * *
 *
 * Configures an NEC read on a given pin and begins reading
 *
 * Parameters:
 *      gpio_num:  the pin in which the IR reciever is connected to
 *
 * Return: nothing
 *
 * Expects
 *      A valid GPIO pin
 *
 * Notes:
 *      Dynamically allocates memory that must be freed using NEC_read_diable
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
extern NEC_handler_t NEC_read_enable(gpio_num_t gpio_num);

/* * * * * * * * * * * * * * NEC_read_diable  * * * * * * * * * * * * *
 *
 * Disables a NEC read that was previously created using NEC_read_enable
 *
 * Parameters:
 *      NEC_handler:  the handler created by NEC_read_enable
 *
 * Return: nothing
 *
 * Expects
 *      A non-null NEC_handler
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
extern void NEC_read_disable(NEC_handler_t NEC_handler);

/* * * * * * * * * * * * * NEC_read_finished  * * * * * * * * * * * * *
 *
 * Checks if the current read is finished or not. If there is no current
 * read the function will return false.
 *
 * Parameters:
 *      NEC_handler:  the handler created by NEC_read_enable
 *
 * Return: If the read is finished or not
 *
 * Expects
 *      A non-null NEC_handler
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
extern bool NEC_read_finished(NEC_handler_t NEC_handler);

/* * * * * * * * * * * * * get_NEC_address * * * * * * * * * * * * *
 *
 * Gets the address from the NEC data that was read.
 *
 * Parameters:
 *      NEC_handler:  the handler created by NEC_read_enable
 *
 * Return: the address that was read
 *
 * Expects
 *      A non-null NEC_handler
 *      NEC_read_finished to return true
 *
 * Notes:
 *      Will set error boolean if the read isn't finished
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
extern uint32_t get_NEC_address(NEC_handler_t NEC_handler);

/* * * * * * * * * * * * * get_NEC_command * * * * * * * * * * * * *
 *
 * Gets the command from the NEC data that was read.
 *
 * Parameters:
 *      NEC_handler:  the handler created by NEC_read_enable
 *
 * Return: the command that was read
 *
 * Expects
 *      A non-null NEC_handler
 *      NEC_read_finished to return true
 *
 * Notes:
 *      Will set error boolean if the read isn't finished
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
extern uint32_t get_NEC_command(NEC_handler_t NEC_handler);

/* * * * * * * * * * * * * NEC_is_error * * * * * * * * * * * * *
 *
 * Checks if any error were encountered during the read.
 *
 * Parameters:
 *      NEC_handler:  the handler created by NEC_read_enable
 *
 * Return: the error boolean
 *
 * Expects
 *      A non-null NEC_handler
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
extern bool NEC_is_error(NEC_handler_t NEC_handler);

/* * * * * * * * * * * * * NEC_read_reset * * * * * * * * * * * * *
 *
 * Resets the current read.
 *
 * Parameters:
 *      NEC_handler:  the handler created by NEC_read_enable
 *
 * Return: Nothing
 *
 * Expects
 *      A non-null NEC_handler
 *
 * Notes:
 *      Will reset error boolean
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
extern void NEC_read_reset(NEC_handler_t NEC_handler);

extern void configure_NEC_output();
extern void free_NEC_output();


