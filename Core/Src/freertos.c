/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//static PlayDirectoryFunction *play_directory;
#include "fourier.h"
#include "tim.h"
#include "Audio.h"
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
/* USER CODE BEGIN Variables */
volatile char buffer[5];
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for musicTask */
osThreadId_t musicTaskHandle;
const osThreadAttr_t musicTask_attributes = {
  .name = "musicTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_1 */
osThreadId_t Task_1Handle;
const osThreadAttr_t Task_1_attributes = {
  .name = "Task_1",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_2 */
osThreadId_t Task_2Handle;
const osThreadAttr_t Task_2_attributes = {
  .name = "Task_2",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for SimpleMutex */
osSemaphoreId_t SimpleMutexHandle;
const osSemaphoreAttr_t SimpleMutex_attributes = {
  .name = "SimpleMutex"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void InitializePlayDirectory(PlayDirectoryFunction *fcn){
	//play_directory = fcn;
}
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartMusicTask(void *argument);
void Task1_init(void *argument);
void Task2_init(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
}
/* USER CODE END 4 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of SimpleMutex */
  SimpleMutexHandle = osSemaphoreNew(1, 1, &SimpleMutex_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of musicTask */
  musicTaskHandle = osThreadNew(StartMusicTask, NULL, &musicTask_attributes);

  /* creation of Task_1 */
  Task_1Handle = osThreadNew(Task1_init, NULL, &Task_1_attributes);

  /* creation of Task_2 */
  Task_2Handle = osThreadNew(Task2_init, NULL, &Task_2_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	UBaseType_t uxHighWaterMark;

	        /* Inspect our own high water mark on entering the task. */
		uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
	  myprintf("watermark bytes unused %d sin %f\n\r",uxHighWaterMark*4, arm_sin_f32(M_PI/6));
	osDelay(1000);
	myprintf("fourier status %d\n\r", process_fourier_transform());
	osDelay(1000);

  /* Infinite loop */
  for(;;)
  {
	  uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
	  myprintf("fourier status %d\n\r", process_fourier_transform());
	  myprintf("watermark bytes unused %d sin %f\n\r",uxHighWaterMark*4, arm_sin_f32(M_PI/6));
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartMusicTask */
/**
* @brief Function implementing the musicTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMusicTask */
void StartMusicTask(void *argument)
{
  /* USER CODE BEGIN StartMusicTask */

  // do some initializing
  myprintf("program started\n\r");
  init_sd();


  /* Infinite loop */
  for(;;)
  {
	play_directory("",0);
	osDelay(2000);
    HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
  }
  /* USER CODE END StartMusicTask */
}

/* USER CODE BEGIN Header_Task1_init */
/**
* @brief Function implementing the Task_1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task1_init */
void Task1_init(void *argument)
{
  /* USER CODE BEGIN Task1_init */
	static int cnt_1 = 0;
  /* Infinite loop */
  for(;;)
  {
	osDelay(100);
//    ST7920_Clear();
//	if(cnt_1%44 == 22){
//		osSemaphoreAcquire(SimpleMutexHandle, 1);
//		  ST7920_SendString(0,0, "Task1!");
//		  myprintf("wyswietlacz\n");
//		  osSemaphoreRelease(SimpleMutexHandle);
//	  }
//	cnt_1 = cnt_1>43?0:cnt_1;
//	cnt_1++;
	SetAudioVolume(180);
  }
  /* USER CODE END Task1_init */
}

/* USER CODE BEGIN Header_Task2_init */
/**
* @brief Function implementing the Task_2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task2_init */
void Task2_init(void *argument)
{
  /* USER CODE BEGIN Task2_init */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  delay_init();

  ST7920_Init();
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

	static int cnt_2 = 0;
	static volatile uint8_t prev = 0;
	buffer[4] = '\0';
  /* Infinite loop */
  for(;;)
  {
	  if(cnt_2%44 == 43){

	osSemaphoreAcquire(SimpleMutexHandle, 1);
	  ST7920_SendString(0,0, "Task2!");
	  osSemaphoreRelease(SimpleMutexHandle);
	  }

	  cnt_2 = cnt_2>43?0:cnt_2;
	  if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3)){
		  HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
		  htim2.Instance->CNT = 0;
		  //myprintf("fourier status %d\n\r", process_fourier_transform());
		  //myprintf("%f\n\r", arm_sin_f32(M_PI/6));
	  }

	  snprintf(buffer, 4, "%d", htim2.Instance->CNT);
	  //SetAudioVolume((htim2.Instance->CNT * 255)/1000);
	  if(prev!= htim2.Instance->CNT){
		  osSemaphoreAcquire(SimpleMutexHandle, 1);
		  ST7920_SendString(2,0, "000");
		  ST7920_SendString(2,0, buffer);
		  osSemaphoreRelease(SimpleMutexHandle);
		  prev = htim2.Instance->CNT;
	  }

	  cnt_2++;
	  osDelay(100);
	  //myprintf("counter val %d\n",htim2.Instance->CNT);
  }
  /* USER CODE END Task2_init */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

