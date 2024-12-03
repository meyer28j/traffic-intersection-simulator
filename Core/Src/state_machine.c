/*
 * state_machine.c
 *
 *  Created on: Nov 24, 2024
 *      Author: jacob
 */

#include "state_machine.h"
#include "gpio.h" // LED on/off and clearAll()

// state machine instance
TrafficLightStateMachine sm = {RED_WAIT, RED_WAIT, NS};

// externally available state periods
uint16_t state_periods[]= {
		2000,	// TURNING_WAIT
		2000,	// GREEN_WALK
		2000,	// GREEN_WAIT_FLASH
		2000,	// GREEN_WAIT
		2000,	// YELLOW_WAIT
		2000,	// RED_WAIT
		0,		// EMERGENCY, period does not apply
		1000	// NO_POWER
};

void ChangeState(State next_state)
{
	// turn off all LEDs regardless of state
	clearAll();

	// get relevant state machine directional state
	State* direction_state = sm.direction == NS ? &sm.ns_state : &sm.ew_state;

	/*
	 * update GPIO target based on direction and define other direction
	 * to be able to turn on non-current direction LED_RED and LED_WAIT
	 */
	GPIO_TypeDef * GPIO_current = sm.direction == NS ? GPIOA : GPIOB;
	GPIO_TypeDef * GPIO_other = sm.direction == NS ? GPIOB : GPIOA;

	// change to the next state
	if (xSemaphoreTake(stateMachineHandle, (TickType_t) 100) == pdTRUE)
	{
		*direction_state = next_state;
		xSemaphoreGive(stateMachineHandle);
	}


	/*
	 * handle:
	 * 1. new state needs to enable/disable flashing lights
	 * 2. new state changes direction (i.e. enter RED_WAIT)
	 */
	switch (next_state)
	{
		case TURNING_WAIT:
			ledOn(GPIO_current, LED_TURNING);
			ledOn(GPIO_current, LED_WAIT);
			// TODO: unblock blinking task for TURNING signal
			break;
		case GREEN_WALK:
			ledOn(GPIO_current, LED_GREEN);
			ledOn(GPIO_current, LED_WALK);
			// TODO: block blinking task
			break;
		case GREEN_WAIT_FLASH:
			ledOn(GPIO_current, LED_GREEN);
			ledOn(GPIO_current, LED_WAIT);
			// TODO: unblock blinking task for pedestrian WAIT
			break;
		case GREEN_WAIT:
			ledOn(GPIO_current, LED_GREEN);
			ledOn(GPIO_current, LED_WAIT);
			// TODO: block blinking task
			break;
		case YELLOW_WAIT:
			ledOn(GPIO_current, LED_YELLOW);
			ledOn(GPIO_current, LED_WAIT);

			/*
			 * not needed under normal operation, as blinking was
			 * blocked by prior state, but included anyways for
			 * robustness in case GREEN_WAIT was skipped
			 */
			// TODO: block blinking task (arguably redundant)
			break;
		case RED_WAIT:
			ledOn(GPIO_current, LED_RED);
			ledOn(GPIO_current, LED_WAIT);

			// change direction
			ChangeDirection();
			break;
		case EMERGENCY:
			// TODO: block state changing until emergency lifted
			break;
		case NO_POWER:
			// TODO: block state changing until power restored
			// TODO: unblock blinking task
			break;
		default:
			// should never reach this point
			Error_Handler();
	}
	/*
	 * SPECIAL CASE: ensure opposite direction LED_RED
	 * and LED_WAIT are always on
	 */
	ledOn(GPIO_other, LED_RED);
	ledOn(GPIO_other, LED_WAIT);
}

void ChangeDirection()
{
	// TODO: create while-loop that attempts to periodically retake the mutex on failure
	// toggle direction between NS and EW
	if (xSemaphoreTake(stateMachineHandle, (TickType_t) 100) == pdTRUE)
	{
		sm.direction = sm.direction == NS ? EW : NS;
		xSemaphoreGive(stateMachineHandle);
	}
}

const char* StateToString(State state)
{
	switch (state)
	{
	case TURNING_WAIT: 		return "TURNING_WAIT";
	case GREEN_WALK:		return "GREEN_WALK";
	case GREEN_WAIT_FLASH:	return "GREEN_WAIT_FLASH";
	case GREEN_WAIT:		return "GREEN_WAIT";
	case YELLOW_WAIT:		return "YELLOW_WAIT";
	case RED_WAIT:			return "RED_WAIT";
	case EMERGENCY:			return "EMERGENCY";
	case NO_POWER:			return "NO_POWER";
	default:				return "ERROR";
	}
}

const uint16_t StringToState(const char* state_name)
{
	if (strcmp(state_name, "TURNING_WAIT") == 0)
	{
		return TURNING_WAIT;
	}
	else if (strcmp(state_name, "GREEN_WALK") == 0)
	{
		return GREEN_WALK;
	}
	else if (strcmp(state_name, "GREEN_WAIT_FLASH") == 0)
	{
		return GREEN_WAIT_FLASH;
	}
	else if (strcmp(state_name, "GREEN_WAIT") == 0)
	{
		return GREEN_WAIT;
	}
	else if (strcmp(state_name, "YELLOW_WAIT") == 0)
	{
		return YELLOW_WAIT;
	}
	else if (strcmp(state_name, "RED_WAIT") == 0)
	{
		return RED_WAIT;
	}
	else if (strcmp(state_name, "EMERGENCY") == 0)
	{
		return EMERGENCY;
	}
	else if (strcmp(state_name, "NO_POWER") == 0)
	{
		return NO_POWER;
	}
	else
	{
		return -1;  // ERROR flag
	}
}
