/**
 * @file		: state_machine.h
 * @brief 		: Defines structure and functions for finite state machine
 * 					controller for LEDs representing traffic lights.
 *
 *  Created on: Nov 24, 2024
 *      Author: jacob
 */

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include <string.h>
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"

typedef enum
{
	TURNING_WAIT,
	GREEN_WALK,
	GREEN_WAIT_FLASH,
	GREEN_WAIT,
	YELLOW_WAIT,
	RED_WAIT,
	EMERGENCY,
	NO_POWER,
} State;

typedef enum
{
	NS,
	EW
} Direction;

typedef struct
{
	State ns_state;			// current north/south lights state
	State ew_state;			// current east/west lights state
	Direction direction;	// current direction (NS or EW)
	uint8_t ns_crosswalk;	// flag for NS pedestrian crosswalk button
	uint8_t ew_crosswalk;	// flag for EW pedestrian crosswalk button
} TrafficLightStateMachine;

// externally available to CLI to update periods
// TODO: near end of project, update to realistic times
extern uint16_t state_periods[];

extern State current_state;

// make available to CLI status updates
extern TrafficLightStateMachine sm;

// mutex for state machine
// TODO: ensure type is osMutexId_t instead of SemaphoreHandle_t
extern SemaphoreHandle_t stateMachineHandle;
extern SemaphoreHandle_t statePeriodHandle;

/**
 * @brief: Utility for deactivating all outputs
 *
 * @details: To be used before switching states; this ensures
 * if any errors in the output occur that they are handled as
 * soon as the state changes again.
 *
 * Preconditions:
 * - state machine must be initialized
 *
 * Postconditions:
 * - all GPIO output ports are unset, turning off all LEDs
 */
void ClearOutputs();


/**
 * @brief: Changes state to the provided parameter
 *
 * @details: Calls cleanup function to deactivate any outputs
 * used in prior state before calling the next state's respective
 * initialization function.
 *
 * Preconditions:
 * - state machine must be initialized
 * - the 'next_state' parameter must be a valid state
 *
 * Postconditions:
 * - if a state is switched to RED_WAIT, it also changes the direction
 * so the next state change to TURNING_WAIT is in the other direction
 */
void ChangeState(State next_state);


/**
 * @brief: Toggles the direction between NS and EW
 *
 * Preconditions:
 * - state machine must be initialized
 *
 * Postconditions:
 * - state machine member "direction" changed to either
 * NS or EW depending on existing direction
 */
void ChangeDirection();

/**
 * @brief: Sets either the NS or EW flag based on direction input.
 * To be used later when changing state to GREEN_WALK for a given
 * direction, at which point the flag is reset to 0.
 *
 * Preconditions:
 * - state machine must be initialized
 *
 * Postconditions:
 * - state machine member "ns_crosswalk" or "ew_crosswalk" set to
 * 1 based on char input "N" or "E"
 */
void SetCrosswalkFlag(char * direction);

/**
 * @brief: Utility for retrieving state name from enum
 */
const char* StateToString(State state);

/**
 * @brief: Utility for retrieving state enum from name
 */
const uint16_t StringToState(const char* state_name);

#endif /* STATE_MACHINE_H_ */
