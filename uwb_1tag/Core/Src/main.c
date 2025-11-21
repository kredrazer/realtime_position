/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306_conf_template.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"
#include "ssd1306.h"
#include "stdio.h"
#include "icm20948.h"

#include <stdio.h>
#include <string.h>
#include "math.h"
#include "app_uwb.h"
#include "app_uart_handler.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/*
 * SPI1: DW1000
 * SPI2: ICM20948 : CS = B11
 * SPI3: BMP280
 * I2C3: OLED
 * B4, B5: LED
 * BUTTON: D2
 * UART1: DEBUG
 * STLINK debug: Không cắm chân 3v3, cắm pin 4.2V 2000mAh, bật nguồn, hoặc ko dùng pin mà cắm usb 5V
*/

uint8_t *data = "Hello USB CDC\n";
uint8_t buf_dis1[20];
uint8_t buffer[64]; 
uint8_t anchor_id=0;// sau đó extern sang usb_cdc_if.c

extern unsigned char garfield_128x64[];
extern unsigned char github_logo_64x64[];

axises my_gyro;
axises my_accel;
axises my_mag;
double a_x =0;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define RNG_DELAY_MS 1000
//
///* Default antenna delay values for 64 MHz PRF. See NOTE 2 below. */
//#define TX_ANT_DLY 16436
//#define RX_ANT_DLY 16436
//
///* Length of the common part of the message (up to and including the function code, see NOTE 3 below). */
//#define ALL_MSG_COMMON_LEN 10
//
///* Indexes to access some of the fields in the frames defined above. */
//#define ALL_MSG_SN_IDX 2
//#define RESP_MSG_POLL_RX_TS_IDX 10
//#define RESP_MSG_RESP_TX_TS_IDX 14
//#define RESP_MSG_TS_LEN 4
//
///* Buffer to store received response message.
// * Its size is adjusted to longest frame that this example code is supposed to handle. */
//#define RX_BUF_LEN 20
//#define UUS_TO_DWT_TIME 65536
//#define POLL_TX_TO_RESP_RX_DLY_UUS 300
//#define RESP_RX_TIMEOUT_UUS 400
//#define SPEED_OF_LIGHT 299702547
double dis0, dis1, dis2, dis3 = 0;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


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
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
//  MX_USB_DEVICE_Init();
  MX_I2C3_Init();
  MX_TIM1_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
//  ssd1306_Init();
  app_uwb_init();
  UART_HE_Receive_IT_Tag();
  HAL_TIM_Base_Start(&htim1);
  TIM1->CNT = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//  //*-------tag_code--------*//
	  app_uwb_process_beacon_tag();
	  app_uwb_process_twr_tag(&dis0, &dis1, &dis2, &dis3);
	  app_uwb_process_dist_send(dis0,dis1,dis2,dis3);

     //*-------anchor_code--------*//
//     app_uwb_process_beacon_anchor(anchor_id);
//     app_uwb_process_twr_anchor(anchor_id);
//     app_uwb_process_dist_revc(&dis0, &dis1, &dis2, &dis3,anchor_id);
// check_becon_send_code
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);

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
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
#ifdef USE_FULL_ASSERT
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
