/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32F4xx.h"
// ----------------------------------------------------------------------------
//
// Standalone STM32F4 empty sample (trace via ITM).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the ITM output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

#define STOP 0
#define START 1

void ms_delay_int_count(volatile unsigned int nTime)
{
	nTime = (nTime * 14000);
	for(; nTime > 0; nTime--);
}

void EXIT_Line_Config(void){
	RCC->AHB1ENR |= 0x0004;
	GPIOC->MODER &= 0x00FFFFFF;

	RCC->APB2ENR |= 0x00004000;
	SYSCFG->EXTICR[3] |= 0x1111;



	EXTI->RTSR |= 0xF000;
	EXTI->IMR  |= 0xF000;

	NVIC_SetPriority(EXTI15_10_IRQn ,2);
	NVIC_EnableIRQ(EXTI15_10_IRQn);

}

unsigned int led0_sta = STOP;
unsigned int led1_sta = STOP;

int
main(int argc, char* argv[])
{
	RCC->AHB1ENR |= 0x0007;

	//LED Config
	GPIOC->MODER |= 0x5555;

	//SW Config
	EXIT_Line_Config();
	unsigned int time=0;

	while (1){
		if(led0_sta == START && led1_sta == START)
			time = 50;
		else if(led0_sta == START || led1_sta == START)
			time = 100;

		if(led0_sta == START){
			GPIOC->ODR |= 0x0001;
			ms_delay_int_count(time);
			GPIOC->BSRR = 0x00010000;
			ms_delay_int_count(time);
		}
		if(led1_sta == START){
			GPIOC->ODR |= 0x0002;
			ms_delay_int_count(time);
			GPIOC->BSRR = 0x00020000;
			ms_delay_int_count(time);
		}


	}

}
void EXTI15_10_IRQHandler(void){
	if(EXTI->PR == 0x1000)
	{
		EXTI->PR |= (0x01<<12);
		if(led0_sta == STOP)
			led0_sta = START;
		else
			led0_sta = STOP;
	}
	if(EXTI->PR == 0x2000)
	{
		EXTI->PR |= (0x01<<13);
		if(led1_sta == STOP)
			led1_sta = START;
		else
			led1_sta = STOP;
	}
	if(EXTI->PR == 0x4000)
	{
		EXTI->PR |= (0x01<<14);
		led0_sta = STOP;
		led1_sta = STOP;
	}
	if(EXTI->PR == 0x8000)
	{
		EXTI->PR |= (0x01<<15);
		GPIOC->ODR ^= 0x0008;
	}

}

void EXTI0_IRQHandler(void){
	EXTI->PR |= (0x01<<0);
}
void EXTI1_IRQHandler(void){
	EXTI->PR |= (0x01<<1);
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
