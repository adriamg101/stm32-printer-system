/*
 * data_visualizer.h
 *
 *  Created on: 19 jun 2026
 *      Author: adria.mg & aleix.rovira
 */

#ifndef INC_DATA_VISUALIZER_H_
#define INC_DATA_VISUALIZER_H_

#include "constants.h"
#include "lcd_helper.h"

void DVIS_ClearSamples();
void DVIS_SetSamples(uint16_t* data, uint8_t up_nDown);
void DVIS_Init();

#endif /* INC_DATA_VISUALIZER_H_ */
