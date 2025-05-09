/*
 * main.c
 *
 *  Created on: Feb 26, 2025
 *      Author: User
 */


#include "stm32f0xx.h"
#include "bsp.h"
#include "main.h"
#include "delay.h"
#include <math.h>
#include "sinewave_table.h"
#include "rtc.h"

// Static functions
static void SystemClock_Config (void);
//static uint8_t wait_for_flags  (void);

// Global variable
uint16_t	 ADC_result;
uint16_t 	 rx_data = 0;
uint8_t      button_irq = 0;
uint8_t      timebase_irq = 0;
uint8_t	     console_rx_byte[10];
uint8_t	     console_rx_irq = 0;
uint8_t	  	 rx_dma_buffer[8];
uint8_t rx_dma_irq = 0;
uint16_t	adc_dma_buffer[3];
uint8_t rtc_irq = 0;

// Main function
int main()
{
	time_t		now;

	// Configure System Clock
	SystemClock_Config();

	// Peripheral Inits
	BSP_LED_Init();
	BSP_Console_Init();
	BSP_PB_Init();

    // NVIC Init
    BSP_NVIC_Init();
	my_printf("\r\n\n --- Program Starting Over ---\r\n");

	// If this is a Power ON reset
	if ( (RCC->CSR & RCC_CSR_PORRSTF_Msk) == RCC_CSR_PORRSTF )
	{
		my_printf("This is a Power ON reset\r\n");

		// Setup RTC clock
		BSP_RTC_Clock_Config();

		// Setup RTC time to 12:00:00
		now.hours   = 12;
		now.minutes = 00;
		now.seconds = 00;
		BSP_RTC_SetTime(&now);

		my_printf("RTC time has been reset to 12:00:00\r\n");
	}

	// If reset pin was held low
	else if ( (RCC->CSR & RCC_CSR_PINRSTF_Msk) == RCC_CSR_PINRSTF )
	{
		if ((RCC->CSR & RCC_CSR_IWDGRSTF_Msk) == RCC_CSR_IWDGRSTF )
		{
			my_printf("The watchdog did his job!\r\n");
		}

		my_printf("Reset pin was low\r\n");
		my_printf("RTC continues...\r\n");
	}

	// Clear reset flags for next reset
	RCC->CSR |= RCC_CSR_RMVF;

	// Watchdog setup
	IWDG->KR = 0x5555;				// Enable write access
	IWDG->PR = (0x03 <<IWDG_PR_PR_Pos);	// Prescaler /32
	IWDG->RLR = 1250;				// 1s period
	IWDG->KR = 0xAAAA;				// Reload Watchdog
	IWDG->KR = 0xCCCC;				// Enable Watchdog

	// Enable interruptions
	BSP_NVIC_Init();

	// Main loop
	while(1)
	{
		BSP_RTC_GetTime(&now);

		my_printf("RTC Time is %02d:%02d:%02d\r", now.hours,
                           now.minutes, now.seconds);

		BSP_LED_Toggle();
		BSP_DELAY_ms(100);

		IWDG->KR = 0xAAAA;		// Reload Watchdog
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

	/*--- Use PA8 as LSE output ---*/
	// Set MCO source as LSE
	RCC->CFGR &= ~RCC_CFGR_MCO_Msk;
	RCC->CFGR |=  RCC_CFGR_MCOSEL_LSE;     // Change here

	// No prescaler
	RCC->CFGR &= ~RCC_CFGR_MCOPRE_Msk;

	// Enable GPIOA clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure PA8 as Alternate function
	GPIOA->MODER &= ~GPIO_MODER_MODER8_Msk;
	GPIOA->MODER |= (0x02 <<GPIO_MODER_MODER8_Pos);

	// Set to AF0 (MCO output)
	GPIOA->AFR[1] &= ~(0x0000000F);
	GPIOA->AFR[1] |=  (0x00000000);

	// Update SystemCoreClock global variable
	SystemCoreClockUpdate();
}

/*
 * Wait for & Report I2C status flags
 */

//static uint8_t wait_for_flags()
//{
//	uint8_t	exit = 0;
//	// Clear STOPF and NACKF flags
//	I2C1->ICR |= I2C_ICR_STOPCF;
//	I2C1->ICR |= I2C_ICR_NACKCF;
//
//	while(exit == 0)
//	{
//		// TXIS -> PA0
//		// Exit when set
//		if ((I2C1->ISR & I2C_ISR_TXIS) != 0)
//		{
//			GPIOA->BSRR = GPIO_BSRR_BS_0;
//			exit = 1;
//		}
//		else GPIOA->BSRR = GPIO_BSRR_BR_0;
//
//		// TC -> PA4
//		// Exit when set
//		if ((I2C1->ISR & I2C_ISR_TC) != 0)
//		{
//			GPIOA->BSRR = GPIO_BSRR_BS_4;
//			exit = 3;
//		}
//		else GPIOA->BSRR = GPIO_BSRR_BR_4;
//
//		// STOPF -> PC1
//		// Exit when set
//		if ((I2C1->ISR & I2C_ISR_STOPF) != 0)
//		{
//			GPIOC->BSRR = GPIO_BSRR_BS_1;
//			exit = 5;
//		}
//		else GPIOC->BSRR = GPIO_BSRR_BR_1;
//	}
//	return exit;
//}
