/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "task.h" // vTaskSuspend used in task startCLIIT
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* Definitions for stateHandler */
osThreadId_t stateHandlerHandle;
const osThreadAttr_t stateHandler_attributes = {
  .name = "stateHandler",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for updateCLI */
osThreadId_t updateCLIHandle;
const osThreadAttr_t updateCLI_attributes = {
  .name = "updateCLI",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for startCLIIT */
osThreadId_t startCLIITHandle;
const osThreadAttr_t startCLIIT_attributes = {
  .name = "startCLIIT",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for blinkDirection */
osThreadId_t blinkDirectionHandle;
const osThreadAttr_t blinkDirection_attributes = {
  .name = "blinkDirection",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for cliChar */
osMessageQueueId_t cliCharHandle;
const osMessageQueueAttr_t cliChar_attributes = {
  .name = "cliChar"
};
/* Definitions for stateMachine */
osMutexId_t stateMachineHandle;
const osMutexAttr_t stateMachine_attributes = {
  .name = "stateMachine"
};
/* Definitions for statePeriod */
osMutexId_t statePeriodHandle;
const osMutexAttr_t statePeriod_attributes = {
  .name = "statePeriod"
};
/* USER CODE BEGIN PV */
volatile uint16_t period = 400;

uint8_t RXChar;			// input character from HAL_UART_Receive

HAL_StatusTypeDef status; // HAL_OK, HAL_ERROR, HAL_BUSY, HAL_TIMEOUT
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
void StartStateHandler(void *argument);
void StartUpdateCLI(void *argument);
void StartCLIInterrupt(void *argument);
void StartBlinkDirection(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
  * @brief process a receiving character from USART
  *        The goal is to keep the function as fast as possible
  *        and to delegate the slow tasks like transmission
  *        to RTOS tasks to increase the availability of the
  *        function to serve the next interrupt
  * @param huart:  handler to USART2 ( the only used one - no need to check)
  * @retval None
  */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of stateMachine */
  stateMachineHandle = osMutexNew(&stateMachine_attributes);

  /* creation of statePeriod */
  statePeriodHandle = osMutexNew(&statePeriod_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of cliChar */
  cliCharHandle = osMessageQueueNew (32, sizeof(uint8_t), &cliChar_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of stateHandler */
  stateHandlerHandle = osThreadNew(StartStateHandler, NULL, &stateHandler_attributes);

  /* creation of updateCLI */
  updateCLIHandle = osThreadNew(StartUpdateCLI, NULL, &updateCLI_attributes);

  /* creation of startCLIIT */
  startCLIITHandle = osThreadNew(StartCLIInterrupt, NULL, &startCLIIT_attributes);

  /* creation of blinkDirection */
  blinkDirectionHandle = osThreadNew(StartBlinkDirection, NULL, &blinkDirection_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_6
                          |GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA6 PA7 PA8
                           PA9 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB10 PB4 PB6
                           PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_6
                          |GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// push received char into queue
	if (xQueueSendFromISR(cliCharHandle, &RXChar, NULL) != pdPASS)
	{
		// queue full or something went wrong
		Error_Handler();
	}
	HAL_UART_Receive_IT(huart, &RXChar, 1);
	/*
	// wait until status is ok
	while((HAL_UART_GetState(huart)&HAL_UART_STATE_BUSY_RX)
			== HAL_UART_STATE_BUSY_RX);
	// restart UART character reception
	status = HAL_UART_Receive_IT(huart, &RXChar, 1);
	if (status != HAL_OK)
	{
		Error_Handler();
	}
	*/
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	// do nothing! :O
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/*
	 * No debounce is needed as this simply sets a flag to 1,
	 * to be unset later when the state changes to GREEN_WALK
	 * for a given direction
	 */
	if (GPIO_Pin == GPIO_PIN_0)
	{ // PC0 for NS crosswalk
		SetCrosswalkFlag("N");
	}
	else if (GPIO_Pin == GPIO_PIN_1)
	{ // PC1 for EW crosswalk
		SetCrosswalkFlag("E");
	}
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartStateHandler */
/**
  * @brief  Function implementing the stateHandler thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartStateHandler */
void StartStateHandler(void *argument)
{
  /* USER CODE BEGIN 5 */
	/* Infinite loop */
	for(;;)
	{
		ChangeState((current_state + 1) % 6); // 6 normal states of operation
		RefreshStatus(&huart2);
		// current_state is updated after calling ChangeState()
		osDelay(state_periods[current_state]);
	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartUpdateCLI */
/**
* @brief Function implementing the updateCLI thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUpdateCLI */
void StartUpdateCLI(void *argument)
{
  /* USER CODE BEGIN StartUpdateCLI */
	/* Infinite loop */
	for(;;)
	{
		RefreshStatus(&huart2);
		osDelay(500);
	}
  /* USER CODE END StartUpdateCLI */
}

/* USER CODE BEGIN Header_StartCLIInterrupt */
/**
* @brief Function implementing the startCLIInterrupt thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCLIInterrupt */
void StartCLIInterrupt(void *argument)
{
  /* USER CODE BEGIN StartCLIInterrupt */
	// initialize CLI
	 CLIInit(&huart2);

	// begin receiving characters through UART
	// interrupt callback restarts the Receive_IT call
	HAL_UART_Receive_IT(&huart2, &RXChar, 1);

	char nextChar = '\0';
	for (;;)
	{
		// block until a character is available in queue
		if (xQueueReceive(cliCharHandle, &nextChar, portMAX_DELAY) == pdPASS)
		{
			HandleInput(&huart2, nextChar);
		}
	}
  /* USER CODE END StartCLIInterrupt */
}

/* USER CODE BEGIN Header_StartBlinkDirection */
/**
* @brief Function implementing the blinkDirection thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBlinkDirection */
void StartBlinkDirection(void *argument)
{
  /* USER CODE BEGIN StartBlinkDirection */
	// TODO: 	refactor to use an LED provided from argument
	//			rather than figuring it out from a given direction
	Direction direction = *((Direction *) argument);

	/* Infinite loop */
	for(;;)
	{
	osDelay(1);
	}
  /* USER CODE END StartBlinkDirection */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM3 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM3) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
