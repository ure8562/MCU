/*
 * This file is part of the ÂµOS++ distribution.
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
#include "stm32f4xx.h"

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


#define CCW		1
#define STOP	2
#define CW		3
#define BREAK	4

#define ON		1
#define OFF		0

/* Áö¿¬·çÆ¾*/
void ms_delay_int_count(volatile unsigned int nTime)
{
	nTime = (nTime * 14000);
	for(; nTime > 0; nTime--);
}

int main(int argc, char* argv[])
{
	// clock enable
	RCC->AHB1ENR = 0x00000007;

	// SW config
	GPIOB->MODER &= 0x00FFFFFF;

	// LED config
	GPIOC->MODER |= 0x00000055;

	unsigned int data;
	unsigned int flag_led[4]={OFF,};
	//unsigned int count=0;

	while (1)
	{
		data = GPIOB->IDR;

		for(int i = 0 ; i<4 ; i++){
				if(data & (0x01<<(12 + i)))	//SW0
				{
					if(flag_led[i] == OFF)
					{
						GPIOC->ODR |= 0x01<<i;
						flag_led[i] = ON;
					}
					else
					{
						GPIOC->ODR &= ~(0x01<<i);
						flag_led[i] = OFF;
					}
					ms_delay_int_count(300);
				}

		}
		/*
		if(data & (0x01<<13))	//SW1
		{
			if(flag_led[1] == OFF)
			{
				GPIOC->ODR |= 0x01<<1;
				flag_led[1] = ON;
			}else
			{
				GPIOC->ODR &= ~(0x01<<1);
				flag_led[1] = OFF;
			}
			ms_delay_int_count(300);
		}

		if(data & (0x01<<14))	//SW2
		{
			if(flag_led[2] == OFF)
			{
				GPIOC->ODR |= 0x01<<2;
				flag_led[2] = ON;
			}else
			{
				GPIOC->ODR &= ~(0x01<<2);
				flag_led[2] = OFF;
			}
			ms_delay_int_count(300);
		}

		if(data & (0x01<<15))	//SW3
		{
			if(flag_led[3] == OFF)
			{
				GPIOC->ODR |= 0x01<<3;
				flag_led[3] = ON;
			}else
			{
				GPIOC->ODR &= ~(0x01<<3);
				flag_led[3] = OFF;
			}
			ms_delay_int_count(300);
		}
		*/
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
