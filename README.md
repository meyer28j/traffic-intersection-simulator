# Traffic Intersection Simulator

## Summary
This project simulates a controller for a 4-way symmetrical traffic intersection using an RTOS on the Nucleo STM32-F103RB development board. 

Each set of lights is composed of a turn signal lamp, green lamp, yellow lamp, and red lamp. Each corner of the intersection also has two sets of two lamps, white and red, for controlling pedestrian traffic in each direction in accordance with the vehicle traffic. Each symmetrical component (e.g. the "north" traffic lights and opposing "south" traffic lights) are wired together into the same controlling port for simplicity. The lights run on a timer, cycling back and forth between traffic flowing North-South and East-West along with the respective pedestrian Walk and Wait signals.

There are four handware buttons:
1. pedestrian waiting to cross East-West
2. pedestrian waiting to cross North-South
3. toggle "Emergency" mode (all lights red)
4. toggle "No Power" mode (all lights red, flashing)

The RTOS will also be executing a command-line interface (CLI) that will monitor and report the status of the lights. Additionally, the CLI can be used to manually set a state (e.g. Emergency mode).

## Development
This project will be executed in two main phases:
1. The software interface and state machine implementation
    - See [Milestone Document](./doc/milestone-requirements.md)
2. The circuit construction and connection to the board
3. Handling pedestrian input (i.e. only displaying "walk" if a pedestrian has hit the crosswalk button) and refinement of timing for each state (e.g. turn-signal duration != all-red-lights duration)
