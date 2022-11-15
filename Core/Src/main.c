/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "i2s.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdarg.h> //for va_list var arg functions


#include "Audio.h"
#include "mp3dec.h"
#include "mp3.h"
#include "test_file.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
int mp3_buffer_counter = 0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#include "mp3.h"
#include <math.h>

#define AUDIO_I2C_ADDR	0x94
#define BUFFER_SIZE		4096

static uint8_t audio_data[2 * BUFFER_SIZE];
static uint8_t output_audio_data[2 * BUFFER_SIZE];
//some variables for FatFs
FATFS FatFs; 	//Fatfs handle
FIL fil; 		//File handle
FRESULT fres; //Result after operations

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//some variables for FatFs
static FIL                  file;
volatile int			    enum_done = 0;
static volatile uint8_t     audio_is_playing = 0;

/* just for test */
extern float                cur_ratio;
static uint16_t             cur_bpm = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void init_sd(){
	myprintf("\r\n~ SD card demo by kiwih ~\r\n\r\n");

	  HAL_Delay(1000); //a short delay is important to let the SD card settle



	  //Open the file system
	  fres = f_mount(&FatFs, "", 1); //1=mount now
	  if (fres != FR_OK) {
		myprintf("f_mount error (%i)\r\n", fres);
		while(1);
	  }

	  //Let's get some statistics from the SD card
	  DWORD free_clusters, free_sectors, total_sectors;

	  FATFS* getFreeFs;

	  fres = f_getfree("", &free_clusters, &getFreeFs);
	  if (fres != FR_OK) {
		myprintf("f_getfree error (%i)\r\n", fres);
		while(1);
	  }

	  //Formula comes from ChaN's documentation
	  total_sectors = (getFreeFs->n_fatent - 2) * getFreeFs->csize;
	  free_sectors = free_clusters * getFreeFs->csize;

	  myprintf("SD card stats:\r\n%10lu KiB total drive space.\r\n%10lu KiB available.\r\n", total_sectors / 2, free_sectors / 2);

//	  //Now let's try to open file "test.txt"
//	  fres = f_open(&fil, "test.txt", FA_READ);
//	  if (fres != FR_OK) {
//		myprintf("f_open error (%i)\r\n");
//		while(1);
//	  }
//	  myprintf("I was able to open 'test.txt' for reading!\r\n");
//
//	  //Read 30 bytes from "test.txt" on the SD card
//	  BYTE readBuf[30];
//
//	  //We can either use f_read OR f_gets to get data out of files
//	  //f_gets is a wrapper on f_read that does some string formatting for us
//	  TCHAR* rres = f_gets((TCHAR*)readBuf, 30, &fil);
//	  if(rres != 0) {
//		myprintf("Read string from 'test.txt' contents: %s\r\n", readBuf);
//	  } else {
//		myprintf("f_gets error (%i)\r\n", fres);
//	  }
//
//	  //Be a tidy kiwi - don't forget to close your file!
//	  f_close(&fil);
//
//	  //Now let's try and write a file "write.txt"
//	  fres = f_open(&fil, "write.txt", FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
//	  if(fres == FR_OK) {
//		myprintf("I was able to open 'write.txt' for writing\r\n");
//	  } else {
//		myprintf("f_open error (%i)\r\n", fres);
//	  }
//
//	  //Copy in a string
//	  strncpy((char*)readBuf, "a new file is made!", 19);
//	  UINT bytesWrote;
//	  fres = f_write(&fil, readBuf, 19, &bytesWrote);
//	  if(fres == FR_OK) {
//		myprintf("Wrote %i bytes to 'write.txt'!\r\n", bytesWrote);
//	  } else {
//		myprintf("f_write error (%i)\r\n");
//	  }
//
//	  //Be a tidy kiwi - don't forget to close your file!
//	  f_close(&fil);


}

void deinit_sd(){
	  //We're done, so de-mount the drive
	  f_mount(NULL, "", 0);
}

// printf override

// void send_char(char c)
// {
//   HAL_UART_Transmit(&huart2, (uint8_t*)c, 1, 1000);
// }
 
// int __io_putchar(int c)
// {
//     if (c=='\n')
//       send_char('\r');
//     send_char(c);
//     return c;
// }

// MP3

static void AudioCallback(void) {
    audio_is_playing = 0;
}

/* MP3 file read, provided to MP3 decoder */
//static uint32_t fd_fetch(void *parameter, uint8_t *buffer, uint32_t length) {
//    uint32_t read_bytes = 0;
//
//    static uint32_t r_bytes;
//    if(r_bytes < sizeof(test_file)-length){
//      memcpy(buffer, test_file + r_bytes, length);
//      r_bytes += length;
//      read_bytes = length;
//    } else
//    {
//
//        uint32_t bytes_left = sizeof(test_file)-r_bytes;
//        memcpy(buffer, test_file + r_bytes, bytes_left);
//        read_bytes = bytes_left;
//        r_bytes += bytes_left;
//
//    }
//    //myprintf("fetch length %d sizeof %d \n", length, read_bytes);
//    //read_bytes = r_bytes;
//    //f_read((FIL *)parameter, (void *)buffer, length, &read_bytes); TODO
//
//    if (read_bytes <= 0) {
//        r_bytes = 0;
//        read_bytes = 0;
//        return 0;
//    }
//
//    return read_bytes;
//}
//static uint32_t fd_fetch(void *parameter, uint8_t *buffer, uint32_t length) {
//	uint32_t read_bytes = 0;
//
//	f_read((FIL *)parameter, buffer, length, (UINT*)&read_bytes);
//    if (read_bytes <= 0) return 0;
//
//    return read_bytes;
//}

