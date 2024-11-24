/**
 * @file		: state_machine.h
 * @brief 		: Defines structure and functions for finite state machine
 * 					controller for LEDs representing traffic lights.
 *
 *  Created on: Nov 24, 2024
 *      Author: jacob
 */

#ifndef SRC_STATE_MACHINE_H_
#define SRC_STATE_MACHINE_H_

typedef enum {
	TURNING_WAIT,
	GREEN_WALK,
	GREEN_WAIT_FLASH,
	GREEN_WAIT,
	YELLOW_WAIT,
	RED_WAIT,
	EMERGENCY,
	NO_POWER,
	STATE_COUNT 			// total number of states
} StateType;

// function pointer for State enter and run functions
typedef void (*StateFunction)();

typedef struct {
	StateType direction; 	// state enum to direct
	StateFunction enter;	// state-specific initialization
	StateFunction run; 		// periodic state execution (e.g. toggle blinking LEDs)
} State;

typedef struct {
	State ns_state;			// current north/south lights state
	State ew_state;			// current east/west lights state
} TrafficLightStateMachine;

// state machine instance
static TrafficLightStateMachine sm;

/**
 * @brief: Initialization for state machine
 *
 * @details: Sets state machine struct variables and begins
 * traffic light operation at NS_TURNING_WAIT.
 *
 * Postconditions:
 * - ns_current_state = TURNING_WAIT
 * - ew_current_state = RED_WAIT
 */
void SMInit();

/**
 * @brief: Utility for deactivating all outputs
 *
 * @details: To be used before switching states; this ensures
 * if any errors in the output occur that they are handled as
 * soon as the state changes again.
 *
 * Postconditions:
 * - all GPIO output ports are unset, turning off all LEDs
 */
void clear_outputs();

/**
 * @brief: Changes state to the provided parameter
 *
 * @details: Calls cleanup function to deactivate any outputs
 * used in prior state before calling the next state's respective
 * initialization function.
 *
 * Preconditions:
 * - the 'next_state' parameter must be a valid state
 */
void change_state(State next_state);

#endif /* SRC_STATE_MACHINE_H_ */
