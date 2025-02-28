/*
 * bsp.h
 *
 *  Created on: Feb 28, 2025
 *      Author: User
 */

#ifndef INC_BSP_H_
#define INC_BSP_H_

#include "stm32f0xx.h"

/*
 * LED driver functions
 */

void	BSP_LED_Init	(void);
void	BSP_LED_On	(void);
void	BSP_LED_Off	(void);
void	BSP_LED_Toggle	(void);


#endif /* INC_BSP_H_ */
