/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "bsp_config.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef STM32F767xx
	#include "stm32f7xx_hal.h"
	#ifdef LC_tech_F767IG
		#include "LC_tech_F767.h"
	#endif
#endif

#ifdef STM32F103xE
	#include "stm32f1xx_hal.h"
	
#endif

#include "sys.h"
#include "controller.h"
extern uint32_t adc_buffer[12];
extern uint32_t adc_tmp[12];
extern uint32_t adc_averval[12];

void Error_Handler(void);



#ifdef __cplusplus
}
#endif

#endif