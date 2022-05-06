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
#include "monitoring.h"
#include "indicators.h"
#include "can_app.h"

#include "main.h"

/***********************************************************************************************************************
 * Private macros
 **********************************************************************************************************************/

/** Duración de blink de LED de estado kWAITING_ECHO */
#define BLINK_TIME              250U

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
uint8_t app_state = kWAITING_ECHO_RESPONSE;

/** Para blink LED de estado kWAITING_ECHO */
uint32_t blink_tickstart;

/***********************************************************************************************************************
 * Private functions prototypes
 **********************************************************************************************************************/

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

		/* Ticks for the blinking of the LED */
		blink_tickstart = HAL_GetTick();

		while(1)
		{
			/* LEDs para indicar confirmación de módulos */
			INDICATORS_Update_ModulesLEDs();

			/* Send ECHO */

			if (bus_can_input.bms_ok == CAN_VALUE_MODULE_OK &&
                bus_can_input.dcdc_ok == CAN_VALUE_MODULE_OK &&
                bus_can_input.inversor_ok == CAN_VALUE_MODULE_OK &&
                bus_can_input.perifericos_ok == CAN_VALUE_MODULE_OK)
			{
				HAL_Delay(500);

				/* Send perifericos_ok MODULE_OK response */
				bus_can_output.control_ok = CAN_VALUE_MODULE_OK;
				CAN_APP_Send_BusData(&bus_can_output);

				/* Indicate that start up has finished */
				INDICATORS_Finish_StartUp();

				app_state = kRUNNING;
			}
		}
		break;

	/* Estado Tarjeta de Control running */
	case kRUNNING:

		DECODE_DATA_Process();

		DRIVING_MODES_SM_Process();

		FAILURES_SM_Process();

		MONITORING_Process();

		RAMPA_PEDAL_Process();

	    INDICATORS_Process();

	    CAN_APP_Process();

		break;

	default:
		break;
	}
}
