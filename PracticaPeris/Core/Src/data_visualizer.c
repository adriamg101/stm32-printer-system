/*
 * data_visualizer.c
 *
 *  Created on: 19 jun 2026
 *      Author: adria.mg & aleix.rovira
 */

#include "data_visualizer.h"

// Chart lines:
// L: Left
// R: Right
// U: Up
// D: Down
// H: Linies Horizontals
// V: Linies Verticals
#define LV_Chart_LINE 9
#define RV_Chart_LINE 310
#define LV_LSample_LINE 52
#define RV_LSample_LINE 137
#define LV_RSample_LINE 182
#define RV_RSample_LINE 267

#define UH_UChart_LINE 238
#define DH_UChart_LINE 136
#define UH_DChart_LINE 133
#define DH_DChart_LINE 31
#define UH_Sample_LINE 28
#define DH_Sample_LINE 3

#define V_Data_SIZE 101
#define H_Data_SIZE 300

// Dibuixa la grafica sense dades, nomes linies i colors de front i back sensor
void _DVIS_setChart() {
	// Linies verticals chart
	DibuixaLiniaVertical(LV_Chart_LINE, DH_UChart_LINE, UH_UChart_LINE, 255, 0, 0, 0);
	DibuixaLiniaVertical(LV_Chart_LINE, DH_DChart_LINE, UH_DChart_LINE, 255, 0, 0, 0);
	DibuixaLiniaVertical(RV_Chart_LINE, DH_UChart_LINE, UH_UChart_LINE, 255, 0, 0, 0);
	DibuixaLiniaVertical(RV_Chart_LINE, DH_DChart_LINE, UH_DChart_LINE, 255, 0, 0, 0);

	// Linies verticals samples
	DibuixaLiniaVertical(LV_LSample_LINE, DH_Sample_LINE, UH_Sample_LINE, 255, 0, 0, 0);
	DibuixaLiniaVertical(RV_LSample_LINE, DH_Sample_LINE, UH_Sample_LINE, 255, 0, 0, 0);
	DibuixaLiniaVertical(LV_RSample_LINE, DH_Sample_LINE, UH_Sample_LINE, 255, 0, 0, 0);
	DibuixaLiniaVertical(RV_RSample_LINE, DH_Sample_LINE, UH_Sample_LINE, 255, 0, 0, 0);

	// Linies horitzontals charT
	DibuixaLiniaHoritzontal(LV_Chart_LINE, RV_Chart_LINE, UH_UChart_LINE, 255, 0, 0, 0);
	DibuixaLiniaHoritzontal(LV_Chart_LINE, RV_Chart_LINE, DH_UChart_LINE, 255, 0, 0, 0);
	DibuixaLiniaHoritzontal(LV_Chart_LINE, RV_Chart_LINE, UH_DChart_LINE, 255, 0, 0, 0);
	DibuixaLiniaHoritzontal(LV_Chart_LINE, RV_Chart_LINE, DH_DChart_LINE, 255, 0, 0, 0);

	// Linies horitzontals samples
	DibuixaLiniaHoritzontal(LV_LSample_LINE, RV_LSample_LINE, UH_Sample_LINE, 255, 0, 0, 0);
	DibuixaLiniaHoritzontal(LV_LSample_LINE, RV_LSample_LINE, DH_Sample_LINE, 255, 0, 0, 0);
	DibuixaLiniaHoritzontal(LV_RSample_LINE, RV_RSample_LINE, UH_Sample_LINE, 255, 0, 0, 0);
	DibuixaLiniaHoritzontal(LV_RSample_LINE, RV_RSample_LINE, DH_Sample_LINE, 255, 0, 0, 0);

	// Sample colors fill
	// Esquerra (red)
	for (uint16_t col = LV_LSample_LINE+1; col < RV_LSample_LINE; col++)
		DibuixaLiniaVertical(col, DH_Sample_LINE+1, UH_Sample_LINE-1, 255, 255, 0, 0);
	// Dreta (blue)
	for (uint16_t col = LV_RSample_LINE+1; col < RV_RSample_LINE; col++)
		DibuixaLiniaVertical(col, DH_Sample_LINE+1, UH_Sample_LINE-1, 255, 0, 0, 255);
}

// Al començar natejem les dades
void DVIS_ClearSamples() {
	EsborraDades(255, 255, 255, 255);
}

// S'ha de pasar tot l'array (up_nDown)
void DVIS_SetSamples(uint16_t* data, uint8_t up_nDown) {
	uint16_t V_min_data = ((up_nDown) ? DH_UChart_LINE : DH_DChart_LINE) + 1;
	uint16_t H_min_data = LV_Chart_LINE + 1;
	uint8_t red = (up_nDown ? 255 : 0);		// up
	uint8_t blue = (up_nDown ? 0 : 255);	// down

	// Setejem tot l'array de cop
	for (uint16_t i = 0; i < H_Data_SIZE; i++) {
		uint16_t row = V_min_data + ((data[i] != 3000) ? (data[i] / 20) : 101);
		// Si es 2000, posem 101
		SetPixel((H_min_data + i) , row, 255, red, 0, blue);
	}
}

// Inicialitza la matriu
void DVIS_Init() {
    // Inicialitzem la pantalla en blanc
    EsborraPantalla(255, 255, 255, 255);
    _DVIS_setChart();
}
