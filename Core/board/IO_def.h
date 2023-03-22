#pragma once
	#if defined(STM32F103xE)
		#include "stm32f1xx.h"
		#include "main.h"
	#endif

	#if defined(STM32F767xx)
		#include "stm32f767xx.h"
		#include "main.h"
	#endif
#include "bsp_config.h"
#define PA GPIOA
#define PB GPIOB
#define PC GPIOC
#define PD GPIOD
#define PE GPIOE
#define PF GPIOF
#define PI GPIOI
#define PH GPIOH

#define Pin0 GPIO_PIN_0
#define Pin1 GPIO_PIN_1
#define Pin2 GPIO_PIN_2
#define Pin3 GPIO_PIN_3
#define Pin4 GPIO_PIN_4
#define Pin5 GPIO_PIN_5
#define Pin6 GPIO_PIN_6
#define Pin7 GPIO_PIN_7
#define Pin8 GPIO_PIN_8
#define Pin9 GPIO_PIN_9
#define Pin10 GPIO_PIN_10
#define Pin11 GPIO_PIN_11
#define Pin12 GPIO_PIN_12
#define Pin13 GPIO_PIN_13
#define Pin14 GPIO_PIN_14
#define Pin15 GPIO_PIN_15



	
typedef struct io
{
	GPIO_TypeDef* port;
	uint32_t			order;
}io_base,*io_base_p;

extern const io_base muscle_in[12];
extern const io_base muscle_out[12];
void io_group_init(const io_base *p, unsigned char length);
void io_control(const io_base *p,char x);
