/*
 * constants.h
 *
 *  Created on: 15 jun 2026
 *      Author: adria
 */

#ifndef INC_CONSTANTS_H_
#define INC_CONSTANTS_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef enum {
	NO_EDGE = 0,
	A_RISING_EDGE,
	A_FALLING_EDGE,
	B_RISING_EDGE,
	B_FALLING_EDGE
} SignalEdge;

typedef enum {
	FALSE = 0,
	TRUE
} Bool;

#endif /* INC_CONSTANTS_H_ */
