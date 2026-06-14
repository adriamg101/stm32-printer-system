/*
 * bounce_handler.c
 *
 *  Created on: 13 jun 2026
 *      Author: adria.mg & aleix.rovira
 */

#include "bounce_handler.h"

static GPIO_TypeDef* BNC_GPIO_Butt;
static uint16_t BNC_GPIO_Pin_Butt;
static TIM_HandleTypeDef *BNC_htim;
static IRQn_Type BNC_EXTI_Butt;

static uint8_t butt_state;	// Guarda l'estat del boto per comprovacions

void BNC_GPIOHandler() {
	uint8_t new_butt_state = HAL_GPIO_ReadPin(BNC_GPIO_Butt, BNC_GPIO_Pin_Butt);

	// Mirem si s'ha de contar rebots
	if (new_butt_state != butt_state) {
		butt_state = new_butt_state;
		// Desactivem les interrupcions del botto
		HAL_NVIC_DisableIRQ(BNC_EXTI_Butt);

		// Activem el timer
		HAL_TIM_Base_Start_IT(BNC_htim);
	}
}

void BNC_TIMHandler() {
	uint8_t new_butt_state = HAL_GPIO_ReadPin(BNC_GPIO_Butt, BNC_GPIO_Pin_Butt);

	// Parem el timer (Simulacio OnePulse)
	HAL_TIM_Base_Stop_IT(BNC_htim);

	// Mirem si s'ha pitjat el boto de veritat
	if (butt_state && new_butt_state) {
		// Important executar abans el handler del PWM_SIM.
		PWM_SIM_PressButton();
		ENC_start_distance_count();
	}
	butt_state = new_butt_state;

	// Tornem a activar les interrupcions
	HAL_NVIC_EnableIRQ(BNC_EXTI_Butt);
}

void BNC_init(GPIO_TypeDef* GPIO_Butt, uint16_t GPIO_Pin_Butt , TIM_HandleTypeDef *htim, IRQn_Type EXTI_Butt) {
	BNC_GPIO_Butt = GPIO_Butt;
	BNC_GPIO_Pin_Butt = GPIO_Pin_Butt;
	BNC_htim = htim;
	BNC_EXTI_Butt = EXTI_Butt;

	HAL_NVIC_EnableIRQ(BNC_EXTI_Butt);
}
