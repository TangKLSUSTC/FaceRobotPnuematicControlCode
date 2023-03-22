#include "IO_def.h"
#include "rtthread.h"
#include "bsp_config.h"

void io_group_init(const io_base *p, unsigned char length)
{
	GPIO_InitTypeDef ini;
	ini.Mode=GPIO_MODE_OUTPUT_PP;
	ini.Pull=GPIO_PULLDOWN;
	ini.Speed=GPIO_SPEED_FREQ_MEDIUM;
	for(unsigned char i=0;i<length;i++)
	{
		ini.Pin=p[i].order;
		HAL_GPIO_Init(p[i].port,&ini);
		io_control(&p[i],0);
	}
}

inline void io_control(const io_base *p,char x)
{
	x?\
	HAL_GPIO_WritePin(p->port,p->order,GPIO_PIN_SET):\
	HAL_GPIO_WritePin(p->port,p->order,GPIO_PIN_RESET);
}

void GPIO_CLK_ENA(void)
{
  /* GPIO Ports Clock Enable */
#ifdef STM32F767xx
	#ifdef USE_GPIOA
	__HAL_RCC_GPIOA_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOB)
	__HAL_RCC_GPIOB_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOC)
	__HAL_RCC_GPIOC_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOD)
	__HAL_RCC_GPIOD_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOE)
	__HAL_RCC_GPIOE_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOF)
	__HAL_RCC_GPIOF_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOG)
	__HAL_RCC_GPIOG_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOH)
	__HAL_RCC_GPIOH_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOI)
	__HAL_RCC_GPIOI_CLK_ENABLE();
	#endif
#endif
#ifdef STM32F103xE
	

#endif

}
INIT_BOARD_EXPORT(GPIO_CLK_ENA);