static uint32_t fd_fetch(void *parameter, uint8_t *buffer, uint32_t length) {
	uint32_t read_bytes = 0;

	f_read((FIL *)parameter, (void *)audio_data, length, (UINT*)&read_bytes);
	memcpy(buffer, audio_data, read_bytes);
	    if (read_bytes <= 0) return 0;

	    return read_bytes;
}

/*
 * bpm detect
 */
static void mp3_get_bpm(char* filename) {
    struct mp3_decoder  *decoder;
    uint16_t            bpm;

	if (FR_OK == f_open(&file, filename, FA_OPEN_EXISTING | FA_READ)) {
		/* decode mp3 */

        decoder = mp3_decoder_create();
        if (decoder != NULL) {
            decoder->fetch_data         = fd_fetch;
            decoder->fetch_parameter    = (void *)&file;
            decoder->output_cb          = NULL;     /* no need */

            if ((bpm = mp3_bpm_detect_run(decoder)) > 0) {
                cur_bpm = bpm;
            }

            /* delete decoder object */
            mp3_decoder_delete(decoder);
        }

        /* Re-initialize and set volume to avoid noise */
        InitializeAudio(Audio44100HzSettings);
        SetAudioVolume(0);

        /* reset the playing flag */
        audio_is_playing = 0;

        /* Close currently open file */
        f_close(&file);
    }
}
/*
 * MP3 player
 */

static uint32_t mp3_callback(MP3FrameInfo *header,
                             int16_t *buffer,
                             uint32_t length) {
    while (audio_is_playing == 1);

    audio_is_playing = 1;
    //myprintf("size %d\n", length);
	//myprintf("size %d %d %d %d\n", buffer[100], buffer[101], buffer[102], buffer[103]);
    ProvideAudioBuffer(buffer, length);

    return 0;
}

static void play_mp3(char* filename) {
    struct mp3_decoder *decoder;

	if (FR_OK == f_open(&file, filename, FA_OPEN_EXISTING | FA_READ)) {
		/* Play mp3 */

		InitializeAudio(Audio44100HzSettings);
		SetAudioVolume(0xBF);
		PlayAudioWithCallback(AudioCallback);

        decoder = mp3_decoder_create();
        if (decoder != NULL) {
          //myprintf("dekoder utworzony\n");
            decoder->fetch_data         = fd_fetch;
            decoder->fetch_parameter    = (void *)&file;
            decoder->output_cb          = mp3_callback;

            //while (mp3_decoder_run(decoder) != -1);
            while (mp3_decoder_run_pvc(decoder) != -1);

            /* delete decoder object */
            mp3_decoder_delete(decoder);
            myprintf("koniec mp3\n");
            mp3_buffer_counter = 0;
        }

        /* Re-initialize and set volume to avoid noise */
        InitializeAudio(Audio44100HzSettings);
        SetAudioVolume(0);

        /* reset the playing flag */
        audio_is_playing = 0;

        /* Close currently open file */
        f_close(&file);
    }
}


/*
 * play directory
 */
static const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

FRESULT play_directory (const char* path, unsigned char seek) {
	FRESULT     res;
	FILINFO     fno;
	DIR         dir;
//static void play_directory (const char* path, unsigned char seek) {

    /* This function is assuming non-Unicode cfg. */
	char        *fn;
	char        buffer[200];
//#if _USE_LFN
//	static char lfn[_MAX_LFN + 1];
//
//	fno.lfname  = lfn;
//	fno.lfsize  = sizeof(lfn);
//#endif


	res = f_opendir(&dir, path);
	if (res == FR_OK) {
		for (;;) {
            /* Read a directory item */
			res = f_readdir(&dir, &fno);

            /* Break on error or end of dir */
			if (res != FR_OK || fno.fname[0] == 0) break;

            /* Ignore dot entry */
			if (fno.fname[0] == '.') continue;

//        #if _USE_LFN
//			fn = *fno.lfname ? fno.lfname : fno.fname;
//        #else
			fn = fno.fname;
//        #endif
			if (fno.fattrib & AM_DIR) {
                /* It is a directory */

			} else {
                /* It is a file. */
				sprintf(buffer, "%s/%s", path, fn);

				/* Check if it is an mp3 file */
				if (strcmp("mp3", get_filename_ext(buffer)) == 0) {

					/* Skip "seek" number of mp3 files... */
					if (seek) {
						seek--;
						continue;
					}

					//mp3_get_bpm(buffer);
					play_mp3(buffer);
				}
			}
		}
	}

	return res;
}

// void myprintf(const char *fmt, ...) // debugging


void myprintf(const char *fmt, ...) {
  static char buffer[512];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  int len = strlen(buffer);
  HAL_UART_Transmit(&huart2, (uint8_t*)buffer, len, -1);

}
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
  /* FPU initialization */
  	SCB->CPACR |= ((3 << 10*2)|(3 << 11*2));
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2S3_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

  // for (int i = 0; i < BUFFER_SIZE; i++) {
	//   int16_t value = (int16_t)(32000.0 * sin(2.0 * M_PI * i / 22.0));
	//   audio_data[i * 2] = value;
	//   audio_data[i * 2 + 1] = value;
  // }


  InitializePlayDirectory(play_directory);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// void _init(){}
// void _exit(){}
// void _kill(){}
// void _getpid(){}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
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
