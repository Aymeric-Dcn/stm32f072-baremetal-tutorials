/*
 * bsp.c
 *
 *  Created on: Feb 28, 2025
 *      Author: User
 */

#include "bsp.h"

/*
 * BSP_LED_Init()
 * Initialize LED pin (PA5) as a High-Speed Push-Pull output
 * Set LED initial state to OFF
 */

void BSP_LED_Init()
{
	// Enable GPIOA clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure PA5 as output
	GPIOA->MODER &= ~GPIO_MODER_MODER5_Msk;
	GPIOA->MODER |= (0x01 <<GPIO_MODER_MODER5_Pos);

	// Configure PA5 as a push-pull output
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_5;

	// Configure PA5 as High-Speed Output
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR5_Msk;
	GPIOA->OSPEEDR |= (0x03 <<GPIO_OSPEEDR_OSPEEDR5_Pos);

	// Disable PA5 Pull-up/PUll-down
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5_Msk;

	// Set Initial State OFF
	GPIOA->BSRR |= GPIO_BSRR_BR_5;
}


/*
 * BSP_LED_On()
 * Turn ON LED on PA5
 */

void BSP_LED_On()
{
	GPIOA->BSRR = GPIO_BSRR_BS_5;
}

/*
 * BSP_LED_Off()
 * Turn OFF LED on PA5
 */

void BSP_LED_Off()
{
	GPIOA->BSRR = GPIO_BSRR_BR_5;
}

/*
 * BSP_LED_Toggle()
 * Toggle LED on PA5
 */

void BSP_LED_Toggle()
{
	GPIOA->ODR ^= GPIO_ODR_5;
}
