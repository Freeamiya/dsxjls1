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
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "VL53L0X.h"
#include "tasks.h"
#include "UI_ble.h"
#include "mpu6050.h"
#include "counter.h"
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
uint8_t rx_data[256] = {0};  // 接收缓冲区
MPU6050_t MPU6050;
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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_data, sizeof(rx_data));
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
  // vl53l0x tof
  initVL53L0X(1, &hi2c1);
  // Configure the sensor for high accuracy and speed in 20 cm.
  setSignalRateLimit(200);
  setVcselPulsePeriod(VcselPeriodPreRange, 6);
  setVcselPulsePeriod(VcselPeriodFinalRange, 12);
  setMeasurementTimingBudget(500 * 1000UL);
  startContinuous(0);
  MPU6050_Init(&hi2c2);
  HAL_Delay(100);
  if(task_index == 1) task1();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      MPU6050_Read_All(&hi2c2, &MPU6050);
//      uart_printf("%.2f,%.2f,%.2f\r\n",MPU6050.KalmanAngleX, MPU6050.KalmanAngleY, MPU6050.AngleZ);
      if(task_running == 1) {
          switch (task_index) {
            case 1:
                task1();
                break;
            case 2:
                task2();
                break;
            case 3:
                task3();
                break;
            case 4:
                task4();
                break;
            case 5:
                task5();
                break;
            case 6:
                task6();
                break;
            default: ;
          }
      }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        if (counter.led_ms > 0) {
            counter.led_ms -= 10;
        }
        if (counter.led_ms > 0) {
            HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);
        }else {
            HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);
        }
    }
}

int my_atoi(const char *str)
{
    int res = 0;
    int sign = 1;
    if (*str == '-') {
        sign = -1;
        str++;
    }
    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str - '0');
        str++;
    }
    return res * sign;
}

float my_atof(const char *str)
{
    float res = 0.0f;
    float frac = 0.0f;
    int sign = 1;
    int after_dot = 0;

    if (*str == '-') {
        sign = -1;
        str++;
    }

    while (*str)
    {
        if (*str >= '0' && *str <= '9') {
            if (after_dot) {
                frac = frac * 10 + (*str - '0');
                after_dot *= 10;
            } else {
                res = res * 10 + (*str - '0');
            }
        } else if (*str == '.') {
            if (after_dot) break;  // 已有小数点
            after_dot = 1;
        } else {
            break;
        }
        str++;
    }

    if (after_dot)
        res = res + frac / after_dot;

    return res * sign;
}

//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
//{
//    if (huart->Instance == USART1)
//    {
//        rx_data[Size] = '\0';  // 终止字符串，便于解析
//        char *var_name = strtok((char *)rx_data, ",");
//        char *value_str = strtok(NULL, ",");
//        if (var_name && value_str)
//        {
//            int found = 0;
//            char msg[64];
//            for (int i = 0; i < variable_count; ++i)
//            {
//                if (strcmp(var_name, variable_table[i].name) == 0)
//                {
//                    found = 1;
//                    switch (variable_table[i].type)
//                    {
//                        case VAR_INT:
//                            *(int *)(variable_table[i].ptr) = my_atoi(value_str);
//                            snprintf(msg, sizeof(msg), "%s=%d\r\n", var_name, *(int *)(variable_table[i].ptr));
//                            HAL_UART_Transmit_DMA(&huart1, (uint8_t *)msg, strlen(msg));
//                            break;
//                        case VAR_FLOAT:
//                            *(float *)(variable_table[i].ptr) = my_atof(value_str);
//                            snprintf(msg, sizeof(msg), "%s=%.2f\r\n", var_name, *(float *)(variable_table[i].ptr));
//                            HAL_UART_Transmit_DMA(&huart1, (uint8_t *)msg, strlen(msg));
//                            break;
//                    }
//                    break;
//                }
//            }
//            if (!found)
//            {
//                const char *err = "Variable not found\r\n";
////                HAL_UART_Transmit(&huart1, (uint8_t *)err, strlen(err), HAL_MAX_DELAY);
//                HAL_UART_Transmit_DMA(&huart1, (uint8_t *)err, strlen(err));
//            }
//        }
//        // 重启接收
//        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_data, sizeof(rx_data));
//        __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
//    }
//}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1)
    {
        rx_data[Size] = '\0';  // 添加字符串结束符，方便使用字符串函数

        // 简单字符串解析
        char *token = strtok((char *)rx_data, ",");
        if (token != NULL) {
            char *var_name = token;
            token = strtok(NULL, ",");
            if (token != NULL) {
                int value = my_atoi(token);  // 字符串转整数
                if (strcmp(var_name, "index") == 0) {
                    task_index = value;
                    // 可选：反馈当前变量状态
                    char msg[64];
                    snprintf(msg, sizeof(msg), "index = %d\r\n", task_index);
                    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)msg, strlen(msg));
                } else {
                    // 未知变量名，可选：发送错误信息
                    const char *error_msg = "Unknown variable\r\n";
                    HAL_UART_Transmit_DMA(&huart1, (uint8_t *) error_msg, strlen(error_msg));
                }
            }
        }
        // 重新开启DMA接收
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_data, sizeof(rx_data));
        __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
    }
}

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
