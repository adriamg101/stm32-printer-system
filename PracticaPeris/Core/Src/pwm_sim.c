/*
 * pwm_sim.c
 *
 *  Created on: 13 abr 2026
 *      Author: adria.mg & aleix.rovira
 */

#include "pwm_sim.h"

// Informacio pel simulador de PWM
static GPIO_TypeDef *PWM_SIM_GPIO;
static uint16_t PWM_SIM_GPIO_PIN_sigA;
static uint16_t PWM_SIM_GPIO_PIN_sigB;

static uint8_t order = 0;

void PWM_SIM_PressButton() {
	// La idea es que si es a l'inici de la execucio no pasa res,
	// pero si s'esta movent el punter hi haura un canvi de direccio i
	// per tant donara error-
	order = ~order;
}

void PWM_SIM_setSignalsAB() {
	// Dona igual quina senyal comença, farem bit toggle
	if (order) HAL_GPIO_TogglePin(PWM_SIM_GPIO, PWM_SIM_GPIO_PIN_sigA);
	else HAL_GPIO_TogglePin(PWM_SIM_GPIO, PWM_SIM_GPIO_PIN_sigB);
	// Fem un bit toggle de order per a pasar de 0 a 255 i alternar el if en cada iteracio
	order = ~order;
}

void PWM_SIM_init(GPIO_TypeDef* GPIOx, uint16_t GPIO_SIG_A, uint16_t GPIO_SIG_B, TIM_HandleTypeDef *htim) {
	PWM_SIM_GPIO = GPIOx;
	PWM_SIM_GPIO_PIN_sigA = GPIO_SIG_A;
	PWM_SIM_GPIO_PIN_sigB = GPIO_SIG_B;

	HAL_TIM_Base_Start_IT(htim);
}
