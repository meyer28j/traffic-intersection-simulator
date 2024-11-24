/*
 * state_machine.c
 *
 *  Created on: Nov 24, 2024
 *      Author: jacob
 */

#include "state_machine.h"

void SMInit()
{
	// set states to be advance-left NS
	sm.ns_state = TURNING_WAIT;
	sm.ew_state = RED_WAIT;
}

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

	// get state according to direction
	State direction_state = sm.direction == NS ? sm.ns_state : sm.ew_state;

	switch (next_state)
	{
		case RED_WAIT:
			// change direction
			change_direction();
		case EW:
			break;
		default:
			// should never reach this point
			Error_Handler();
	}

	return;
}

void change_direction()
{
	if (sm.direction == NS)
	{
		sm.direction = EW;
	}
	else
	{
		sm.direction = NS;
	}
}
