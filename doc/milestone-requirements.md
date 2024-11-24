# Milestone Requirements
**Author:** Jacob Meyer, 200364255
**Authoring Date:** Nov. 19, 2024
**Submission Date:** Nov. 20, 2024

See [Project Summary](../README.md) for broader details.

## Requirement Details
The milestone revolves around the initial implementing the software of the simulator. Briefly, this entails implementing a finite state machine controlled by the RTOS that moves through each of the logical states of the traffic intersection, as well as a CLI that displays and can manually override this information. 

### RTOS Tasks
1. StartCLIInterrupt
    - Executes the `HAL_UART_Receive_IT` function to begin interrupt-driven user-UART interaction.
    - This task will terminate after activating the interrupt, as it passes control to the `HAL_UART_RxCpltCallback` function to act when the interrupt fires, which in turn also restarts the `HAL_UART_Receive_IT` call
2. StartUpdateCLI
    - Manually refreshes the CLI every few seconds by using the `RefreshStatus` function
    - Note that every function that updates the state will also call `RefreshStatus` after executing, so this thread is only to handle potential errors
3. StartStateHandler
    - Control state execution
    - Control state change, along with state entrance and exit functions
    - All states will use the same osDelay() value
