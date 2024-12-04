/*
 * gpio.c
 *
 *  Created on: Nov 12, 2024
 *      Author: jacob
 */

#include "gpio.h"

// define all LEDs for clearAll function
const ledPins PINS_NS =
{
	.WALK = GPIO_PIN_6,
	.WAIT = GPIO_PIN_7,
	.TURNING = GPIO_PIN_8,
	.GREEN = GPIO_PIN_9,
	.YELLOW = GPIO_PIN_10,
	.RED = GPIO_PIN_1
};

const ledPins PINS_EW =
{
	.WALK = GPIO_PIN_6,
	.WAIT = GPIO_PIN_4,
	.TURNING = GPIO_PIN_8,
	.GREEN = GPIO_PIN_9,
	.YELLOW = GPIO_PIN_10,
	.RED = GPIO_PIN_0
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
	ledOff(GPIOA, PINS_NS.WALK);
	ledOff(GPIOA, PINS_NS.WAIT);
	ledOff(GPIOA, PINS_NS.TURNING);
	ledOff(GPIOA, PINS_NS.GREEN);
	ledOff(GPIOA, PINS_NS.YELLOW);
	ledOff(GPIOA, PINS_NS.RED);

	ledOff(GPIOB, PINS_EW.WALK);
	ledOff(GPIOB, PINS_EW.WAIT);
	ledOff(GPIOB, PINS_EW.TURNING);
	ledOff(GPIOB, PINS_EW.GREEN);
	ledOff(GPIOB, PINS_EW.YELLOW);
	ledOff(GPIOB, PINS_EW.RED);
}
