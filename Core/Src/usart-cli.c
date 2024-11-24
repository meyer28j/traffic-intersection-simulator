/*
 * usart-cli.c
 *
 *  Created on: Oct 22, 2024
 *      Author: jacob
 */


#include "usart-cli.h"

static const char ANSI_ERASE_SCREEN[] = {"\x1b[2J"};
static const char ANSI_SCROLL_WINDOW[] = {"\x1b[7;20r"};
static const char ANSI_SAVE_CURSOR_POS[] = {"\x1b[s"};
static const char ANSI_RETURN_CURSOR_POS[] = {"\x1b[u"};
static const char ANSI_MOVE_TO_STATUS_LINE[] = {"\x1b[5;0f"};

static const char INTRO_MSG[] = "\r\n"
		"\t**UART Command Line Interface**"
		"\r\n"
		"Enter a command to control the on-board LED.\r\n"
		"\r\n"
		"PERIOD: 0\r\n"
		"***************\r\n"
		"\r\n";
static const char HELP_MSG[] = "Commands:\r\n"
		"(c)lear - Clear the command window\r\n"
		"(h)elp - Display this message again\r\n"
		"(p)eriod - Set period of on-board LED in ms (50 - 3000)\r\n";
static const char PROMPT[] = "Enter a command: ";
// error message used in main to check reception status, must be non-static
const char ERROR_CRITICAL[] = "Critical error, exiting program...\r\n";
static const char ERROR_PERIOD[] = "Usage: enter a number between 50 and 3000.\r\n";

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
	// move cursor to status line and rewrite "PERIOD" message
	snprintf(response, MSG_LEN, "%s%sPERIOD: %d  %s",
			ANSI_SAVE_CURSOR_POS, ANSI_MOVE_TO_STATUS_LINE,
			period, ANSI_RETURN_CURSOR_POS);

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
			int period_arg = 0;
			if (sscanf(input, "period %d", &period_arg) == 1
				|| sscanf(input, "p %d", &period_arg) == 1)
			{
				if (period_arg > 3000 || period_arg < 50)
				{
					HAL_UART_Transmit(huart, (uint8_t*)ERROR_PERIOD, strlen(ERROR_PERIOD), TIMEOUT);
				}
				else
				{
					period = (uint16_t)period_arg;
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
