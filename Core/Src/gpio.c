/*
 * gpio.c
 *
 *  Created on: Nov 12, 2024
 *      Author: jacob
 */

#include "gpio.h"

// define all LEDs for clearAll function
static const ledPinNS PINS_NS[] =
{
	LED_WALK,
	LED_WAIT,
	LED_TURNING,
	LED_GREEN,
	LED_YELLOW,
	LED_RED,
};

/*
 * LED control functions
 */
void ledToggle(GPIO_TypeDef * port, uint16_t pin)
{
	HAL_GPIO_TogglePin(port, pin); // toggle light
	return;
}

void ledOff(GPIO_TypeDef * port, uint16_t pin)
{
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
	return;
}

void ledOn(GPIO_TypeDef * port, uint16_t pin)
{
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
	return;
}

void clearAll()
{
	for (uint16_t pin = 0; pin < sizeof(PINS_NS) / sizeof(PINS_NS[0]); pin++)
	{
		ledOff(GPIOA, PINS_NS[pin]);
	}
}
