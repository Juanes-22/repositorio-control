/**
 * @file can_hw.c
 * @author Subgrupo Control y Periféricos - Elektron Motorsports
 * @brief Configuración hardware CAN de tarjeta Control
 * @version 0.1
 * @date 2022-05-05
 *
 * @copyright Copyright (c) 2022
 *
 */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/

#include "can_hw.h"

/***********************************************************************************************************************
 * Private macros
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private variables definitions
 **********************************************************************************************************************/

/** @brief CAN object instance */
CAN_t can_obj;

/** @brief Bandera mensaje recibido CAN */
can_rx_status_t flag_rx_can = CAN_MSG_NOT_RECEIVED;

/** @brief Bandera transmisión CAN */
can_tx_status_t flag_tx_can = CAN_TX_READY;

/** @brief ID para prueba comunicación CAN */
uint8_t test_msg_id = 0x30;

int i = 0;

/***********************************************************************************************************************
 * Private functions prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Public functions implementation
 **********************************************************************************************************************/

void CAN_HW_Init(void)
{
	/* Inicializa CAN usando driver */
	CAN_API_Init(&can_obj,
				 STANDARD_FRAME,
				 NORMAL_MSG,
				 CAN_Wrapper_Init,
				 CAN_Wrapper_TransmitData,
				 CAN_Wrapper_ReceiveData,
				 CAN_Wrapper_DataCount);
}

/***********************************************************************************************************************
 * Exported functions implementation
 **********************************************************************************************************************/

/*
 *  Callback mensaje CAN recibido
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
    /* The flag indicates that the callback was called */
    flag_rx_can = CAN_MSG_RECEIVED;

    /* Get the received message */
	if(CAN_API_Read_Message(&can_obj) != CAN_STATUS_OK)
	{
		Error_Handler();
	}
}

/*
 * Callback timer trigger de transmisión de datos de bus de salida CAN a módulo CAN
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	i++;

	if(htim == &htim7 && i>5)
	{
		/* Toggle LED 1 (Red LED) */
		BSP_LED_Toggle(LED1);

		/* The flag indicates that the callback was called */
		flag_tx_can = CAN_TX_READY;

		/* Transmit test message */
		can_obj.Frame.id = test_msg_id;
		can_obj.Frame.payload_length = 1;
		can_obj.Frame.payload_buff[0] = (char)(i & 0xff);

		if(CAN_API_Send_Message(&can_obj) != CAN_STATUS_OK)
		{
			Error_Handler();
		}
	}
}

/***********************************************************************************************************************
 * Private functions implementation
 **********************************************************************************************************************/
