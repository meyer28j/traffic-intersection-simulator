/*
 * usart-cli.c
 *
 *  Created on: Oct 22, 2024
 *      Author: jacob
 */


#include "usart-cli.h"
#include "state_machine.h"

static const char ANSI_ERASE_SCREEN[] = 		{"\x1b[2J"};
static const char ANSI_ERASE_LINE[] = 			{"\x1b[2K"};
static const char ANSI_SCROLL_WINDOW[] = 		{"\x1b[8;20r"}; // row 8 to 20
static const char ANSI_SAVE_CURSOR_POS[] = 		{"\x1b[s"};
static const char ANSI_RETURN_CURSOR_POS[] = 	{"\x1b[u"};
static const char ANSI_MOVE_TO_STATUS_LINE[] = 	{"\x1b[5;0f"}; // row 5, column 0
static const char ANSI_HIDE_CURSOR[] =			{"\x1b[?25l"};
static const char ANSI_SHOW_CURSOR[] =			{"\x1b[?25h"};


static const char INTRO_MSG[] = "\r\n"
		"**Traffic Light Simulator Command Line Interface**"
		"\r\n"
		"Enter a command to control the simulator.\r\n"
		"\r\n"
		"NS Lights: UNINITIALIZED\r\n"
		"EW Lights: UNINITIALIZED\r\n"
		"***************\r\n"
		"\r\n";
static const char HELP_MSG[] = "Commands:\r\n"
		"(c)lear - Clear the command window\r\n"
		"(h)elp - Display this message again\r\n"
		"(p)eriod <state name> <period in ms> - Update state period\r\n";
static const char PROMPT[] = "Enter a command: ";
// error message used in main to check reception status, must be non-static
const char ERROR_CRITICAL[] = "Critical error, exiting program...\r\n";
const char ERROR_PERIOD[] = "Usage: period <state name> <period in ms> --- 1000 < 30000\r\n";

uint8_t c_pos = 0;
char input[MAX_CLI_LEN] = {'\0'};
char response[MSG_LEN] = {'\0'};

void CLIInit(UART_HandleTypeDef* huart)
{
	memset(response, 0, strlen(response)); // clear string buffer

	// erase screen, print intro message, create scroll window, print prompt
	snprintf(response, MSG_LEN, "%s%s%s%s%s", ANSI_ERASE_SCREEN,
			INTRO_MSG, ANSI_SCROLL_WINDOW, HELP_MSG, PROMPT);

	HAL_UART_Transmit(huart, (uint8_t*)response, strlen(response), 1000);
	RefreshStatus(huart);
}

void RefreshStatus(UART_HandleTypeDef* huart)
{
	// retrieve traffic light states as strings
	char NS_status[20];
	strlcpy(NS_status, StateToString(sm.ns_state), sizeof(NS_status));
	char EW_status[20];
	strlcpy(EW_status, StateToString(sm.ew_state), sizeof(EW_status));

	// move cursor to status line and rewrite "XX Lights:" messages
	snprintf(response, MSG_LEN, "%s%s%s"
			"%sNS Lights: %s\n\r"
			"%sEW Lights: %s"
			"%s%s",
			ANSI_SAVE_CURSOR_POS, ANSI_HIDE_CURSOR, ANSI_MOVE_TO_STATUS_LINE,
			ANSI_ERASE_LINE, NS_status,
			ANSI_ERASE_LINE, EW_status,
			ANSI_RETURN_CURSOR_POS, ANSI_SHOW_CURSOR);

	HAL_UART_Transmit(huart, (uint8_t*)response, strlen(response), TIMEOUT);

	return;
}

void HandleInput(UART_HandleTypeDef* huart, uint8_t c)
{
	if (c == '\r' || c == '\n')
	{ // user hits 'Enter' key
		// ensure carriage return
		HAL_UART_Transmit(huart, (uint8_t*)"\r\n", 2, TIMEOUT);

		// set last character as string terminal
		input[c_pos] = '\0';

		if (strcmp(input, "help") == 0
			|| strcmp(input, "h") == 0)
		{
			HAL_UART_Transmit(huart, (uint8_t*)HELP_MSG, strlen(HELP_MSG), TIMEOUT);
		}
		else if (strcmp(input, "clear") == 0
				|| strcmp(input,"c") == 0)
		{ // clear the scroll window, rows 7-20
			snprintf(response, MSG_LEN, "\x1b[7;0H\x1b[0J");
			HAL_UART_Transmit(huart, (uint8_t*)response, strlen(response), TIMEOUT);
		}
		else
		{
			// parse for period input argument
			char period_arg_state[20] = {'\0'};
			uint16_t period_arg_ms = 0;

			if (sscanf(input, "period %20s %hu", period_arg_state, &period_arg_ms) == 2
				|| sscanf(input, "p %20s %hu", period_arg_state, &period_arg_ms) == 2)
			{
				// convert state argument to enum
				uint16_t period_state = StringToState(period_arg_state);
				if (period_arg_ms > 30000 || period_arg_ms < 1000
					|| period_state == 65535)
				{
					HAL_UART_Transmit(huart, (uint8_t*)ERROR_PERIOD, strlen(ERROR_PERIOD), TIMEOUT);
				}
				else
				{
					if (xSemaphoreTake(statePeriodHandle, (TickType_t) 100) == pdTRUE)
					{
						state_periods[period_state] = period_arg_ms;
						xSemaphoreGive(statePeriodHandle);
					}
					snprintf(response, MSG_LEN, "Updating %s period to %d\r\n", period_arg_state, period_arg_ms);
					HAL_UART_Transmit(huart, (uint8_t*)response, strlen(response), TIMEOUT);
					RefreshStatus(huart);
				}
			} else { // invalid command

				snprintf(response, MSG_LEN, "'%s' is an unrecognized command.\r\n", input);
				HAL_UART_Transmit(huart, (uint8_t*)response, strlen(response), TIMEOUT);
			}
		}

		memset(input, 0, strlen(input)); // clear string buffer
		c_pos = 0;

		// display input prompt to user
		HAL_UART_Transmit(huart, (uint8_t*)PROMPT, strlen(PROMPT), TIMEOUT);
	}
	else if (c == 0x8)
	{ // user hits 'Backspace' key
		if (c_pos == 0)
		{
			return; // beginning of input, do nothing
		}
		input[c_pos] = 0;
		c_pos--;
		// display character deletion
		HAL_UART_Transmit(huart, (uint8_t*)"\b \b", 3, TIMEOUT);
	}
	else if (c_pos < MAX_CLI_LEN)
	{
		// disallow user to enter over-size command
		// add typed character to input string
		input[c_pos] = c;
		c_pos++;
		// echo input to user
		HAL_UART_Transmit(huart, &c, sizeof (c), TIMEOUT);
	}
}
