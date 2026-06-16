/*
 * bounce_handler.h
 *
 *  Created on: 13 jun 2026
 *      Author: adria
 */

#ifndef INC_BOUNCE_HANDLER_H_
#define INC_BOUNCE_HANDLER_H_

#include "constants.h"
#include "encoder_module.h"
#include "pwm_sim.h"
#include "sensor_handler.h"


void BNC_GPIOHandler();
void BNC_TIMHandler();

void BNC_init(GPIO_TypeDef* GPIO_Butt, uint16_t GPIO_Pin_Butt , TIM_HandleTypeDef *htim, IRQn_Type EXTI_Butt);

#endif /* INC_BOUNCE_HANDLER_H_ */
