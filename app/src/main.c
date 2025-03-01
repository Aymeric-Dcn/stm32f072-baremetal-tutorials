/*
 * main.c
 *
 *  Created on: Feb 26, 2025
 *      Author: User
 */


#include "stm32f0xx.h"
#include "bsp.h"
#include "main.h"

// Static functions
static void SystemClock_Config (void);

// Global variable
uint16_t	ADC_result;

#include "stm32f0xx.h"
#include "bsp.h"
#include "delay.h"

// Static functions
static void SystemClock_Config(void);

// Global variables
uint16_t	TIM6_counter;
uint16_t	TIM6_UIF;

// Main function
// Main function
// Main function
// Main function
// Main function
// Main function
int main()
{
	uint16_t pulse_A, pulse_B;
	uint8_t	 state;

	// Configure System Clock
	SystemClock_Config();

	// Initialize Timer for delays
	BSP_DELAY_TIM_init();

	// Initialize Timer for PWM output
	BSP_TIMER_PWM_Init();

	// Initialize state variables
	state 	= 0;
	pulse_A = 1500;
	pulse_B = 1500;

	while(1)
	{
		// Set PWMs duty ratios according to pulse_A and pulse_B values
		TIM1->CCR1 = pulse_A;
		TIM1->CCR2 = pulse_B;
		// Manage state machine
		switch (state)
		{
			// Increasing A, Decreasing B
			case 0 :
			{
				pulse_A++;
				pulse_B--;
				// Stop condition -> State 1
				if (pulse_A == 2000) state = 1;
				break;
			}

			// Increasing B, Decreasing A
			case 1 :
			{
				pulse_A--;
				pulse_B++;
				// Stop condition -> State 0
				if (pulse_B == 2000) state = 0;
				break;
			}
		}

		// Wait for 2 ms
		BSP_DELAY_TIM_ms(2);
	}
}
/*
 * Clock configuration for the Nucleo STM32F072RB board
 * HSE input Bypass Mode            -> 8MHz
 * SYSCLK, AHB, APB1                -> 48MHz
 * PA8 as MCO with /16 prescaler    -> 3MHz
 */

static void SystemClock_Config()
{
	uint32_t	HSE_Status;
	uint32_t	PLL_Status;
	uint32_t	SW_Status;
	uint32_t	timeout = 0;
	timeout = 1000000;

	// Start HSE in Bypass Mode
	RCC->CR |= RCC_CR_HSEBYP;
	RCC->CR |= RCC_CR_HSEON;

	// Wait until HSE is ready
	do
	{
		HSE_Status = RCC->CR & RCC_CR_HSERDY_Msk;
		timeout--;
	} while ((HSE_Status == 0) && (timeout > 0));

	// Select HSE as PLL input source
	RCC->CFGR &= ~RCC_CFGR_PLLSRC_Msk;
	RCC->CFGR |= (0x02 <<RCC_CFGR_PLLSRC_Pos);

	// Set PLL PREDIV to /1
	RCC->CFGR2 = 0x00000000;

	// Set PLL MUL to x6
	RCC->CFGR &= ~RCC_CFGR_PLLMUL_Msk;
	RCC->CFGR |= (0x04 <<RCC_CFGR_PLLMUL_Pos);

	// Enable the main PLL
	RCC->CR |= RCC_CR_PLLON;
	// Wait until PLL is ready
	do
	{
		PLL_Status = RCC->CR & RCC_CR_PLLRDY_Msk;
		timeout--;
	} while ((PLL_Status == 0) && (timeout > 0));

	// Set AHB prescaler to /1
	RCC->CFGR &= ~RCC_CFGR_HPRE_Msk;
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	//Set APB1 prescaler to /1
	RCC->CFGR &= ~RCC_CFGR_PPRE_Msk;
	RCC->CFGR |= RCC_CFGR_PPRE_DIV1;

	// Enable FLASH Prefetch Buffer and set Flash Latency
	FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

	// Select the main PLL as system clock source
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	// Wait until PLL becomes main switch input
	do
	{
		SW_Status = (RCC->CFGR & RCC_CFGR_SWS_Msk);
		timeout--;
	} while ((SW_Status != RCC_CFGR_SWS_PLL) && (timeout > 0));

	// Update SystemCoreClock global variable
	SystemCoreClockUpdate();
}
