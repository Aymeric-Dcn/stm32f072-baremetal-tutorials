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

/*
 * BSP_PB_Init()
 * Initialize Push-Button pin (PC13) as input without Pull-up/Pull-down
 * Enable EXTI13 on falling edge
 */

void BSP_PB_Init()
{
	// Enable GPIOC clock
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	// Configure PC13 as input
	GPIOC->MODER &= ~GPIO_MODER_MODER13_Msk;
	GPIOC->MODER |= (0x00 <<GPIO_MODER_MODER13_Pos);

	// Disable PC13 Pull-up/Pull-down
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR13_Msk;

	// Enable SYSCFG clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// Select Port C as interrupt source for EXTI line 13
	SYSCFG->EXTICR[3] &= ~ SYSCFG_EXTICR4_EXTI13_Msk;
	SYSCFG->EXTICR[3] |=   SYSCFG_EXTICR4_EXTI13_PC;

	// Enable EXTI line 13
	EXTI->IMR |= EXTI_IMR_IM13;

	// Disable Rising / Enable Falling trigger
	EXTI->RTSR &= ~EXTI_RTSR_RT13;
	EXTI->FTSR |=  EXTI_FTSR_FT13;
}

/*
 * BSP_PB_GetState()
 * Returns the state of the button (0=released, 1=pressed)
 */

uint8_t BSP_PB_GetState()
{
	uint8_t state;
	if ((GPIOC->IDR & GPIO_IDR_13) == GPIO_IDR_13)
	{
		state = 0;
	}
	else
	{
		state = 1;
	}
	return state;
}

/*
 * BSP_Console_Init()
 * USART2 @ 115200 Full Duplex
 * 1 start - 8-bit - 1 stop
 * TX -> PA2 (AF1)
 * RX -> PA3 (AF1)
 */

void BSP_Console_Init()
{
	// Enable GPIOA clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure PA2 and PA3 as Alternate function
	GPIOA->MODER &= ~(GPIO_MODER_MODER2_Msk | GPIO_MODER_MODER3_Msk);
	GPIOA->MODER |= (0x02 <<GPIO_MODER_MODER2_Pos | 0x02 <<GPIO_MODER_MODER3_Pos);

	// Set PA1 and PA2 to AF1 (USRT2)
	GPIOA->AFR[0] &= ~(0x0000FF00);
	GPIOA->AFR[0] |=  (0x00001100);

	// Enable USART2 clock
	RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;

	// Clear USART2 configuration (reset state)
	// 8-bit, 1 start, 1 stop, CTS/RTS disabled
	USART2->CR1 = 0x00000000;
	USART2->CR2 = 0x00000000;
	USART2->CR3 = 0x00000000;

	// Select PCLK (APB1) as clock source
	// PCLK -> 48 MHz
	RCC->CFGR3 &= ~RCC_CFGR3_USART2SW_Msk;

	// Baud Rate = 115200
	// With OVER8=0 and Fck=48MHz, USARTDIV =   48E6/115200 = 416.6666
	// BRR = 417 -> Baud Rate = 115107.9137 -> 0.08% error
	//
	// With OVER8=1 and Fck=48MHz, USARTDIV = 2*48E6/115200 = 833.3333
	// BRR = 833 -> Baud Rate = 115246.0984 -> 0.04% error (better)
	USART2->CR1 |= USART_CR1_OVER8;
	USART2->BRR = 833;

	// Enable both Transmitter and Receiver
	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;

	// Enable USART2
	USART2->CR1 |= USART_CR1_UE;
}

/*
 * ADC_Init()
 * Initialize ADC for a single channel conversion
 * on channel 11 -> pin PC1
 */

void BSP_ADC_Init()
{
	// Enable GPIOC clock
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	// Configure pin PC1 as analog
	GPIOC->MODER &= ~GPIO_MODER_MODER1_Msk;
	GPIOC->MODER |= (0x03 <<GPIO_MODER_MODER1_Pos);

	// Enable ADC clock
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	// Reset ADC configuration
	ADC1->CR 	= 0x00000000;
	ADC1->CFGR1  = 0x00000000;
	ADC1->CFGR2  = 0x00000000;
	ADC1->CHSELR = 0x00000000;

	// Enable continuous conversion mode
	ADC1->CFGR1 |= ADC_CFGR1_CONT;

	// 12-bit resolution
	ADC1->CFGR1 |= (0x00 <<ADC_CFGR1_RES_Pos);

	// Select PCLK/2 as ADC clock
	ADC1->CFGR2 |= (0x01 <<ADC_CFGR2_CKMODE_Pos);

	// Set sampling time to 28.5 ADC clock cycles
	ADC1->SMPR = 0x03;

	// Select channel 11
	ADC1->CHSELR |= ADC_CHSELR_CHSEL11;

	// Enable ADC
	ADC1->CR |= ADC_CR_ADEN;

	// Start conversion
	ADC1->CR |= ADC_CR_ADSTART;
}






