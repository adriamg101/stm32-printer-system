/*
 * sensor_handler.c
 *
 *  Created on: 14 jun 2026
 *      Author: adria.mg & aleix.rovira
 */

#include "sensor_handler.h"

/*
 * Els ADCs estan organitzats de la següent manera:
 * - ADC1: Front Sensor
 * - ADC2: Back Sensor
 */

// Front Sensor
static ADC_HandleTypeDef* sen_front_hadc;
static uint16_t FrontSensor_ORG[SEN_NUM_SAMPLES];
static uint16_t FrontSensorProc_ORG[SEN_NUM_AVERAGES];	// En um
static ExtraSensorData FrontSensorExtra;

// Rear Sensor
static ADC_HandleTypeDef* sen_rear_hadc;
static uint16_t RearSensor_ORG[SEN_NUM_SAMPLES];
static uint16_t RearSensorProc_ORG[SEN_NUM_AVERAGES];	// En um
static ExtraSensorData RearSensorExtra;

static SignalEdge starting_edge;

uint16_t _SEP_ADC_value_to_distance(uint16_t val) {
	// Per reduir petites inconsistencies dels ADCs als limits
	if (val < SEN_ADC_MIN) val = SEN_ADC_MIN;
	else if (val > SEN_ADC_MAX) val = SEN_ADC_MAX;

	// Calculem la distancia
	return (uint16_t)((((uint32_t)val - SEN_ADC_MIN) * SEN_MAX_DIST) / SEN_ADC_RANGE);
}

void _SEN_CheckMinMax(ExtraSensorData* sensor_extra, uint16_t val) {
	uint16_t dist = _SEP_ADC_value_to_distance(val);
	// Mirem max
	if (dist > sensor_extra->max) sensor_extra->max = dist;
	// Mirem min
	if (dist < sensor_extra->min) sensor_extra->min = dist;
}

#define STARTING_AVERAGE_POS  1 // Comencem en el segon perque el primer valor ha de fer mitjana amb nomes 0, 1 i 2
#define MAX_AVERAGE_POS_COUNT 10
#define AVERAGE_CALCULATE_POS 5

#define AVERAGE_SAMPLES_FIRST_LAST	3
#define AVERAGE_SAMPLES_MIDDLE		5

void _SEN_calcAverage(
		uint16_t* SensorProc_ORG,
		ExtraSensorData* sensor_extra,
		uint16_t val,
		uint16_t* average_pos,
		uint8_t* average_pos_count,
		uint32_t* extra_average) {
	uint16_t dist = _SEP_ADC_value_to_distance(val);

	if (++(*average_pos_count) <= AVERAGE_CALCULATE_POS)
		// Augmentem el average_sim
		SensorProc_ORG[(*average_pos)] += dist;
	else if ((*average_pos_count) >= MAX_AVERAGE_POS_COUNT) {
		// Calculem average a l'array hem de mirar que no siguin ni la primera ni la ultima posicio
		uint16_t div = (((*average_pos) != 0 && (*average_pos) != (SEN_NUM_AVERAGES - 1)) ? AVERAGE_SAMPLES_MIDDLE : AVERAGE_SAMPLES_FIRST_LAST);
		SensorProc_ORG[(*average_pos)] = SensorProc_ORG[(*average_pos)] / div;
		(*average_pos_count) = 0;
		(*average_pos)++;
	}

	// Calculem el average total
	(*extra_average) += dist;
}

#define _SEN_processFrontSamples()	_SEN_processSamples(FrontSensor_ORG, FrontSensorProc_ORG, &FrontSensorExtra)
#define _SEN_processRearSamples()	_SEN_processSamples(RearSensor_ORG, RearSensorProc_ORG, &RearSensorExtra)
// Funcio general de processament de Samples
void _SEN_processSamples(
		uint16_t* Sensor_ORG,
		uint16_t* SensorProc_ORG,
		ExtraSensorData* sensor_extra) {
	uint16_t average_pos = 0;		// Punter de l'array de SensorProc
	uint8_t average_pos_count = 2;	// Comenca a dos perque la primera posicio fa la mitjana
									// amb nomes tres valors.
	uint32_t extra_average = 0;		// Calcul de l'average de el sensor extra

	// Inicialitzem variables de extra
	sensor_extra->average = 0;
	sensor_extra->min = 0xFFFF;
	sensor_extra->max = 0x0000;

	// S'ha de mirar tot l'array
	for (uint16_t i = 0; i < SEN_NUM_SAMPLES; i++) {
		// Calculem l'average si es necessari
		_SEN_calcAverage(SensorProc_ORG, sensor_extra, Sensor_ORG[i], &average_pos, &average_pos_count, &extra_average);
		// Calculem el minim i el maxim
		_SEN_CheckMinMax(sensor_extra, Sensor_ORG[i]);
	}

	sensor_extra->average = extra_average / SEN_NUM_SAMPLES;
	average_pos += 0;
}

void SEN_setStartingEdge(SignalEdge edge) { starting_edge = edge; }

// Funcio que processa els samples dels sensors
void SEN_processADCSamples(ADC_TypeDef* adc) {
	if (adc == ADC1) {
		_SEN_processFrontSamples();
		DVIS_SetSamples(FrontSensorProc_ORG, 1);
	} else {
		_SEN_processRearSamples();
		DVIS_SetSamples(RearSensorProc_ORG, 0);
	}
}

void SEN_takeADCSample(SignalEdge edge) {
	// Mirem que haguem de prendre mesura
	if (edge != starting_edge) return;

	// Activem els ADCs
	SET_BIT(sen_front_hadc->Instance->CR2, ADC_CR2_SWSTART);
	SET_BIT(sen_rear_hadc->Instance->CR2, ADC_CR2_SWSTART);
}

void SEN_prepareADCs() {
	// Preparem els ADCs per a una nova lectura
	HAL_ADC_Start_DMA(sen_front_hadc, (uint32_t*)FrontSensor_ORG, SEN_NUM_SAMPLES);
	HAL_ADC_Start_DMA(sen_rear_hadc, (uint32_t*)RearSensor_ORG, SEN_NUM_SAMPLES);

	starting_edge = NO_EDGE;
	DVIS_ClearSamples();
}

void SEN_stopADCs() {
	// Aturem els ADCs
	HAL_ADC_Stop_DMA(sen_front_hadc);
	HAL_ADC_Stop_DMA(sen_rear_hadc);
}

void SEN_init(ADC_HandleTypeDef* front_hadc, ADC_HandleTypeDef* back_hadc) {
	sen_front_hadc = front_hadc;
	sen_rear_hadc = back_hadc;
}
