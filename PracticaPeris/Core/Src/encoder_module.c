/*
 * encoder_module.c
 *
 *  Created on: 13 abr 2026
 *      Author: adria.mg & aleix.rovira
 */

#include "encoder_module.h"

#define ENC_60CM 3000	// * 0.2mm = 60cm

#define EXTI_INDEX	EXTI0_IRQn
#define EXTI_SIG_A	EXTI15_10_IRQn
#define EXTI_SIG_B	EXTI15_10_IRQn

// Channel del OnePulse
#define TIM_CHANNEL_OP TIM_CHANNEL_1

typedef enum {
	NO_DIRECTION,
	RL_DIRECTION,	// Right -> Left
	LR_DIRECTION	// Left -> Right
} ENC_Direction;

// Informacio necessaria per al encoder
static GPIO_TypeDef *ENC_GPIO_in, *ENC_GPIO_out;
static uint16_t ENC_GPIO_PIN_sigA, ENC_GPIO_PIN_sigB, ENC_GPIO_PIN_ERR, ENC_GPIO_PIN_EOP;
static TIM_HandleTypeDef *ENC_htim_OP, *ENC_htim_FR;

// Variables
static uint16_t num_flancs;
static uint8_t direction;
static uint16_t last_time;
static uint32_t total_time;
static uint16_t final_velocity;

// Desactivem les interrupcions de INA i INB
void _ENC_disable_input_interrupts() {
	HAL_NVIC_DisableIRQ(EXTI_SIG_A);
	HAL_NVIC_DisableIRQ(EXTI_SIG_B);
}

// Funcio privada que calcula la velocitat del puntal i la guarda en una variable.
void _ENC_calculate_velocity() {
	// Afegim el temps al total
	uint16_t actual_time = __HAL_TIM_GET_COUNTER(ENC_htim_FR);
	total_time += (uint32_t)(actual_time - last_time);
	last_time = actual_time;

	// Mirem si hem arribat al final de la fulla
	if (++num_flancs >= ENC_60CM) {
		// Activem senyal OUTEOP
		HAL_GPIO_WritePin(ENC_GPIO_out, ENC_GPIO_PIN_EOP, GPIO_PIN_SET);

		// Calcular velocidad mediana en (um/s)
		final_velocity = (uint16_t)(total_time / (uint32_t)num_flancs);

		_ENC_disable_input_interrupts();
	}
}

// Funcio privada que s'executa quan hi ha un error en la direccio del puntal
void _ENC_direction_error() {
	_ENC_disable_input_interrupts();

	// Setejem OUTERR
	HAL_GPIO_WritePin(ENC_GPIO_out, ENC_GPIO_PIN_ERR, GPIO_PIN_SET);
}

// Funcio que mira si la direccio es correcta i reacciona segons el resultat
void _ENC_check_direction(ENC_Direction dir, uint8_t check_vel) {
	if (direction != dir) {
		// Mirem si es que no s'habia setejat encara
		if (dir == NO_DIRECTION) {
			// Setejem el last time
			last_time = __HAL_TIM_GET_COUNTER(ENC_htim_FR);
			direction = dir;
		}
		else _ENC_direction_error();	// Error
	} else {
		// Tot OK, mirem si hem de comptar la velocitat
		if (check_vel) _ENC_calculate_velocity();
	}
}

void ENC_encode_signal(uint16_t GPIO_Pin) {
	// Primer de tot activem el TIM en mode OnePulse.
	// OUTPULSE
	__HAL_TIM_DISABLE(ENC_htim_OP);
	__HAL_TIM_SET_COUNTER(ENC_htim_OP, 0);
	HAL_TIM_OnePulse_Start(ENC_htim_OP, TIM_CHANNEL_OP);


	// Mirem que sigui correcte la direccio i o calculem la velocitat o marquem error
	if (GPIO_Pin == ENC_GPIO_PIN_sigA) {
		// Primer mirem el flanc en A
		if (HAL_GPIO_ReadPin(ENC_GPIO_in, ENC_GPIO_PIN_sigA)) {
			// High flanc (mirem velocitat)
			_ENC_check_direction(HAL_GPIO_ReadPin(ENC_GPIO_in, ENC_GPIO_PIN_sigB) ? LR_DIRECTION : RL_DIRECTION, 1);
		} else {
			// Low flanc
			_ENC_check_direction(HAL_GPIO_ReadPin(ENC_GPIO_in, ENC_GPIO_PIN_sigB) ? RL_DIRECTION : LR_DIRECTION, 0);
		}
	} else { // (GPIO_Pin == ENC_GPIO_PIN_sigB)
		// Primer mirem el flanc en B
		if (HAL_GPIO_ReadPin(ENC_GPIO_in, ENC_GPIO_PIN_sigB)) {
			// High flanc
			_ENC_check_direction(HAL_GPIO_ReadPin(ENC_GPIO_in, ENC_GPIO_PIN_sigA) ? RL_DIRECTION : LR_DIRECTION, 0);
		} else {
			// Low flanc
			_ENC_check_direction(HAL_GPIO_ReadPin(ENC_GPIO_in, ENC_GPIO_PIN_sigA) ? LR_DIRECTION : RL_DIRECTION, 0);
		}
	}
}

void ENC_start_distance_count() {
	// Setejem totes els leds d'informacio a 0
	HAL_GPIO_WritePin(ENC_GPIO_out, ENC_GPIO_PIN_ERR, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ENC_GPIO_out, ENC_GPIO_PIN_EOP, GPIO_PIN_RESET);

	// Setejem variables
	direction = NO_DIRECTION;
	num_flancs = 0;
	final_velocity = 0;
	total_time = 0;
	last_time = 0;

	// Activem les interrupcions de INA i INB
	HAL_NVIC_EnableIRQ(EXTI_SIG_A);
	HAL_NVIC_EnableIRQ(EXTI_SIG_B);
}

void ENC_init(
		GPIO_TypeDef* GPIOx_in,
		GPIO_TypeDef* GPIOx_out,
		uint16_t GPIO_SIG_A,
		uint16_t GPIO_SIG_B,
		uint16_t GPIO_EOP,
		uint16_t GPIO_ERR,
		TIM_HandleTypeDef *htim_OP,
		TIM_HandleTypeDef *htim_FR) {
	ENC_GPIO_in = GPIOx_in;
	ENC_GPIO_out = GPIOx_out;
	ENC_GPIO_PIN_sigA = GPIO_SIG_A;
	ENC_GPIO_PIN_sigB = GPIO_SIG_B;
	ENC_GPIO_PIN_ERR = GPIO_ERR;
	ENC_GPIO_PIN_EOP = GPIO_EOP;
	ENC_htim_OP = htim_OP;
	ENC_htim_FR = htim_FR;

	// Iniciem el free running timer
	HAL_TIM_Base_Start(ENC_htim_FR);

	// Desactivem les interrupcions de INA i INB fins INDEX
	HAL_NVIC_EnableIRQ(EXTI_INDEX);
	_ENC_disable_input_interrupts();
}
