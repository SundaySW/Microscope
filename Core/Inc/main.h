/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
static inline void delay(uint32_t delay);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CURRENT_WIND_Z_Pin GPIO_PIN_1
#define CURRENT_WIND_Z_GPIO_Port GPIOF
#define RESET_Z_Pin GPIO_PIN_10
#define RESET_Z_GPIO_Port GPIOG
#define CONFIG_3_Pin GPIO_PIN_0
#define CONFIG_3_GPIO_Port GPIOA
#define CONFIG_2_Pin GPIO_PIN_1
#define CONFIG_2_GPIO_Port GPIOA
#define CONFIG_1_Pin GPIO_PIN_2
#define CONFIG_1_GPIO_Port GPIOA
#define ENABLE_Z_Pin GPIO_PIN_3
#define ENABLE_Z_GPIO_Port GPIOA
#define STEP_Y_Pin GPIO_PIN_4
#define STEP_Y_GPIO_Port GPIOA
#define DIR_Y_Pin GPIO_PIN_5
#define DIR_Y_GPIO_Port GPIOA
#define ENABLE_Y_Pin GPIO_PIN_6
#define ENABLE_Y_GPIO_Port GPIOA
#define RESET_Y_Pin GPIO_PIN_7
#define RESET_Y_GPIO_Port GPIOA
#define RESET_Y_EXTI_IRQn EXTI9_5_IRQn
#define CURRENT_WIND_Y_Pin GPIO_PIN_0
#define CURRENT_WIND_Y_GPIO_Port GPIOB
#define X_endpoint_PA8_Pin GPIO_PIN_8
#define X_endpoint_PA8_GPIO_Port GPIOA
#define Y_endpoint_PA8_Pin GPIO_PIN_9
#define Y_endpoint_PA8_GPIO_Port GPIOA
#define Z_endpoint_PA8_Pin GPIO_PIN_10
#define Z_endpoint_PA8_GPIO_Port GPIOA
#define DIR_Z_Pin GPIO_PIN_15
#define DIR_Z_GPIO_Port GPIOA
#define DIR_Z_EXTI_IRQn EXTI15_10_IRQn
#define STEP_Z_Pin GPIO_PIN_3
#define STEP_Z_GPIO_Port GPIOB
#define RESET_X_Pin GPIO_PIN_4
#define RESET_X_GPIO_Port GPIOB
#define ENABLE_X_Pin GPIO_PIN_5
#define ENABLE_X_GPIO_Port GPIOB
#define CURRENT_WIND_X_Pin GPIO_PIN_6
#define CURRENT_WIND_X_GPIO_Port GPIOB
#define STEP_X_Pin GPIO_PIN_7
#define STEP_X_GPIO_Port GPIOB
#define DIR_X_Pin GPIO_PIN_8
#define DIR_X_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

#define TIME_GRID_BTN_LONG_PRESS        1000
#define TOTAL_DISTANCE_N_OF_STEPS       2140
#define XAxisSize                       65535
#define YAxisSize                       65535
#define ZAxisSize                       1500

#define LOAD_UNLOAD_SPEED               1985
#define START_SPEED                     1569

#define CONFIG1_SPEED                   4535
#define CONFIG2_SPEED                   3969
#define CONFIG3_SPEED                   5003
#define CONFIG4_SPEED                   7129
#define CONFIG1_ACCELERATION            40
#define CONFIG2_ACCELERATION            32
#define CONFIG3_ACCELERATION            43
#define CONFIG4_ACCELERATION            52

enum BOARD_ERROR{
    NO_ERROR,
    LIMIT_SWITCH_ERROR,
    STANDBY_MOVEMENT_ERROR,
};
enum DEVICE_STATUS{
    DEVICE_STARTS,
    DEVICE_INITIAL_MOVEMENT,
    DEVICE_ERROR,
    DEVICE_RETURN_TO_INITIAL_STATE,
    DEVICE_MOTOR_MOVING,
    DEVICE_STANDBY
};
enum BTN_TYPE{
    PUSHBUTTON
};

enum MOTOR_EVENT {
    EVENT_NULL = 0,
    EVENT_STOP, // 	move stopped
    EVENT_CSS,  //	constant speed reached
    EVENT_CSE   //  constant speed end
};
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