/*
 * BSP_TIMER_Timebase_Init()
 * TIM6 at 48MHz
 * Prescaler   = 48000 -> Counting period = 1ms
 * Auto-reload = 200   -> Update period   = 200ms
 */

/*
 * BSP_TIMER_Timebase_Init()
 * TIM6 at 48MHz
 * Prescaler   = 48000 -> Counting period = 1ms
 * Auto-reload = 200   -> Update period   = 200ms
 */

void BSP_TIMER_Timebase_Init()
{
	// Enable TIM6 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

	// Reset TIM6 configuration
	TIM6->CR1 = 0x0000;
	TIM6->CR2 = 0x0000;

	// Set TIM6 prescaler
	// Fck = 48MHz -> /48000 = 1kHz counting frequency
	TIM6->PSC = (uint16_t) 48000 -1;

	// Set TIM6 auto-reload register for 200ms
	TIM6->ARR = (uint16_t) 200 -1;

	// Enable auto-reload preload
	TIM6->CR1 |= TIM_CR1_ARPE;

	// Enable Interrupt upon Update Event
	TIM6->DIER |= TIM_DIER_UIE;

	// Start TIM6 counter
	TIM6->CR1 |= TIM_CR1_CEN;
}
/*
 * BSP_TIMER_IC_Init()
 * TIM3 as Input Capture
 * Channel 1 -> PB4 (AF1)
 */

void BSP_TIMER_IC_Init()
{
	// Enable GPIOB clock
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	// Configure PB4 as Alternate function
	GPIOB->MODER &= ~(GPIO_MODER_MODER4_Msk);
	GPIOB->MODER |=  (0x02 <<GPIO_MODER_MODER4_Pos);

	// Set PB4 to AF1 (TIM3_CH1)
	GPIOB->AFR[0] &= ~(0x000F0000);
	GPIOB->AFR[0] |=  (0x00010000);

	// Enable TIM3 clock
	RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;

	// Reset TIM3 configuration
	TIM3->CR1  = 0x0000;
	TIM3->CR2  = 0x0000;
	TIM3->CCER = 0x0000;

	// Set TIM3 prescaler
	// Fck = 48MHz -> /48000 = 1KHz counting frequency
	TIM3->PSC = (uint16_t) 48000 -1;

	// Set Auto-Reload to maximum value
	TIM3->ARR = (uint16_t) 0xFFFF;

	// Reset Input Capture configuration
	TIM3->CCMR1 = 0x0000;
	TIM3->CCMR2 = 0x0000;

	// Set Channel 1 input on TI1
	TIM3->CCMR1 |= (0x01 <<TIM_CCMR1_CC1S_Pos);

	// Filter with N=8
	TIM3->CCMR1 |= (0x03 <<TIM_CCMR1_IC1F_Pos);

	// Select falling edge for channel 1
	TIM3->CCER |= (0x00 <<TIM_CCER_CC1NP_Pos) | (0x01 <<TIM_CCER_CC1P_Pos);

	// Enable capture on channel 1
	TIM3->CCER |= (0x01 <<TIM_CCER_CC1E_Pos);

	// Enable TIM3
	TIM3->CR1 |= TIM_CR1_CEN;

	// Setup Input Capture
		TIM3->CCMR1 = 0x0000;
		TIM3->CCMR2 = 0x0000;

		// Channel 1 input on TI1
		TIM3->CCMR1 |= (0x01 <<TIM_CCMR1_CC1S_Pos);

		// Channel 2 input also on TI1
		TIM3->CCMR1 |= (0x02 <<TIM_CCMR1_CC2S_Pos);

		// Filter with N=8
		TIM3->CCMR1 |= (0x03 <<TIM_CCMR1_IC1F_Pos) | (0x03 <<TIM_CCMR1_IC2F_Pos);

		// Select falling edge for channel 1
		TIM3->CCER |= (0x00 <<TIM_CCER_CC1NP_Pos) | (0x01 <<TIM_CCER_CC1P_Pos);

		// Select rising edge for channel 2
		TIM3->CCER |= (0x00 <<TIM_CCER_CC2NP_Pos) | (0x00 <<TIM_CCER_CC2P_Pos);

		// Enable capture on channel 1 & channel 2
		TIM3->CCER |= (0x01 <<TIM_CCER_CC1E_Pos) | (0x01 <<TIM_CCER_CC2E_Pos);

		// Choose Channel 1 as trigger input
		TIM3->SMCR |= (0x05 <<TIM_SMCR_TS_Pos);

		// Slave mode -> Resets counter when trigger occurs
		TIM3->SMCR |= (0x4 <<TIM_SMCR_SMS_Pos);

		// Enable TIM3
		TIM3->CR1 |= TIM_CR1_CEN;
}

