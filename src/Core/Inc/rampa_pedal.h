/**
 * @file rampa_pedal.c
 * @author Subgrupo Control y Periféricos - Elektron Motorsports
 * @brief Archivo header para rampa_pedal.h
 * @version 0.1
 * @date 2022-05-05
 *
 * @copyright Copyright (c) 2022
 *
 */


#ifndef _RAMPA_PEDAL_H_
#define _RAMPA_PEDAL_H_

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/

#include <math.h>

#include "buses.h"

/***********************************************************************************************************************
 * Public function prototypes
 **********************************************************************************************************************/

/**
 * @brief Función principal de bloque rampa pedal.
 *
 * Se encarga de transformar el valor de pedal registrado de periféricos a un valor de
 * velocidad que será empleado por inversor. Para cada modo de manejo se tiene una
 * función de transferencia diferente para determinar el valor de velocidad asociado al
 * valor de pedal registrado desde periféricos.
 *
 * Lee la variable pedal de la estructura de variables decodificadas de periféricos,
 * es decir, la estructura de tipo rx_peripherals_t que se encuentra en el bus_data.
 *
 * Escribe en la variable velocidad del bus_data.
 *
 * No es static, por lo que puede ser usada por otros archivos.
 *
 * @param None
 * @retval None
 */
void RAMPA_PEDAL_Process(void);

#endif /* _RAMPA_PEDAL_H_ */
