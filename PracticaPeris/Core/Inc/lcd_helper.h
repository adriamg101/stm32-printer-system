/*
 * data_visualizer.h
 *
 *  Created on: 17 jun 2026
 *      Author: adria.mg & aleix.rovira
 */

#ifndef INC_LCD_HELPER_H_
#define INC_LCD_HELPER_H_

#include "constants.h"

#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"

// Zona de data
#define LH_Data 10
#define RH_Data 309
#define UV_UData 237
#define DV_UData 137
#define UV_DData 132
#define DV_DData 32

typedef enum {
 NO_OK = 0 ,
 OK = !NO_OK
} RetSt ;

// Rutines de visualització
RetSt SetPixel (uint16_t col, uint16_t row, uint8_t alpha, uint8_t Rval, uint8_t Gval, uint8_t Bval );
RetSt DibuixaLiniaHoritzontal (uint16_t col_inici, uint16_t col_fi, uint16_t row, uint8_t alpha, uint8_t Rval, uint8_t Gval, uint8_t Bval );
RetSt DibuixaLiniaVertical (uint16_t col, uint16_t row_inici, uint16_t row_fi, uint8_t alpha, uint8_t Rval, uint8_t Gval, uint8_t Bval );
RetSt EsborraDades (uint8_t alpha, uint8_t Rval, uint8_t Gval, uint8_t Bval );
RetSt EsborraPantalla (uint8_t alpha, uint8_t Rval, uint8_t Gval, uint8_t Bval );

// Funcio d'inicialització del LCD
void LCDH_Init();

#endif /* INC_LCD_HELPER_H_ */
