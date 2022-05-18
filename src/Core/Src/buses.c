/**
 * @file buses.c
 * @author Subgrupo Control y Periféricos - Elektron Motorsports
 * @brief Buses arquitectura de firmware de Control
 * @version 0.1
 * @date 2022-05-05
 *
 * @copyright Copyright (c) 2022
 *
 */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/

#include "buses.h"

/***********************************************************************************************************************
 * Buses initialization
 **********************************************************************************************************************/

/* Inicialización de bus de datos (bus 1) */
typedef_bus1_t bus_data =
{
		/* Variable modo de manejo */
		.driving_mode = kDRIVING_MODE_NORMAL,

		/* Variable falla */
		.failure = kFAILURE_CAUTION1,

		/* Variable velocidad */
		.velocidad_inversor = 0U,

		/* Estructura de periféricos */
		.Rx_Peripherals = 
		{
			.botones_cambio_estado = kBTN_NONE, 
			.hombre_muerto = kHOMBRE_MUERTO_OFF, 
			.perifericos_ok = kMODULE_INFO_OK
		},

		/* Estructuras de BMS */
		.Rx_Bms = {.bms_ok = kMODULE_INFO_OK},
		.St_Bms = {kVAR_STATE_DATA_PROBLEM},

		/* Estructuras de DCDC */
		.Rx_Dcdc = {.dcdc_ok = kMODULE_INFO_OK},
		.St_Dcdc = {kVAR_STATE_DATA_PROBLEM},

		/* Estructuras de inversor */
		.Rx_Inversor = {.inversor_ok = kMODULE_INFO_OK},
		.St_Inversor = {kVAR_STATE_DATA_PROBLEM},

		/* Variables estado general de cada módulo */
		.bms_status = kMODULE_STATUS_DATA_PROBLEM,
		.dcdc_status = kMODULE_STATUS_DATA_PROBLEM,
		.inversor_status = kMODULE_STATUS_DATA_PROBLEM,
};

/* Inicialización de bus de salida CAN (bus 2) */
typedef_bus2_t bus_can_output = 
{
	.autokill = CAN_VALUE_AUTOKILL_OFF,
	.estado_manejo = CAN_VALUE_DRIVING_MODE_NORMAL,
	.estado_falla = CAN_VALUE_FAILURE_CAUTION1,
	.nivel_velocidad = 0U,
	.boost_enable = CAN_VALUE_BOOST_OFF,
	.control_ok = CAN_VALUE_MODULE_IDLE
};

/* Inicialización de bus de recepción CAN (bus 3) */
typedef_bus3_t bus_can_input =
{
	.bms_ok = CAN_VALUE_MODULE_OK,
	.dcdc_ok = CAN_VALUE_MODULE_OK,
	.inversor_ok = CAN_VALUE_MODULE_OK,
	.perifericos_ok = CAN_VALUE_MODULE_OK
};
