/**
 * @file can_wrapper.c
 * @author Subgrupo Control y Periféricos - Elektron Motorsports
 * @brief Implementaciones funciones wrapper CAN para tarjeta de Control
 * @version 0.1
 * @date 2022-01-05
 *
 * @copyright Copyright (c) 2022
 *
 */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/

#include "can_wrapper.h"

/***********************************************************************************************************************
 * Private variables definitions
 **********************************************************************************************************************/

/** @brief CAN handle structure instance */
static CAN_HandleTypeDef hcan;

/** @brief CAN filter configuration structure instance */
static CAN_FilterTypeDef aFilterConfig;

/** @brief CAN Tx message header structure instance */
static CAN_TxHeaderTypeDef txHeader;

/** @brief CAN Rx message header structure definition */
static CAN_RxHeaderTypeDef rxHeader;

/***********************************************************************************************************************
 * Private functions prototypes
 **********************************************************************************************************************/

static void CAN_Init(void);

static void CAN_FilterConfig(void);

static void CAN_ActivateNotification(void);

static void CAN_Start(void);

/***********************************************************************************************************************
 * Public functions implementation
 **********************************************************************************************************************/

/**
 * @brief Función wrapper inicialización de periférico CAN.
 *
 * No es static, por lo que puede ser usada por otros archivos.
 *
 * @param   None
 * @retval  None
 */
can_status_t CAN_Wrapper_Init(void)
{
    CAN_Init();					// CAN1 peripheral initialization

    CAN_FilterConfig();    		// CAN1 filter configuration

	CAN_ActivateNotification();	// CAN1 enable interrupt

	CAN_Start();					// CAN1 start the CAN module
	
	return CAN_STATUS_OK;
}

/**
 * @brief Función wrapper transmisión de datos CAN.
 *
 * No es static, por lo que puede ser usada por otros archivos.
 *
 * @param id Standard identifier
 * @param ide Type of identifier
 * @param rtr Type of frame
 * @param dlc Length of frame
 * @param data Data to transmit
 * @retval None
 */
can_status_t CAN_Wrapper_TransmitData(uint32_t id, uint8_t ide, uint8_t rtr, uint8_t dlc, uint8_t *data)
{
    uint32_t mailbox;

	txHeader.StdId = id;            // standard identifier value
	txHeader.DLC = dlc; 			// length of frame
	txHeader.IDE = CAN_ID_STD; 		// type of identifier
	txHeader.RTR = CAN_RTR_DATA;    // data frame

	HAL_CAN_AddTxMessage(&hcan, &txHeader, data, &mailbox);

	if(mailbox == CAN_TX_MAILBOX2)
    {
		while( HAL_CAN_IsTxMessagePending(&hcan, CAN_TX_MAILBOX0));
	}

	return CAN_STATUS_OK;
}

/**
 * @brief Función wrapper recepción de datos CAN.
 *
 * No es static, por lo que puede ser usada por otros archivos.
 *
 * @param id Received identifier
 * @param data Received data
 * @retval  None
 */
can_status_t CAN_Wrapper_ReceiveData(uint32_t *id, uint8_t *data)
{
    HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &rxHeader, data);

    *id = rxHeader.StdId;	// received standard identifier

	return CAN_STATUS_OK;
}

/**
 * @brief Función wrapper conteo dato recibido por CAN.
 *
 * No es static, por lo que puede ser usada por otros archivos.
 *
 * @return can_status_t
 */
can_status_t CAN_Wrapper_DataCount(void)
{
	return CAN_STATUS_OK;
}

/***********************************************************************************************************************
 * Private functions implementation
 **********************************************************************************************************************/

/**
 * @brief STM32 CAN1 Initialization Function.
 *
 * @param None
 * @retval None
 */
static void CAN_Init(void)
{
	hcan.Instance = CAN1;
	hcan.Init.Prescaler = 16;
	hcan.Init.Mode = CAN_MODE_NORMAL;
	hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan.Init.TimeSeg1 = CAN_BS1_1TQ;
	hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
	hcan.Init.TimeTriggeredMode = DISABLE;
	hcan.Init.AutoBusOff = DISABLE;
	hcan.Init.AutoWakeUp = DISABLE;
	hcan.Init.AutoRetransmission = DISABLE;
	hcan.Init.ReceiveFifoLocked = DISABLE;
	hcan.Init.TransmitFifoPriority = DISABLE;
	if (HAL_CAN_Init(&hcan) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief STM32 CAN1 Filter Configuration Function.
 *
 * @param None
 * @retval None
 */
static void CAN_FilterConfig(void)
{
	aFilterConfig.FilterBank = 0;
	aFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	aFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	aFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	aFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
	aFilterConfig.SlaveStartFilterBank = 14;

	//aFilterConfig.FilterIdHigh = ID_PEDAL<<5;
	aFilterConfig.FilterIdLow = 0x000;
	//aFilterConfig.FilterMaskIdHigh = ID_PEDAL<<5;
	aFilterConfig.FilterMaskIdLow = 0x000;

	HAL_CAN_ConfigFilter(&hcan, &aFilterConfig);

	aFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	aFilterConfig.FilterBank = 1;
	aFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	aFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
	aFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
	//aFilterConfig.FilterIdHigh = ID_CORRIENTE_BMS<<5;
	//aFilterConfig.FilterIdLow = ID_POTENCIA_BMS<<5;
	//aFilterConfig.FilterMaskIdHigh = ID_NIVEL_BATERIA_BMS<<5;
	//aFilterConfig.FilterMaskIdLow = ID_VELOCIDAD_INV<<5;
	//aFilterConfig.SlaveStartFilterBank = 14;

	HAL_CAN_ConfigFilter(&hcan, &aFilterConfig);
}

/**
 * @brief STM32 CAN1 Enable Interrupt
 *
 * @param None
 * @retval None
 */
static void CAN_ActivateNotification(void)
{
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

/**
 * @brief STM32 CAN1 Start the CAN Module
 *
 * @param None
 * @retval None
 */
static void CAN_Start(void)
{
	HAL_CAN_Start(&hcan);
}

