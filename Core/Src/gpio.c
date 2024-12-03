/*
 * gpio.c
 *
 *  Created on: Nov 12, 2024
 *      Author: jacob
 */

#include "gpio.h"

/*
 * LED control functions
 */
void LED_toggle(GPIO_Typedef * port, uint16_t pin)
{
	HAL_GPIO_TogglePin(port, pin); // toggle light
	return;
}

void LED_off(GPIO_Typedef * port, uint16_t pin)
{
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
	return;
}

void LED_on(GPIO_Typedef * port, uint16_t pin)
{
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
	return;
}
