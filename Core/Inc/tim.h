#ifndef __tim_H
#define __tim_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"


extern TIM_HandleTypeDef htim2;


void MX_TIM2_Init(void);
                        
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                    

#ifdef __cplusplus
}
#endif
#endif 

