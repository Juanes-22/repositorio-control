/**
 * @file can_app.c
 * @author Subgrupo Control y Periféricos - Elektron Motorsports
 * @brief Codigo aplicativo para envío y recepción CAN
 * @version 0.1
 * @date 2022-05-05
 *
 * @copyright Copyright (c) 2022
 *
 */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/

#include "can_app.h"

/***********************************************************************************************************************
 * Private macros
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private variables definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private functions prototypes
 **********************************************************************************************************************/

static void CAN_APP_Store_ReceivedMessage(void);

/***********************************************************************************************************************
 * Public functions implementation
 **********************************************************************************************************************/

/**
 * @brief Función principal de CAN a nivel de aplicación
 *
 * Guarda mensaje CAN recibido en bus de entrada CAN cuando se activa
 * bandera de recepción. Envía datos de bus de salida CAN cuando se
 * activa bandera de transmisión.
 *
 * No es static, por lo que puede ser usada por otros archivos.
 *
 * @param None
 * @retval None
 */
void CAN_APP_Process(void)
{
	/* Recibió mensaje CAN */
	if(flag_rx_can == CAN_MSG_RECEIVED)
	{
		/* Guarda mensaje CAN recibido en bus de entrada CAN */
		CAN_APP_Store_ReceivedMessage();

		/* Clear CAN received message flag */
		flag_rx_can = CAN_MSG_NOT_RECEIVED;

		/* Activa bandera para decodificar */
		flag_decodificar = DECODIFICA;
	}

	/* Hubo trigger para transmisión mensaje CAN */
	if(flag_tx_can == CAN_TX_READY)
	{
		/* Envío de datos del bus de salida CAN a módulo CAN */
		CAN_APP_Send_BusData(&bus_can_output);

		/* Clear CAN TX ready flag */
		flag_rx_can = CAN_TX_NOT_READY;
	}
}

/**
 * @brief Función de envío de datos de bus de salida CAN a módulo CAN.
 *
 * Realiza el envío de las variables que se encuentran en el bus de salida CAN a módulo CAN.
 *
 * No es static, por lo que puede ser usada por otros archivos.
 *
 * @param bus_can_output Puntero a estructura de tipo typedef_bus2_t (bus de salida CAN)
 * @retval None
 */
void CAN_APP_Send_BusData(typedef_bus2_t *bus_can_output)
{
	/* --------------- Send autokill --------------- */

	can_obj.Frame.id = CAN_ID_AUTOKILL;
	can_obj.Frame.payload_length = 1;
	can_obj.Frame.payload_buff[0] = bus_can_output->autokill;

	if( CAN_API_Send_Message(&can_obj) != CAN_STATUS_OK )
	{
		Error_Handler();
	}

	/* ------------- Send estado_manejo ------------- */

	can_obj.Frame.id = CAN_ID_ESTADO_MANEJO;
	can_obj.Frame.payload_length = 1;
	can_obj.Frame.payload_buff[0] = bus_can_output->estado_manejo;

	if( CAN_API_Send_Message(&can_obj) != CAN_STATUS_OK )
	{
		Error_Handler();
	}

	/* ------------- Send estado_falla ------------- */

	can_obj.Frame.id = CAN_ID_ESTADO_FALLA;
	can_obj.Frame.payload_length = 1;
	can_obj.Frame.payload_buff[0] = bus_can_output->estado_falla;

	if( CAN_API_Send_Message(&can_obj) != CAN_STATUS_OK )
	{
		Error_Handler();
	}

	/* ------------ Send nivel_velocidad ------------ */

	can_obj.Frame.id = CAN_ID_NIVEL_VELOCIDAD;
	can_obj.Frame.payload_length = 1;
	can_obj.Frame.payload_buff[0] = bus_can_output->nivel_velocidad;

	if( CAN_API_Send_Message(&can_obj) != CAN_STATUS_OK )
	{
		Error_Handler();
	}

	/* ------------- Send boost_enable ------------- */

	can_obj.Frame.id = CAN_ID_BOOST_ENABLE;
	can_obj.Frame.payload_length = 1;
	can_obj.Frame.payload_buff[0] = bus_can_output->boost_enable;

	if( CAN_API_Send_Message(&can_obj) != CAN_STATUS_OK )
	{
		Error_Handler();
	}

	/* -------------- Send control_ok -------------- */

	can_obj.Frame.id = CAN_ID_CONTROL_OK;
	can_obj.Frame.payload_length = 1;
	can_obj.Frame.payload_buff[0] = bus_can_output->control_ok;

	if( CAN_API_Send_Message(&can_obj) != CAN_STATUS_OK )
	{
		Error_Handler();
	}
}

/***********************************************************************************************************************
 * Private functions implementation
 **********************************************************************************************************************/

/* Guardar mensaje CAN recibido en bus de entrada CAN */
static void CAN_APP_Store_ReceivedMessage(void)
{
	if(CAN_API_Read_Message(&can_obj) != CAN_STATUS_OK)
	{
		Error_Handler();
	}

	/* Según standard identifier que se recibió, guarda dato en variables de bus de recepción CAN */
	switch(can_obj.Frame.id)
	{

    /* ------------------------------ Periféricos ------------------------------ */

    case CAN_ID_PEDAL:
        bus_can_input.pedal = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_HOMBRE_MUERTO:
        bus_can_input.hombre_muerto = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_BOTONES_CAMBIO_ESTADO:
        bus_can_input.botones_cambio_estado = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_PERIFERICOS_OK:
        bus_can_input.perifericos_ok = can_obj.Frame.payload_buff[0];
        break;

    /* ---------------------------------- BMS ---------------------------------- */

    case CAN_ID_VOLTAJE_BMS:
        bus_can_input.voltaje_bms = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_CORRIENTE_BMS:
        bus_can_input.corriente_bms = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_VOLTAJE_MIN_CELDA_BMS:
        bus_can_input.voltaje_min_celda_bms = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_POTENCIA_BMS:
        bus_can_input.potencia_bms = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_T_MAX_BMS:
        bus_can_input.t_max_bms = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_NIVEL_BATERIA_BMS:
        bus_can_input.nivel_bateria_bms = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_BMS_OK:
        bus_can_input.bms_ok = can_obj.Frame.payload_buff[0];
        break;

    /* --------------------------------- DCDC ---------------------------------- */

    case CAN_ID_VOLTAJE_BATERIA_DCDC:
        bus_can_input.voltaje_bateria_dcdc = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_VOLTAJE_SALIDA_DCDC:
        bus_can_input.voltaje_salida_dcdc = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_T_MAX_DCDC:
        bus_can_input.t_max_dcdc = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_POTENCIA_DCDC:
        bus_can_input.potencia_dcdc = can_obj.Frame.payload_buff[0];
        break;

    /* -------------------------------- Inversor ------------------------------- */

    case CAN_ID_VELOCIDAD_INVERSOR:
        bus_can_input.velocidad_inv = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_V_INVERSOR:
        bus_can_input.V_inv = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_I_INVERSOR:
        bus_can_input.I_inv = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_TEMP_MAX_INVERSOR:
        bus_can_input.temp_max_inv = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_TEMP_MOTOR_INVERSOR:
        bus_can_input.temp_motor_inv = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_POTENCIA_INVERSOR:
        bus_can_input.potencia_inv = can_obj.Frame.payload_buff[0];
        break;
    case CAN_ID_INVERSOR_OK:
        bus_can_input.inversor_ok = can_obj.Frame.payload_buff[0];
        break;
	}
}