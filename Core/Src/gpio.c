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
void LED_toggle()
{
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // toggle light
	return;
}

void LED_off()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	return;
}

void LED_on()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	return;
}
