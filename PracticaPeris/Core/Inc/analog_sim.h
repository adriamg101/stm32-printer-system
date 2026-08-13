/*
 * analog_sim.h
 *
 *  Created on: 14 jun 2026
 *      Author: adria
 */

#ifndef INC_ANALOG_SIM_H_
#define INC_ANALOG_SIM_H_

#include "constants.h"

/*
 * Range of values:
 * - Min: 0 um	(0mm)
 * - Max: 2000 um (2mm)
 */
#define ASIM_UM_DIST 1000	// distancia en um que es vol testejar

void ASIM_init(DAC_HandleTypeDef* hdac, uint32_t channel);

#endif /* INC_ANALOG_SIM_H_ */
