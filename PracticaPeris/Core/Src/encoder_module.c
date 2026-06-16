/*
 * encoder_module.c
 *
 *  Created on: 13 abr 2026
 *      Author: adria.mg & aleix.rovira
 */

#include "encoder_module.h"

#define ENC_60CM 3000	// * 0.2mm = 60cm
#define EXTI_SIG_A	EXTI15_10_IRQn
#define EXTI_SIG_B	EXTI15_10_IRQn

typedef enum {
	NO_DIRECTION,
	RL_DIRECTION,	// Right -> Left
	LR_DIRECTION	// Left -> Right
} ENC_Direction;

// Informacio necessaria per al encoder
static GPIO_TypeDef *ENC_GPIO_in;		//
static GPIO_TypeDef *ENC_GPIO_out;		//
static uint16_t ENC_GPIO_PIN_sigA;		// Pin input GPIO d'entrada del senyal A
static uint16_t ENC_GPIO_PIN_sigB;		// Pin input GPIO d'entrada del senyal A
static uint16_t ENC_GPIO_PIN_PULSE;		// Pin output GPIO de la senyal de Pulse (OnePulse simulat)
static uint16_t ENC_GPIO_PIN_ERR;		// Pin output GPIO de senyal d'error
static uint16_t ENC_GPIO_PIN_EOP;		// Pin output GPIO de senyal de tot OK
static TIM_HandleTypeDef *ENC_htim_OP;	// One-Pulse (simulat) timer
static TIM_HandleTypeDef *ENC_htim_FR;	// Free-running timer per comptar temps

// Variable de test (TODO: Esborrar en un futur)
static uint8_t is_working;

// Variables
static uint16_t num_flancs;				// Registre per comptar el nombre de flancs
static uint8_t direction;				// Registre que indica la direccio del punter
static unsigned long last_time;			// Registre que guarda el ultim temps registrat
static unsigned long total_time;		// Registre que conte el temps total
static unsigned long final_velocity;	// Registre amb la velocitat final calculada

static SignalEdge starting_edge;

// Desactivem les interrupcions de INA i INB
void _ENC_disable_input_interrupts() {
	HAL_NVIC_DisableIRQ(EXTI_SIG_A);
	HAL_NVIC_DisableIRQ(EXTI_SIG_B);
}

// Funcio privada que calcula la velocitat del puntal i la guarda en una variable.
void _ENC_calculate_velocity() {
	// Afegim el temps al total
	unsigned long actual_time = __HAL_TIM_GET_COUNTER(ENC_htim_FR);
	total_time += (unsigned long)(actual_time - last_time);
	last_time = actual_time;

	// Mirem si hem arribat al final de la fulla
	if (++num_flancs >= ENC_60CM) {
		// Activem senyal OUTEOP
		HAL_GPIO_WritePin(ENC_GPIO_out, ENC_GPIO_PIN_EOP, GPIO_PIN_SET);

		// Calcular velocidad mediana en (us/200um)
		final_velocity = (total_time / (unsigned long)num_flancs);

		// TODO: Esborrar en un futur
		is_working = 0;

		_ENC_disable_input_interrupts();
	}
}

// Funcio privada que s'executa quan hi ha un error en la direccio del puntal
void _ENC_direction_error() {
	_ENC_disable_input_interrupts();

	// TODO: Esborrar en un futur
	is_working = 0;

	// Setejem OUTERR
	HAL_GPIO_WritePin(ENC_GPIO_out, ENC_GPIO_PIN_ERR, GPIO_PIN_SET);
}

// Funcio que mira si la direccio es correcta i reacciona segons el resultat
void _ENC_check_direction(ENC_Direction dir, SignalEdge edge) {
 	if (direction != dir) {
		// Mirem si es que no s'habia setejat encara
		if (direction == NO_DIRECTION) {
			// Setejem el last time
			last_time = __HAL_TIM_GET_COUNTER(ENC_htim_FR);
			direction = dir;
		}
		else _ENC_direction_error();	// Error
	} else {
		// Tot OK, mirem si hem de comptar la velocitat
		if (edge == starting_edge) _ENC_calculate_velocity();
	}
}

SignalEdge _ENC_getSignalEdge(uint16_t GPIO_Pin) {
	if (GPIO_Pin == ENC_GPIO_PIN_sigA)
		return (HAL_GPIO_ReadPin(ENC_GPIO_in, ENC_GPIO_PIN_sigA)) ? A_RISING_EDGE : A_FALLING_EDGE;
	else
		return (HAL_GPIO_ReadPin(ENC_GPIO_in, ENC_GPIO_PIN_sigB)) ? B_RISING_EDGE : B_FALLING_EDGE;
}

