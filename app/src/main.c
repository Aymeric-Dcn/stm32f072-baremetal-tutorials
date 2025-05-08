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
static uint8_t wait_for_flags  (void);

// Global variable
uint16_t	ADC_result;
uint8_t	button_irq = 0;
uint8_t 	timebase_irq = 0;
uint16_t rx_data = 0;

#include "stm32f0xx.h"
#include "bsp.h"
#include "delay.h"
#include <math.h>

// Static functions
static void SystemClock_Config(void);



// Main function
int main(void)
{
	uint8_t		rx_data[2];

	// Configure System Clock for 48MHz from 8MHz HSE
	SystemClock_Config();

	// Initialize LED and USER Button
	BSP_LED_Init();
	BSP_PB_Init();

	// Initialize Debug Console
	BSP_Console_Init();
	my_printf("\r\nConsole Ready!\r\n");
	my_printf("SYSCLK = %d Hz\r\n", SystemCoreClock);

	// Initialize I2C1 peripheral
	BSP_I2C1_Init();

	// Read 2 bytes in VEML7700 register @0x00
	BSP_I2C1_Read(0x10, 0x00, rx_data, 2);

	// Set device address
	I2C1->CR2 &= ~I2C_CR2_SADD_Msk;
	I2C1->CR2 |= ((0x10 <<1U) <<I2C_CR2_SADD_Pos);

	// Set I2C in Write mode
	I2C1->CR2 &= ~I2C_CR2_RD_WRN;

	// Transfer NBYTES = 3, no AUTOEND
	I2C1->CR2 &= ~I2C_CR2_NBYTES;
	I2C1->CR2 |= (3 <<16U);
	I2C1->CR2 &= ~I2C_CR2_AUTOEND;

	// Start I2C transaction
	I2C1->CR2 |= I2C_CR2_START;

	wait_for_flags();          // TXIS #1

	// Send Register address
 	I2C1->TXDR = 0x00;

	wait_for_flags();          // TXIS #2

	// Send first byte (LSB)
	I2C1->TXDR = 0x00;

	wait_for_flags();          // TXIS #3

	// Send second byte (MSB)
	I2C1->TXDR = 0x00;

	wait_for_flags();          // TC

	// Generate STOP condition
	I2C1->CR2 |= I2C_CR2_STOP;

	while(1)
	{
	}
	while(1)
	{
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

/*
 * Wait for & Report I2C status flags
 */

static uint8_t wait_for_flags()
{
	uint8_t	exit = 0;
	// Clear STOPF and NACKF flags
	I2C1->ICR |= I2C_ICR_STOPCF;
	I2C1->ICR |= I2C_ICR_NACKCF;

	while(exit == 0)
	{
		// TXIS -> PA0
		// Exit when set
		if ((I2C1->ISR & I2C_ISR_TXIS) != 0)
		{
			GPIOA->BSRR = GPIO_BSRR_BS_0;
			exit = 1;
		}
		else GPIOA->BSRR = GPIO_BSRR_BR_0;

		// TC -> PA4
		// Exit when set
		if ((I2C1->ISR & I2C_ISR_TC) != 0)
		{
			GPIOA->BSRR = GPIO_BSRR_BS_4;
			exit = 3;
		}
		else GPIOA->BSRR = GPIO_BSRR_BR_4;

		// STOPF -> PC1
		// Exit when set
		if ((I2C1->ISR & I2C_ISR_STOPF) != 0)
		{
			GPIOC->BSRR = GPIO_BSRR_BS_1;
			exit = 5;
		}
		else GPIOC->BSRR = GPIO_BSRR_BR_1;
	}
	return exit;
}
