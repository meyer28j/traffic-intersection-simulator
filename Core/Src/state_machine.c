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
		3000,	// TURNING_WAIT
		3000,	// GREEN_WALK
		2000,	// GREEN_WAIT_FLASH
		2000,	// GREEN_WAIT
		1000,	// YELLOW_WAIT
		1000,	// RED_WAIT
		0,		// EMERGENCY, period does not apply
		1000	// NO_POWER
};

State current_state = TURNING_WAIT;
Direction current_direction = NS;

State * direction_state = NULL;
GPIO_TypeDef * GPIO_current = NULL;
GPIO_TypeDef * GPIO_other = NULL;
const ledPins * pins = NULL;
const ledPins * pins_other = NULL;

void ChangeState(State next_state)
{
	// turn off all LEDs regardless of state
	clearAll();

	// update state-direction based values - used in switch-case
	// to allow generic functions
	if (sm.direction == NS)
	{
		direction_state = &sm.ns_state;
		GPIO_current = GPIOA;
		GPIO_other = GPIOB;
		pins = &PINS_NS;
		pins_other = &PINS_EW;
	}
	else
	{
		direction_state = &sm.ew_state;
		GPIO_current = GPIOB;
		GPIO_other = GPIOA;
		pins = &PINS_EW;
		pins_other = &PINS_NS;
	}

	// change to the next state
	if (xSemaphoreTake(stateMachineHandle, (TickType_t) 100) == pdTRUE)
	{
		*direction_state = next_state;
		xSemaphoreGive(stateMachineHandle);
	}

	//current_state = next_state;

	/*
	 * handle:
	 * 1. new state needs to enable/disable flashing lights
	 * 2. new state changes direction (i.e. enter RED_WAIT)
	 */
	switch (next_state)
	{
		case TURNING_WAIT:
			ledOn(GPIO_current, pins->TURNING);
			ledOn(GPIO_current, pins->WAIT);
			// TODO: unblock blinking task for TURNING signal
			current_state = TURNING_WAIT;
			break;
		case GREEN_WALK:
			// TODO: block blinking task
			if ((sm.direction == NS && sm.ns_crosswalk)
				|| (sm.direction == EW && sm.ew_crosswalk))
			{
				ledOn(GPIO_current, pins->GREEN);
				ledOn(GPIO_current, pins->WALK);
				current_state = GREEN_WALK;
				break;
			}
			// INTENTIONALLY do not break to instead flow into
			// GREEN_WAIT state in the case that no pedestrian
			// has hit the crosswalk button
		case GREEN_WAIT_FLASH:
			// TODO: unblock blinking task for pedestrian WAIT
			if ((sm.direction == NS && sm.ns_crosswalk)
				|| (sm.direction == EW && sm.ew_crosswalk))
			{
				ledOn(GPIO_current, pins->GREEN);
				ledOn(GPIO_current, pins->WAIT);
				if (sm.direction == NS) sm.ns_crosswalk = 0;
				if (sm.direction == EW) sm.ew_crosswalk = 0;
				current_state = GREEN_WAIT_FLASH;
				break;
			}
			// INTENTIONALLY do not break as above
		case GREEN_WAIT:
			// TODO: block blinking task
			ledOn(GPIO_current, pins->GREEN);
			ledOn(GPIO_current, pins->WAIT);
			current_state = GREEN_WAIT;
			break;
		case YELLOW_WAIT:
			ledOn(GPIO_current, pins->YELLOW);
			ledOn(GPIO_current, pins->WAIT);

			/*
			 * not needed under normal operation, as blinking was
			 * blocked by prior state, but included anyways for
			 * robustness in case GREEN_WAIT was skipped
			 */
			// TODO: block blinking task (arguably redundant)
			current_state = YELLOW_WAIT;
			break;
		case RED_WAIT:
			ledOn(GPIO_current, pins->RED);
			ledOn(GPIO_current, pins->WAIT);

			// change direction
			ChangeDirection();
			current_state = RED_WAIT;
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
	ledOn(GPIO_other, pins_other->RED);
	ledOn(GPIO_other, pins_other->WAIT);
}

void ChangeDirection()
{
	// TODO: create while-loop that attempts to periodically retake the mutex on failure
	// toggle direction between NS and EW
	if (xSemaphoreTake(stateMachineHandle, (TickType_t) 100) == pdTRUE)
	{
		sm.direction = sm.direction == NS ? EW : NS;
		current_direction = sm.direction;
		xSemaphoreGive(stateMachineHandle);
	}
}

void SetCrosswalkFlag(char * direction)
{
	if (strcmp(direction, "N") == 0)
	{
		sm.ns_crosswalk = 1;
	}
	else if (strcmp(direction,"E") == 0)
	{
		sm.ew_crosswalk = 1;
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
