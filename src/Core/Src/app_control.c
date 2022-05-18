/**
 * @file app_control.c
 * @author Subgrupo Control y Periféricos - Elektron Motorsports
 * @brief Código aplicativo de tarjeta Control
 * @version 0.1
 * @date 2022-05-05
 *
 * @copyright Copyright (c) 2022
 *
 */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/

#include "app_control.h"

#include "buses.h"
#include "decode_data.h"
#include "driving_modes.h"
#include "failures.h"
#include "rampa_pedal.h"
#include "monitoring.h"
#include "indicators.h"
#include "can_app.h"

#include "main.h"

/***********************************************************************************************************************
 * Private macros
 **********************************************************************************************************************/

/** @brief Duración del echo en ms */
#define ECHO_LENGTH         1000U

/** @brief Duración de la espera hasta timeout en ms */
#define TIMEOUT_VALUE       5000U

/***********************************************************************************************************************
 * Private variables definitions
 **********************************************************************************************************************/

/** @brief Estados posibles de la máquina de estados */
enum App_States
{
	kWAITING_ECHO_RESPONSE = 0,      	/**< Estado WAITING_ECHO_RESPONSE */
    kRUNNING,       					/**< Estado RUNNING */
};

/** @brief Estado de la máquina de estados */
static uint8_t app_state = kWAITING_ECHO_RESPONSE;

/** @brief  Para conteo de timeout en estado kWAITING_ECHO_RESPONSE */
static uint32_t tickstart;

/** @brief  Para timeout de echo en estado kWAITING_ECHO_RESPONSE */
static bool time_out = false;

/***********************************************************************************************************************
 * Private functions prototypes
 **********************************************************************************************************************/

static void MX_APP_Send_Echo(typedef_bus2_t* bus_can_output);

/***********************************************************************************************************************
 * Public functions implementation
 **********************************************************************************************************************/

void MX_APP_Init(void)
{
    /* Initialize board LEDs */
    BSP_LED_Init(LED1);
    BSP_LED_Init(LED2);
    BSP_LED_Init(LED3);

    /* Initialize board buzzer */
    BSP_BUZZER_Init();

    /* Initialize hardware */
    CAN_HW_Init();

    /* Indicate that initialization was completed */
    for(int i=0; i<3; i++)
    {
    	BSP_LED_On(LED1);
    	BSP_LED_On(LED2);
    	BSP_LED_On(LED3);

    	HAL_Delay(200);

    	BSP_LED_Off(LED1);
    	BSP_LED_Off(LED2);
    	BSP_LED_Off(LED3);

    	HAL_Delay(200);
    }
}

void MX_APP_Process(void)
{
	switch (app_state)
	{

	/* Estado esperando respuesta ECHO a Tarjetas: BMS, DCDC, Inversor, Perifericos */
	case kWAITING_ECHO_RESPONSE:

		/* Envía echo a demás tarjetas */
		//MX_APP_Send_Echo(&bus_can_output);

		/* Get ticks for counting of timeout */
		tickstart = HAL_GetTick();

		while(1)
		{
			/* LEDs para indicar confirmación de módulos */
			INDICATORS_Update_ModulesLEDs();

			if( (HAL_GetTick() - tickstart) > TIMEOUT_VALUE )
			{
				time_out = true;
			}

			/* Si todos los módulos OK, control está listo */
			if (bus_can_input.bms_ok == CAN_VALUE_MODULE_OK &&
                bus_can_input.dcdc_ok == CAN_VALUE_MODULE_OK &&
                bus_can_input.inversor_ok == CAN_VALUE_MODULE_OK &&
                bus_can_input.perifericos_ok == CAN_VALUE_MODULE_OK)
			{
				HAL_Delay(500);

				/* Send control_ok MODULE_OK response */
				bus_can_output.control_ok = CAN_VALUE_MODULE_OK;
				//CAN_APP_Send_BusData(&bus_can_output);

				/* Indicate that start up has finished */
				INDICATORS_Finish_StartUp();

				app_state = kRUNNING;

				break;
			}
			else if(time_out)
			{
				//MX_APP_Send_Echo(&bus_can_output);

				tickstart = HAL_GetTick();

				time_out = false;
			}
		}
		break;

	/* Estado Tarjeta de Control running */
	case kRUNNING:

		DECODE_DATA_Process();

		DRIVING_MODES_Process();

		FAILURES_Process();

		MONITORING_Process();

		RAMPA_PEDAL_Process();

	    //INDICATORS_Process();

	    //CAN_APP_Process();

		break;
	}
}

/***********************************************************************************************************************
 * Private functions implementation
 **********************************************************************************************************************/

static void MX_APP_Send_Echo(typedef_bus2_t* bus_can_output)
{
	bus_can_output->control_ok = CAN_VALUE_MODULE_OK;

	CAN_APP_Send_BusData(bus_can_output);

	HAL_Delay(ECHO_LENGTH);

	bus_can_output->control_ok = CAN_VALUE_MODULE_IDLE;

	CAN_APP_Send_BusData(bus_can_output);
}
