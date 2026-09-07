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
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "control.h"
#include "bms_action.h"
#include "cell.h"
#include "measure.h"
#include <stdio.h>
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
/* USER CODE BEGIN PV */

volatile uint32_t adc_raw = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);

    return len;
}

void display_the_info(Cell c);

void display_the_info(Cell c)
{
    PowerMode mode = get_power_mode();

    printf("Voltage: %.2f V\r\n", c.voltage);
    printf("Current: %.3f A\r\n", c.current);
    printf("Temperature: %.2f C\r\n", c.temp);
    printf("Charge: %.2f %%\r\n", c.state_of_charge);

    if (mode == LOW_POWER)
    {
        printf("Power mode: LOW POWER\r\n");
    }
    else
    {
        printf("Power mode: FULL POWER\r\n");
    }
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

  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();

  /* USER CODE BEGIN 2 */

  //Start motor PWM
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 4095);

  //Start red LED PWM
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 4095);

  //Start green LED PWM
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 4095);

  Cell c = get_cell_data();

  uint8_t control_mode = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
      //Take raw potentiometer values
      HAL_ADC_Start(&hadc1);
      HAL_ADC_PollForConversion(&hadc1, 100);

      adc_raw = HAL_ADC_GetValue(&hadc1);

      HAL_ADC_Stop(&hadc1);

      update_cell(&c);
      display_the_info(c);

      c.state = select_mode(c);

      float throttle_state = execute_state(&c);

      //Update control based on interrupts
      control_mode = update_control();

      PowerMode P = get_power_mode();

      float power_limit = 0.0f;

      if (P == LOW_POWER)
      {
          power_limit = 0.5f;
      }
      else
      {
          power_limit = 1.0f;
      }

      uint32_t output =
          (uint32_t)(throttle_state * adc_raw * power_limit);

      if (control_mode == 0)
      {
          //Potentiometer controls motor speed
          __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, output);
      }
      else
      {
          //Potentiometer controls LED brightness
          __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, output);
          __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, output);
      }

      printf("ADC: %lu\r\n", (unsigned long)adc_raw);

      printf("Control mode: %s\r\n",
             control_mode == 0 ? "MOTOR" : "LED");

      printf("Throttle limit: %.2f\r\n", throttle_state);

      printf("PWM: %lu\r\n", (unsigned long)output);

      printf("\r\n");

      HAL_Delay(1000);
  }

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
}

/* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK |
      RCC_CLOCKTYPE_SYSCLK |
      RCC_CLOCKTYPE_PCLK1 |
      RCC_CLOCKTYPE_PCLK2;

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(
          &RCC_ClkInitStruct,
          FLASH_LATENCY_0) != HAL_OK)
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

  __disable_irq();

  while (1)
  {
  }

  /* USER CODE END Error_Handler_Debug */
}


#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */

  /* USER CODE END 6 */
}

#endif /* USE_FULL_ASSERT */
