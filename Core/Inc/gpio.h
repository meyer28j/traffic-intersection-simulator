/*
 * gpio.h
 *
 *  Created on: Nov 12, 2024
 *      Author: jacob
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f1xx_hal.h"

// LED control functions
void LED_toggle();
void LED_on();
void LED_off();

#endif /* GPIO_H_ */
