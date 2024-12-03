/*
 * gpio.h
 *
 *  Created on: Nov 12, 2024
 *      Author: jacob
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f1xx_hal.h"

/*
 * Values taken from wiring schematic, align pin
 * with connected traffic light purpose
 */
// north/south uses GPIOA
typedef enum
{
	LED_WALK = GPIO_PIN_6,
	LED_WAIT = GPIO_PIN_7,
	LED_TURNING = GPIO_PIN_8,
	LED_GREEN = GPIO_PIN_9,
	LED_YELLOW = GPIO_PIN_10,
	LED_RED = GPIO_PIN_1
} ledPinNS;

// east/west uses GPIOB
/*
typedef enum
{
	LED_WALK = GPIO_PIN_,
	LED_WAIT = GPIO_PIN_,
	LED_TURNING = GPIO_PIN_,
	LED_GREEN = GPIO_PIN_,
	LED_YELLOW = GPIO_PIN_,
	LED_RED = GPIO_PIN_
} ledPinEW;
*/

/*
const ledPinEW pinsEW[] =
{
		LED_WALK,
		LED_WAIT,
		LED_TURNING,
		LED_GREEN,
		LED_YELLOW,
		LED_RED,
};
*/
// LED control functions
void ledToggle(GPIO_TypeDef * port, uint16_t pin);
void ledOn(GPIO_TypeDef * port, uint16_t pin);
void ledOff(GPIO_TypeDef * port, uint16_t pin);

// disable all outputs
void clearAll();

#endif /* GPIO_H_ */
