/*
 * encoder_module.h
 *
 *  Created on: 13 abr 2026
 *      Author: adria
 */

#ifndef INC_ENCODER_MODULE_H_
#define INC_ENCODER_MODULE_H_

#include "constants.h"
#include "sensor_handler.h"

#define ENC_SIG_A	1
#define ENC_SIG_B	0

void ENC_OnePulseModeSimulatedHandler();
void ENC_encode_signal(uint16_t GPIO_Pin);
void ENC_start_distance_count();

void ENC_init(
		GPIO_TypeDef* GPIOx_in,
		GPIO_TypeDef* GPIOx_out,
		uint16_t GPIO_SIG_A,
		uint16_t GPIO_SIG_B,
		uint16_t GPIO_PULSE,
		uint16_t GPIO_EOP,
		uint16_t GPIO_ERR,
		TIM_HandleTypeDef* htim_OP,
		TIM_HandleTypeDef* htim_FR);

#endif /* INC_ENCODER_MODULE_H_ */
