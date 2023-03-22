#pragma once

#include "bsp_config.h"
#include "IO_def.h"
#include "main.h"
#include "sys.h"
#include "core_cm7.h"


#define BOARD_LED0_Toggle (HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_11))
#define BOARD_LEDEX (HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13))
void board_led_init();

