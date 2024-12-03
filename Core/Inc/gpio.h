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
void LED_toggle(GPIO_Typedef * port, uint16_t pin);
void LED_on(GPIO_Typedef * port, uint16_t pin);
void LED_off(GPIO_Typedef * port, uint16_t pin);

#endif /* GPIO_H_ */
