# Traffic Intersection Simulator

## Summary
This project simulates a controller for a 4-way symmetrical traffic intersection using an RTOS on the Nucleo STM32-F103RB development board. 

Each set of lights is composed of a turn signal lamp, green lamp, yellow lamp, and red lamp. Each corner of the intersection also has two sets of two lamps, white and red, for controlling pedestrian traffic in each direction in accordance with the vehicle traffic. Each symmetrical component (e.g. the "north" traffic lights and opposing "south" traffic lights) are wired together into the same controlling port for simplicity. The lights run on an adjustable timer, cycling back and forth between traffic flowing North-South and East-West along with the respective pedestrian Walk and Wait signals.

There are two handware buttons:
1. pedestrian waiting to cross East-West
2. pedestrian waiting to cross North-South

The RTOS also executes a command-line interface (CLI) through a UART connection that monitors and report the status of the lights. Additionally, the CLI can be used to manually set the duration of each light's state, shared across both directions.

## Implementation Details

### Tasks

There are 4 tasks:
1. **StateHandler**: executes the state machine's ChangeState function to continue to the next state. If the next state requires the blinking task, it is resumed before this task delays itself for the next state's period.
2. **updateCLI**: periodically refreshes the UART display as a backup in case the refresh function was not called after a display-relevant change.
3. **startCLIIT**: initializes the CLI and starts the UART receive interrupt. It then waits to receive data from the character queue, fed by UART interrupts, and handles the input character(s) accordingly.
4. **blinkDirection**: suspends itself unless the current state requires a blinking light (TURNING_WAIT or GREEN_WAIT_FLASHING) and then toggles the relevant LED pin repeatedly until it is suspended again by StateHandler moving on to a non-blinking state.

### Interrupts

There are 2 interrupts:
1. **UART Receive**: the uart handler receives a character from the connected device and adds the character to the queue to be handled by startCLIIT when available.
2. **Pedestrian Crosswalk**: there are two buttons, one for each direction, that when pushed trigger an interrupt that sets a flag for the given direction to enable the GREEN_WALK and GREEN_WAIT_FLASH states when the cycle reaches those states.

### Queues and Semaphores

As previously mentioned, a CLI character queue is populated by UART interrupts and then processed by the startCLIIT thread when available.

Mutexes are used to control access to the state machine (e.g. ChangeState function) and each state's period updates (e.g. HandleInput function when a period-update command is entered).

## Development
This project will be executed in two main phases:
1. The software interface and state machine implementation
    - See [Milestone Document](./doc/milestone-requirements.md)
2. The circuit construction and connection to the board
3. Handling pedestrian input (i.e. only displaying "walk" if a pedestrian has hit the crosswalk button) and refinement of timing for each state (e.g. turn-signal duration != all-red-lights duration)