/*
 * BSP_TIMER_PWM_Init()
 * TIM1 as Output Compare PWM mode
 * Channel 1 -> PA8 (AF2)
 * Channel 2 -> PA9 (AF2)
 */

void BSP_TIMER_PWM_Init()
{
	// Enable GPIOA clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure PA8 and PA9 as Alternate Function
	GPIOA->MODER &= ~(GPIO_MODER_MODER8_Msk | GPIO_MODER_MODER9_Msk);
	GPIOA->MODER |=  (0x02 <<GPIO_MODER_MODER8_Pos) | (0x02 <<GPIO_MODER_MODER9_Pos);

	// Set PA8 and PA9 to AF2 (TIM1)
	GPIOA->AFR[1] &= ~(0x000000FF);
	GPIOA->AFR[1] |=  (0x00000022);

	// Enable TIM1 clock
	RCC -> APB2ENR |= RCC_APB2ENR_TIM1EN;

	// Reset TIM1 configuration
	TIM1->CR1  = 0x0000;
	TIM1->CR2  = 0x0000;
	TIM1->CCER = 0x0000;

	// Set TIM1 prescaler
	// Fck = 48MHz -> /48 = 1MHz counting frequency (1µs resolution)
	TIM1->PSC = (uint16_t) 48 -1;

	// Set Auto-Reload to period = 11ms
	TIM1->ARR = (uint16_t) 11000;

	// Enable Auto-Reload Preload register
	TIM1->CR1 |= TIM_CR1_ARPE;

	// Setup Input Capture
	TIM1->CCMR1 = 0x0000;
	TIM1->CCMR2 = 0x0000;

	// Setup PWM mode 1 output
	TIM1->CCMR1 |= (0x06 <<TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE;
	TIM1->CCMR1 |= (0x06 <<TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE;

	// Set default PWM values
	TIM1->CCR1 = 1500;
	TIM1->CCR2 = 1500;

	// Enable Outputs
	TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;

	// Enable Main output
	TIM1->BDTR |= TIM_BDTR_MOE;

	// Enable TIM1
	TIM1->CR1 |= TIM_CR1_CEN;
}

/*
 * DAC_Init()
 * Initialize DAC for a single output
 * on channel 1 -> pin PA4
 */

void BSP_DAC_Init()
{
	// Enable GPIOA clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure pin PA4 as analog
	GPIOA->MODER &= ~GPIO_MODER_MODER4_Msk;
	GPIOA->MODER |= (0x03 <<GPIO_MODER_MODER4_Pos);

	// Enable DAC clock
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;

	// Reset DAC configuration
	DAC->CR = 0x00000000;

	// Enable DAC Channel 1
	DAC->CR |= DAC_CR_EN1;
}



/*
 * BSP_NVIC_Init()
 * Setup NVIC controller for desired interrupts
 */

void BSP_NVIC_Init()
{
	// Set maximum priority for EXTI line 4 to 15 interrupts
	NVIC_SetPriority(EXTI4_15_IRQn, 0);

	// Enable EXTI line 4 to 15 (user button on line 13) interrupts
	NVIC_EnableIRQ(EXTI4_15_IRQn);

	// Set priority level 1 for TIM6 interrupt
	NVIC_SetPriority(TIM6_DAC_IRQn, 1);

	// Enable TIM6 interrupts
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}


