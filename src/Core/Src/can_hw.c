/**
 * @file can_hw.c
 * @author Subgrupo Control y Periféricos - Elektron Motorsports
 * @brief Codigo configuración hardware CAN
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

CAN_FilterTypeDef sFilterConfig;

CAN_TxHeaderTypeDef TxHeader;

CAN_RxHeaderTypeDef RxHeader;

uint8_t TxData[8];

uint8_t RxData[8];

uint32_t TxMailbox;

uint8_t msg_id = 0x30;

/* CAN object instance */
CAN_t can_obj;

/** Bandera mensaje recibido CAN */
can_rx_status_t flag_rx_can = CAN_MSG_NOT_RECEIVED;

/** Bandera transmisión CAN */
can_tx_status_t flag_tx_can = CAN_TX_READY;

/***********************************************************************************************************************
 * Private functions prototypes
 **********************************************************************************************************************/

static void CAN_FilterConfig(void);

/***********************************************************************************************************************
 * Public functions implementation
 **********************************************************************************************************************/

void CAN_HW_Init(void)
{
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
		/* Notification Error */
		Error_Handler();
	}

	/* CAN message transmission configuration */
	TxHeader.IDE = CAN_ID_STD;				// standard ID
	TxHeader.StdId = msg_id;				// ID
	TxHeader.RTR = CAN_RTR_DATA;			// data frame
	TxHeader.DLC = 1;						// 1 byte to send
	TxHeader.TransmitGlobalTime = DISABLE;

	TxData[0] = 25;							// 1 data byte

	/* Start transmission process */
	if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		Error_Handler();
	}

	/* Start time base trigger CAN timer */
	HAL_TIM_Base_Start_IT(&htim7);

	/* Inicializa CAN usando driver */
	/*CAN_API_Init( &can_obj,
				STANDARD_FRAME,
				NORMAL_MSG,
				CAN_Wrapper_Init,
				CAN_Wrapper_TransmitData,
				CAN_Wrapper_ReceiveData,
				CAN_Wrapper_DataCount);*/
}

/***********************************************************************************************************************
 * Exported functions implementation
 **********************************************************************************************************************/

/* Callback mensaje CAN recibido */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
    /* The flag indicates that the callback was called */
    flag_rx_can = CAN_MSG_RECEIVED;
}

/* Callback timer trigger de transmisión de datos de bus de salida CAN a módulo CAN */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	i++;

	if(htim == &htim7 && i>5)
	{
		/* Toggle LED 1 (Red LED) */
		BSP_LED_Toggle(LED1);

		/* The flag indicates that the callback was called */
		flag_tx_can = CAN_TX_READY;

		/* Transmit data test */
		TxData[0] = (char)(i & 0xff);
		HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
	}
}

/***********************************************************************************************************************
 * Private functions implementation
 **********************************************************************************************************************/

static void CAN_FilterConfig(void)
{
	sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	sFilterConfig.FilterBank = 0;			// CAN 1 [0..13]
	sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;

	sFilterConfig.FilterIdHigh = 0;			//msg_id << 5;
	sFilterConfig.FilterIdLow = 0;
	sFilterConfig.FilterMaskIdHigh = 0; 	//msg_id << 5;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.SlaveStartFilterBank = 20;

	if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig)!= HAL_OK)
	{
		Error_Handler();
	}
}
