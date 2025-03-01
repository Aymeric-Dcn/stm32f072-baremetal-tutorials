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


/*
 * Push-Button driver functions
 */

void       BSP_PB_Init		(void);
uint8_t    BSP_PB_GetState	(void);

/*
 * Debug Console init
 */

void	BSP_Console_Init	(void);

/*
 * ADC functions
 */
void BSP_ADC_Init		(void);

/*
 * Timer functions
 */
void BSP_TIMER_Timebase_Init	(void);
void BSP_TIMER_IC_Init		(void);
/*
 * Timer delays
 */

void BSP_DELAY_TIM_init		(void);
void BSP_DELAY_TIM_ms		(uint16_t ms);

#endif /* INC_BSP_H_ */
