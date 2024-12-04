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
typedef struct
{
	uint16_t WALK;
	uint16_t WAIT;
	uint16_t TURNING;
	uint16_t GREEN;
	uint16_t YELLOW;
	uint16_t RED;
} ledPins;

// north/south uses GPIOA
extern const ledPins PINS_NS;

// east/west uses GPIOB
extern const ledPins PINS_EW;
/*
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
typedef enum
{
	LED_WALK = GPIO_PIN_6,
	LED_WAIT = GPIO_PIN_4,
	LED_TURNING = GPIO_PIN_8,
	LED_GREEN = GPIO_PIN_9,
	LED_YELLOW = GPIO_PIN_10,
	LED_RED = GPIO_PIN_13
} ledPinEW;
*/
// LED control functions
void ledToggle(GPIO_TypeDef * port, uint16_t pin);
void ledOn(GPIO_TypeDef * port, uint16_t pin);
void ledOff(GPIO_TypeDef * port, uint16_t pin);

// disable all outputs
void clearAll();

#endif /* GPIO_H_ */
