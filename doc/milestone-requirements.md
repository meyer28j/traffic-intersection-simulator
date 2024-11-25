# Milestone Requirements
**Author:** Jacob Meyer, 200364255
**Authoring Date:** Nov. 19, 2024
**Submission Date:** Nov. 24, 2024

See [Project Summary](../README.md) for broader details.

## Requirement Details
The milestone revolves around the initial implementing the software of the simulator. Briefly, this entails implementing a finite state machine controlled by the RTOS that moves through each of the logical states of the traffic intersection, as well as a CLI that displays this information. 

### RTOS Tasks
1. StartStateHandler
    - Periodically change state in a logical order (TURN, GREEN, YELLOW, RED, switch direction, repeat)
    - Calls `RefreshStatus` after state change to update display immediately
    - All states use the same osDelay() value for now
2. StartUpdateCLI
    - Frequently refreshes the CLI using the `RefreshStatus` function
    - This task is included as a backup in case code that normally calls `RefreshStatus` is blocked
3. StartCLIInterrupt
    - Initializes CLI display
    - Executes the `HAL_UART_Receive_IT` function to begin interrupt-driven user-UART interaction.
    - This task will suspend after activating the interrupt, as it passes control to the `HAL_UART_RxCpltCallback` function, which in turn also restarts the `HAL_UART_Receive_IT` call
4. StartBlinkDirection
    - Stub for task to control blinking of specified LED

## Remaining Work
1. Add error handling for tasks failing to acquire a mutex
3. Add queue for CLI input, as functions are currently called directly from the RX interrupt
4. Construct and connect hardware representation of the traffic light intersection
5. Add GPIO library to execute light changes required for each state
    - Only enables LEDs
    - Operation assumes `ClearOutput` called before each state change that unsets all LED outputs before setting the new state's LEDs
6. Implement task that blinks a given light(s)
    - Blocks/unblocks for required states (TURNING_WAIT, GREEN_WAIT_FLASH, and NO_POWER)
7. Add CLI control of state machine
    - Manually set state
    - Update state period
    - pause/resume state execution
    - Enter and restore from EMERGENCY and NO_POWER states
8. Add hardware interrupts simulating a pedestrian hitting the crosswalk button
    - Ignored in EMERGENCY and NO_POWER states
    - (Stretch) add mechanism to skip GREEN_WALK and GREEN_WAIT_FLASH state if not pressed before the cycle starts; GREEN_WAIT running time should be extended accordingly
9. (Stretch) add timestamped logging file output