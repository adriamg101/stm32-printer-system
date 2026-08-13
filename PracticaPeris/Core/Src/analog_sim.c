/*
 * analog_sim.c
 *
 *  Created on: 14 jun 2026
 *      Author: adria.mg & aleix.rovira
 */

#include "analog_sim.h"

#include "sensor_handler.h"

static DAC_HandleTypeDef* asim_hdac;
static uint32_t asim_ch;

#define _ASIM_getDACValueFromVoltage(dist) ((uint32_t)(SEN_ADC_MIN + ((dist) * SEN_ADC_RANGE) / SEN_MAX_DIST))

void ASIM_init(DAC_HandleTypeDef* hdac, uint32_t channel) {
	asim_hdac = hdac;
	asim_ch = channel;

	// Setejem el DAC per a la resta de la execució
	HAL_DAC_Start(asim_hdac, asim_ch);
	HAL_DAC_SetValue(asim_hdac, asim_ch, DAC_ALIGN_12B_R, _ASIM_getDACValueFromVoltage(ASIM_UM_DIST));
}
