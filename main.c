/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "stm32l0xx.h"
#include "stm32l0538_discovery.h"
			
void comp2_gpio_init();
void comp2_init();
void comp2_start();

int main(void)
{
	comp2_gpio_init();
	comp2_init();
	comp2_start();

	for(;;);
}

void comp2_gpio_init() {
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;	// RCC_SYSCFG_CLK_ENABLE
	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;		// GPIOB Clock enable

	GPIOB->MODER |= GPIO_MODER_MODE7;		// analog mode
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD7_Msk;	// no pullup/down
}

void comp2_init() {
	NVIC_SetPriority(ADC1_COMP_IRQn,0);
	NVIC_EnableIRQ(ADC1_COMP_IRQn);

	SYSCFG->CFGR3 |= SYSCFG_CFGR3_EN_VREFINT | SYSCFG_CFGR3_ENBUFLP_VREFINT_COMP;	// enable vref
	for(int i = 0; i < 1000; ++i);	// wait a bit

	COMP2->CSR &= ~(COMP_CSR_COMP2SPEED | COMP_CSR_COMP2SPEED | COMP_CSR_COMP2INPSEL | COMP_CSR_COMP2POLARITY | COMP_CSR_COMP2LPTIM1IN2 | COMP_CSR_COMP2LPTIM1IN1);
	COMP2->CSR |= COMP_CSR_COMP2INPSEL_2 | COMP_CSR_COMP2INNSEL_2 | COMP_CSR_COMP2EN; // PB7, 1/4vref, enable

	COMP1->CSR &= ~COMP_CSR_COMP1WM_Msk; // make sure window mode disabled

	uint32_t exti_line = EXTI_IMR_IM22;
	SET_BIT(EXTI->RTSR, exti_line);	// rising edge
	SET_BIT(EXTI->FTSR, exti_line); // falling edge

	WRITE_REG(EXTI->PR, exti_line);	// Clear COMP EXTI pending bit (if any)
	SET_BIT(EXTI->EMR, exti_line);	// event mode enabled (otherwise clear)
	SET_BIT(EXTI->IMR, exti_line);	// enable EXTI interrupt mode (otherwise clear)
}

void comp2_start() {
	SET_BIT(COMP2->CSR, COMP_CSR_COMPxEN);
	for(int i = 0; i < 1000; ++i);	// wait a bit
}

void ADC1_COMP_IRQHandler() {
	if(READ_BIT(EXTI->PR, COMP_EXTI_LINE_COMP2) != RESET) {	// check for pending event for COMP2
		WRITE_REG(EXTI->PR, COMP_EXTI_LINE_COMP2);			// clear event

		// DO SOMETHING TODO
	}
}
