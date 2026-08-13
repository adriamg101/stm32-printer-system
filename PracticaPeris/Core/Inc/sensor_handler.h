/*
 * sensor_handler.h
 *
 *  Created on: 14 jun 2026
 *      Author: adria.mg & aleix.rovira
 */

#ifndef INC_SENSOR_HANDLER_H_
#define INC_SENSOR_HANDLER_H_

#include "constants.h"
#include "data_visualizer.h"

#define SEN_NUM_SAMPLES	 3000
#define SEN_NUM_AVERAGES 300

/*
 * Formula: (voltage * 4095(12bits 0x0FFF)) / 3.3 (Vref+)
 */
#define SEN_ADC_MIN		155	 // 0 mm
#define SEN_ADC_MAX		2948 // 2 mm
#define SEN_ADC_RANGE	(SEN_ADC_MAX - SEN_ADC_MIN)
#define SEN_MAX_DIST	2000 // um = 2 mm

typedef struct {
	uint16_t average;
	uint16_t max;
	uint16_t min;
} ExtraSensorData;

void SEN_processADCSamples(ADC_TypeDef* adc);
void SEN_setStartingEdge(SignalEdge edge);
void SEN_takeADCSample(SignalEdge edge);
void SEN_prepareADCs();
void SEN_stopADCs();

void SEN_init(ADC_HandleTypeDef* front_hadc, ADC_HandleTypeDef* back_hadc);

#endif /* INC_SENSOR_HANDLER_H_ */
