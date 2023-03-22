#include "main.h"
#include "rtthread.h"
#include "adc.h"


#define T_LED_PRIORITY 7
#define T_LED_STACK_SIZE 192
#define T_LED_TICK 20


uint32_t adc_buffer[12];
uint32_t adc_tmp[12];
uint32_t adc_averval[12];
static void t_led(void* param)
{
//	rt_kprintf("led task running\n");
	while(1)
	{
//		rt_kprintf("led task running\n");
		BOARD_LED0_Toggle;
		rt_thread_mdelay(500);
	}

}


int main(void)
{

	rt_thread_t tid=RT_NULL;
	tid =  rt_thread_create("led",t_led,RT_NULL,
	T_LED_STACK_SIZE,
	T_LED_PRIORITY,
	T_LED_TICK);
	if(tid != RT_NULL)
	{
//		rt_kprintf("Led thread is created\n");
		rt_thread_startup(tid);
	}
	#ifdef STM32F767xx
	HAL_ADC_Start_DMA(&hadc1,adc_buffer,12);
	__HAL_DMA_ENABLE_IT(&hdma_adc1,DMA_IT_TC);
	__HAL_DMA_DISABLE_IT(&hdma_adc1,DMA_IT_HT);
	__HAL_DMA_DISABLE_IT(&hdma_adc1,DMA_IT_TE);
	__HAL_DMA_DISABLE_IT(&hdma_adc1,DMA_IT_FE);
	__HAL_DMA_DISABLE_IT(&hdma_adc1,DMA_IT_DME);
	#endif
	ctrl_thread_init();
	return RT_EOK;
}





void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif