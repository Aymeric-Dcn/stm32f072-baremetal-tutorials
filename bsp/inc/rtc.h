/*
 * rtc.h
 *
 *  Created on: May 9, 2025
 *      Author: aymer
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_

#include "stm32f0xx.h"

// Typedefs
typedef struct
{
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
} time_t;

// Public functions
void BSP_RTC_Clock_Config	(void);
void BSP_RTC_EXTI_Init		(void);

void BSP_RTC_SetTime		(time_t *ptime);
void BSP_RTC_GetTime		(time_t *ptime);

void BSP_RTC_SetAlarmA		(time_t *ptime);
void BSP_RTC_GetAlarmA		(time_t *ptime);

#endif /* INC_RTC_H_ */
