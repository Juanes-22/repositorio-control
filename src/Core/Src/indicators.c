/**
 * @file indicators.c
 * @author Subgrupo Control y Periféricos - Elektron Motorsports
 * @brief Generación de indicadores de tarjeta Control
 * @version 0.1
 * @date 2022-05-05
 *
 * @copyright Copyright (c) 2022
 *
 */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/

#include "indicators.h"

/***********************************************************************************************************************
 * Private macros
 **********************************************************************************************************************/

/** @brief Duración de blink de los LEDs */
#define BLINK_TIME              250U

/** @brief Duración para apagado de los LEDs */
#define LEDS_TURNOFF_TIME       3000U

/** @brief Duración para apagado del buzzer */
#define BUZZER_TURNOFF_TIME     2000U

/***********************************************************************************************************************
 * Private variables definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private functions prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Public functions implementation
 **********************************************************************************************************************/

/**
 * @brief Función principal bloque generación de indicadores.
 * 
 * Actualiza LEDs de acuerdo al modo de manejo actual.
 *
 * No es static, por lo que puede ser usada por otros archivos.
 *
 * @param None
 * @retval None
 */
void INDICATORS_Process(void)
{
	/*
    if(bus_data.driving_mode == kDRIVING_MODE_ECO)
    {
        BSP_LED_On(LED1);
        BSP_LED_Off(LED2);
        BSP_LED_Off(LED3);
    }

    else if(bus_data.driving_mode == kDRIVING_MODE_NORMAL)
    {
        BSP_LED_Off(LED1);
        BSP_LED_On(LED2);
        BSP_LED_Off(LED3);
    }

    else if(bus_data.driving_mode == kDRIVING_MODE_SPORT)
    {
        BSP_LED_Off(LED1);
        BSP_LED_Off(LED2);
        BSP_LED_On(LED3);
    }

    if(bus_data.failure == kFAILURE_AUTOKILL)
    {
        BSP_BUZZER_On();

        // blink buzzer
    }
    */
	asm("nop");
}

/**
 * @brief Actualiza LEDs para indicar confirmación de los módulos durante inicialización
 * 
 * No es static, por lo que puede ser usada por otros archivos.
 *
 * @param None
 * @retval None
 */
void INDICATORS_Update_ModulesLEDs(void)
{
    if(bus_can_input.bms_ok == CAN_VALUE_MODULE_OK)
    {
        BSP_LED_On(LED1);
    }

    if(bus_can_input.dcdc_ok == CAN_VALUE_MODULE_OK)
    {
        BSP_LED_On(LED2);
    }

    if(bus_can_input.inversor_ok == CAN_VALUE_MODULE_OK)
    {
        BSP_LED_On(LED3);
    }
}

/**
 * @brief Indicación de que la tarjeta ha finalizado la inicialización.
 *
 * No es static, por lo que puede ser usada por otros archivos.
 *
 * @param None
 * @retval None
 */
void INDICATORS_Finish_StartUp(void)
{
    uint32_t tickstart;

    uint32_t blink_tickstart;

    BSP_BUZZER_On();

    tickstart = HAL_GetTick();      // get ticks for the turn off time of LEDs and buzzer

    blink_tickstart = tickstart;    // ticks for the blinking of the LEDs

    BSP_LED_Off(LED1);
    BSP_LED_Off(LED2);
    BSP_LED_Off(LED3);

    while(1)
    {
        if( (HAL_GetTick() - blink_tickstart) > BLINK_TIME )  		// 250ms
        {
            BSP_LED_Toggle(LED1);
            BSP_LED_Toggle(LED2);
            BSP_LED_Toggle(LED3);

            blink_tickstart = HAL_GetTick();
        }

        if( (HAL_GetTick() - tickstart) > BUZZER_TURNOFF_TIME )   	// 2s
        {
            BSP_BUZZER_Off();
        }

        if( (HAL_GetTick() - tickstart) > LEDS_TURNOFF_TIME )   	// 3s
        {
            BSP_LED_Off(LED1);
            BSP_LED_Off(LED2);
            BSP_LED_Off(LED3);

            break;  // startup is completed
        }
    }
}

/***********************************************************************************************************************
 * Private functions implementation
 **********************************************************************************************************************/
