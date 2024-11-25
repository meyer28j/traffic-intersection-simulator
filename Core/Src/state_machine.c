/*
 * state_machine.c
 *
 *  Created on: Nov 24, 2024
 *      Author: jacob
 */

#include "state_machine.h"

// state machine instance
static TrafficLightStateMachine sm = {RED_WAIT, RED_WAIT, NS};

void clear_outputs()
{
	// TODO: unset all GPIO port/pins used for LEDs
	return;
}

void enter_state(State next_state)
{
	return;
}

void change_state(State next_state)
{
	// turn off all LEDs regardless of state
	clear_outputs();

	// get relevant state machine directional state
	State* direction_state = sm.direction == NS ? &sm.ns_state : &sm.ew_state;

	// change to the next state
	*direction_state = next_state;

	/*
	 * handle:
	 * 1. new state needs to enable/disable flashing lights
	 * 2. new state changes direction (i.e. enter RED_WAIT)
	 */
	switch (next_state)
	{
		case TURNING_WAIT:
			// TODO: unblock blinking task for TURNING signal
			break;
		case GREEN_WALK:
			// TODO: block blinking task
			break;
		case GREEN_WAIT_FLASH:
			// TODO: unblock blinking task for pedestrian WAIT
			break;
		case GREEN_WAIT:
			// TODO: block blinking task
			break;
		case YELLOW_WAIT:
			/*
			 * not needed under normal operation, as blinking was
			 * blocked by prior state, but included anyways for
			 * robustness in case GREEN_WAIT was skipped
			 */
			// TODO: block blinking task (arguably redundant)
			break;
		case RED_WAIT:
			// change direction
			change_direction();
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

	return;
}

void change_direction()
{
	// toggle direction between NS and EW
	sm.direction = sm.direction == NS ? EW : NS;
}