void _ENC_setStartingEdge(SignalEdge edge) {
	starting_edge = edge;
	SEN_setStartingEdge(edge);
}

// Funcio que es crida quan salta la interrupcio del timer
void ENC_OnePulseModeSimulatedHandler() {
	// Posem el pulse a down
	HAL_GPIO_WritePin(ENC_GPIO_out, ENC_GPIO_PIN_PULSE, GPIO_PIN_RESET);
	// Parem el Timer per simular un OnePulse
	HAL_TIM_Base_Stop_IT(ENC_htim_OP);
}

void ENC_encode_signal(uint16_t GPIO_Pin) {
	// Primer de tot activem el TIM en mode OnePulse.
	// OUTPULSE simulat
	// Activem el Pulse
	HAL_GPIO_WritePin(ENC_GPIO_out, ENC_GPIO_PIN_PULSE, GPIO_PIN_SET);
	// Activem el timer i comencem a comptar
	HAL_TIM_Base_Start_IT(ENC_htim_OP);

	// Edge
	SignalEdge edge = _ENC_getSignalEdge(GPIO_Pin);
	if (!starting_edge) _ENC_setStartingEdge(edge);	// NO_EDGE = 0

	// Cridem els ADCs
	SEN_takeADCSample(edge);

	// Mirem que sigui correcte la direccio i o calculem la velocitat o marquem error
	if (edge == A_RISING_EDGE || edge == B_FALLING_EDGE)
		_ENC_check_direction(HAL_GPIO_ReadPin(ENC_GPIO_in, ENC_GPIO_PIN_sigB) ? LR_DIRECTION : RL_DIRECTION, edge);
	else // (edge == A_FALLING_EDGE || edge == B_RISING_EDGE)
		_ENC_check_direction(HAL_GPIO_ReadPin(ENC_GPIO_in, ENC_GPIO_PIN_sigB) ? RL_DIRECTION : LR_DIRECTION, edge);
}

void ENC_start_distance_count() {
	// TODO: Esborrar en un futur
	// Mirem si ja s'habia iniciat el process per a no fer res
	if (is_working) return;

	// Setejem totes els leds d'informacio a 0
	HAL_GPIO_WritePin(ENC_GPIO_out, ENC_GPIO_PIN_ERR, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ENC_GPIO_out, ENC_GPIO_PIN_EOP, GPIO_PIN_RESET);

	// TODO: Eliminar en un futur
	is_working = 1;

	// Setejem variables
	direction = NO_DIRECTION;
	num_flancs = 0;
	final_velocity = 0;
	total_time = 0;
	last_time = 0;
	starting_edge = NO_EDGE;

	__HAL_GPIO_EXTI_CLEAR_FLAG(ENC_GPIO_PIN_sigA);
	__HAL_GPIO_EXTI_CLEAR_FLAG(ENC_GPIO_PIN_sigB);
	HAL_NVIC_ClearPendingIRQ(EXTI_SIG_A);
	HAL_NVIC_ClearPendingIRQ(EXTI_SIG_B);

	// Activem les interrupcions de INA i INB
	HAL_NVIC_EnableIRQ(EXTI_SIG_A);
	HAL_NVIC_EnableIRQ(EXTI_SIG_B);
}

void ENC_init(
		GPIO_TypeDef* GPIOx_in,
		GPIO_TypeDef* GPIOx_out,
		uint16_t GPIO_SIG_A,
		uint16_t GPIO_SIG_B,
		uint16_t GPIO_PULSE,
		uint16_t GPIO_EOP,
		uint16_t GPIO_ERR,
		TIM_HandleTypeDef* htim_OP,
		TIM_HandleTypeDef* htim_FR) {
	ENC_GPIO_in = GPIOx_in;
	ENC_GPIO_out = GPIOx_out;
	ENC_GPIO_PIN_sigA = GPIO_SIG_A;
	ENC_GPIO_PIN_sigB = GPIO_SIG_B;
	ENC_GPIO_PIN_PULSE = GPIO_PULSE;
	ENC_GPIO_PIN_ERR = GPIO_ERR;
	ENC_GPIO_PIN_EOP = GPIO_EOP;
	ENC_htim_OP = htim_OP;
	ENC_htim_FR = htim_FR;

	is_working = 0;

	// Iniciem el free running timer
	HAL_TIM_Base_Start(ENC_htim_FR);

	// Desactivem les interrupcions de INA i INB fins INDEX
	ENC_OnePulseModeSimulatedHandler();
	_ENC_disable_input_interrupts();
}
