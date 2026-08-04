/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "can.h"
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_can.h"
#include "bsp_time.h"
#include "bsp_gpio.h"
#include "bq76940.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define BQ_ACQUISITION_PERIOD_MS (500U)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

volatile bq76940_status_t bq_init_result = BQ76940_STATUS_NOT_INIT;
volatile bq76940_status_t bq_calibration_result = BQ76940_STATUS_NOT_INIT;
volatile bq76940_status_t bq_status_result = BQ76940_STATUS_NOT_INIT;
volatile int16_t bq_adc_gain_uv_per_lsb;
volatile int8_t bq_adc_offset_mv;
volatile uint8_t bq_system_status;
volatile bq76940_status_t bq_cell_read_result = BQ76940_STATUS_NOT_INIT;
volatile uint32_t bq_acquisition_attempts;
volatile uint32_t bq_acquisition_successes;
volatile uint32_t bq_acquisition_crc_errors;
volatile uint32_t bq_acquisition_timeout_errors;
volatile uint32_t bq_acquisition_other_errors;
volatile uint16_t bq_last_cell_mv[BQ76940_CELL_COUNT];

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
  MX_I2C1_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */
  if (bsp_can_init() != BSP_STATUS_SUCCESS)
  {
    Error_Handler();
  }

  {
    bq76940_calibration_t calibration;
    uint8_t system_status;

    bq_init_result = bq76940_init();
    bq_calibration_result = bq76940_get_calibration(&calibration);
    if (bq_calibration_result == BQ76940_STATUS_OK)
    {
      bq_adc_gain_uv_per_lsb = calibration.adc_gain_uv_per_lsb;
      bq_adc_offset_mv = calibration.adc_offset_mv;
    }

    bq_status_result = bq76940_read_status(&system_status);
    if (bq_status_result == BQ76940_STATUS_OK)
    {
      bq_system_status = system_status;
    }
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    {
      uint16_t cell_mv[BQ76940_CELL_COUNT];

      ++bq_acquisition_attempts;
      bq_cell_read_result = bq76940_read_cell_voltages(cell_mv);

      if (bq_cell_read_result == BQ76940_STATUS_OK)
      {
        ++bq_acquisition_successes;

        for (uint8_t cell_index = 0U;
             cell_index < BQ76940_CELL_COUNT;
             ++cell_index)
        {
          bq_last_cell_mv[cell_index] = cell_mv[cell_index];
        }
      }
      else if (bq_cell_read_result == BQ76940_STATUS_CRC_ERR)
      {
        ++bq_acquisition_crc_errors;
      }
      else if (bq_cell_read_result == BQ76940_STATUS_TIMEOUT)
      {
        ++bq_acquisition_timeout_errors;
      }
      else
      {
        ++bq_acquisition_other_errors;
      }
    }

    bsp_gpio_status_led_toggle();
    bsp_time_delay_ms(BQ_ACQUISITION_PERIOD_MS);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
