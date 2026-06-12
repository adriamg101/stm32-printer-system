/*
 * pwm_sim.h
 *
 *  Created on: 13 abr 2026
 *      Author: adria
 */

#ifndef INC_PWM_SIM_H_
#define INC_PWM_SIM_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

/*
 * Rang frequencies per velocitats:
 *  - Max (2mm/s) ->
 *  - Min (1mm/s) ->
 */
#define PWM_SIM_FREQ 10

// Funcio que va generant el pwm de les senyals A i B
void PWM_SIM_setSignalsAB();

void PWM_SIM_init(GPIO_TypeDef* GPIOx, uint16_t GPIO_SIG_A, uint16_t GPIO_SIG_B, TIM_HandleTypeDef *htim);

#endif /* INC_PWM_SIM_H_ */
