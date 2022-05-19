/**
 * @file can_wrapper.c
 * @author Subgrupo Control y Periféricos - Elektron Motorsports
 * @brief Implementaciones funciones wrapper CAN para tarjeta de Control
 * @version 0.1
 * @date 2022-18-05
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

/* STM32 CAN Tx message header structure instance */
static CAN_TxHeaderTypeDef TxHeader;

/* STM32 CAN Rx message header structure definition */
static CAN_RxHeaderTypeDef RxHeader;

/* STM32 CAN filter configuration structure instance */
static CAN_FilterTypeDef sFilterConfig;

/***********************************************************************************************************************
 * Private functions prototypes
 **********************************************************************************************************************/

static void CAN_FilterConfig(void);

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
	/**
	 *  STM32 CAN initialization
	 */

	/* Initialize time base timer for CAN triggering */
	MX_TIM7_Init();

	/* Initialize CAN1 */
	MX_CAN1_Init();

	/* Disable debug freeze */
	CAN1->MCR &= (~CAN_MCR_DBF);

	/* CAN filter configuration */
	CAN_FilterConfig();

	/* Start CAN module */
	if (HAL_CAN_Start(&hcan1) != HAL_OK)
	{
		Error_Handler();
	}

	/* Activate CAN notification (enable interrupts) */
	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		Error_Handler();
	}
	
	/* Start time base trigger CAN timer */
	HAL_TIM_Base_Start_IT(&htim7);

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
	/**
	 *  STM32 CAN transmit message
	 */

    uint32_t TxMailbox;

    /* CAN message transmission configuration */
	TxHeader.StdId = id;            			// standard identifier value
	TxHeader.DLC = dlc; 						// length of frame
	TxHeader.IDE = CAN_ID_STD; 					// type of identifier
	TxHeader.RTR = CAN_RTR_DATA;    			// type of frame
	TxHeader.TransmitGlobalTime = DISABLE;

	/* Start CAN transmission process */
	if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, data, &TxMailbox) != HAL_OK)
	{
		Error_Handler();						// transmission request Error
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
	/**
	 *  STM32 CAN receive message
	 */

	/* Get CAN received message */
    HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, data);

    /* Received standard identifier */
    *id = RxHeader.StdId;

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
 * @brief CAN Filter Configuration Function
 *
 * @param None
 * @retval None
 */
static void CAN_FilterConfig(void)
{
	/**
	 *  STM32 CAN filter configuration
	 */

	/* CAN filter configuration structure */
	sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	sFilterConfig.FilterBank = 0;							// CAN 1 [0..13]
	sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;

	sFilterConfig.FilterIdHigh = 0;							// msg_id << 5;
	sFilterConfig.FilterIdLow = 0;
	sFilterConfig.FilterMaskIdHigh = 0; 					// msg_id << 5;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.SlaveStartFilterBank = 20;

	/* Configure CAN filter */
	if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig)!= HAL_OK)
	{
		Error_Handler();
	}
}
