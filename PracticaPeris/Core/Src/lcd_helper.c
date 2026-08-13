/*
 * data_visualizer.c
 *
 *  Created on: 17 jun 2026
 *      Author: adria.mg & aleix.rovira
 */

#include "lcd_helper.h"
#include <string.h>

#define NUM_COL BSP_LCD_GetYSize()
#define NUM_ROW BSP_LCD_GetXSize()

#define FRAME_BUFFER_LCD 0xD0000000U

extern LTDC_HandleTypeDef LtdcHandler;

// Funcio que reetorna el valor de ARGB del pixel a partir de les seves parts
uint16_t _convert_ARGBColor(
		uint8_t alpha,
		uint8_t Rval,
		uint8_t Gval,
		uint8_t Bval) {
	uint16_t res = ((uint16_t)alpha << 12) & 0xF000;
	res |= ((uint16_t)Rval << 8) & 0x0F00;
	res |= ((uint16_t)Gval << 4) & 0x00F0;
	res |= (uint16_t)Bval & 0x000F;
	return res;
}

/*
 * Dibuixa el píxel amb coordenades (col, row)-columna i fila, respectivament- amb els
 * valors de les components ARGB (AlphaRedGreenBlue) especificats en els paràmetres
 * alpha, Rval, Gval i Bval, respectivament.
 * Si la rutina es pot executar correctament retorna el valor OK, en cas contrari, retorna el
 * valor NO_OK.
 */
RetSt SetPixel (
		uint16_t col,
		uint16_t row,
		uint8_t alpha,
		uint8_t Rval,
		uint8_t Gval,
		uint8_t Bval) {
	// Fem mirror de col i row
	col = NUM_COL - 1 - col;
	row = NUM_ROW - 1 - row;

	// Primer hem de comprobar que la columna i la row siguin correctes
	if (col < NUM_COL && row < NUM_ROW) {
		// Si son correctes escribin en les adreces
		// El 2 es perque son 2 bytes cada color
		*((uint16_t *)FRAME_BUFFER_LCD + ((NUM_ROW * col) + row)) = _convert_ARGBColor(alpha, Rval, Gval, Bval);
		return OK;
	}
	return NO_OK;
}

/*
 * Dibuixa una línia horitzontal entre el píxel amb coordenades (col_inici, row) i el píxel
 * amb coordenades (col_fi, row). No s’estableix cap restricció respecte a si col_inici
 * ha de ser menor o igual que col_fi la rutina haurà de gestionar correctament qualsevol
 * direcció de dibuix.
 * Si la rutina es pot executar correctament retorna el valor OK, en cas contrari, retorna el
 * valor NO_OK.
 */
RetSt DibuixaLiniaHoritzontal (
		uint16_t col_inici,
		uint16_t col_fi,
		uint16_t row,
		uint8_t alpha,
		uint8_t Rval,
		uint8_t Gval,
		uint8_t Bval) {
	// Comprobem que inici es menor que fi
	if (col_fi < col_inici) return NO_OK;

	// Recorrem les columnes
	for (uint16_t col = col_inici; col <= col_fi; col++) {
		if (!SetPixel(col, row, alpha, Rval, Gval, Bval)) return NO_OK;
	}
	return OK;
}

/*
 * Dibuixa una línia vertical entre el píxel amb coordenades (col, row_inici) i el píxel
 * amb coordenades (col, row_fi). No s’estableix cap restricció respecte a si row_inici
 * ha de ser menor o igual que row_fi la rutina haurà de gestionar correctament qualsevol
 * direcció de dibuix.
 * Si la rutina es pot executar correctament retorna el valor OK, en cas contrari, retorna el
 * valor NO_OK.
 */
RetSt DibuixaLiniaVertical (
		uint16_t col,
		uint16_t row_inici,
		uint16_t row_fi,
		uint8_t alpha,
		uint8_t Rval,
		uint8_t Gval,
		uint8_t Bval) {
	// Comprobem que inici es menor que fi
	if (row_fi < row_inici) return NO_OK;

	// Recorrem les files
	for (uint16_t row = row_inici; row <= row_fi; row++) {
		if (!SetPixel(col, row, alpha, Rval, Gval, Bval)) return NO_OK;
	}
	return OK;
}

/*
 * Esborra les dades visualitzades i inicialitza la totalitat dels píxels amb el color especificat.
 * La rutina no esborra els marcs de les finestres de visualització ni els rectangles de
 * identificació del color de les formes d’ona. La implementació d’aquesta rutina depèn de
 * la implementació del sistema que hàgiu decidit (com es fan servir les layers, per
 * exemple).
 * Si la rutina es pot executar correctament retorna el valor OK, en cas contrari, retorna el
 * valor NO_OK.
 */
RetSt EsborraDades (
		uint8_t alpha,
		uint8_t Rval,
		uint8_t Gval,
		uint8_t Bval) {
	// s'ha de esborrar cada rectangle
	for (uint16_t col = LH_Data; col <= RH_Data; col++) {
		// Data de dalt
		if (!DibuixaLiniaVertical(col, DV_UData, UV_UData, alpha, Rval, Gval, Bval)) return NO_OK;
		// Data de baix
		if (!DibuixaLiniaVertical(col, DV_DData, UV_DData, alpha, Rval, Gval, Bval)) return NO_OK;
	}

	return OK;
}

/*
 * Esborra la totalitat de la pantalla i inicialitza la totalitat dels píxels amb el color
 * especificat. La implementació d’aquesta rutina depèn de la implementació del sistema
 * que hàgiu decidit (com es fan servir les layers, per exemple).
 * Si la rutina es pot executar correctament retorna el valor OK, en cas contrari, retorna el
 * valor NO_OK.
 */
RetSt EsborraPantalla (
		uint8_t alpha,
		uint8_t Rval,
		uint8_t Gval,
		uint8_t Bval) {
	for (uint16_t col = 0; col < NUM_COL; col++) {
		if (!DibuixaLiniaVertical(col, 0, NUM_ROW-1, alpha, Rval, Gval, Bval)) return NO_OK;
	}
	return OK;
}

void LCDH_Init(void) {
    BSP_LCD_Init();

    RCC_PeriphCLKInitTypeDef periphClk;
    periphClk.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    periphClk.PLLSAI.PLLSAIN = 192;
    periphClk.PLLSAI.PLLSAIR = 4;
    periphClk.PLLSAIDivR = RCC_PLLSAIDIVR_16;
    HAL_RCCEx_PeriphCLKConfig(&periphClk);

    BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER);
    BSP_LCD_SetLayerVisible(0, ENABLE);
    BSP_LCD_SelectLayer(0);
    BSP_LCD_DisplayOn();
}
