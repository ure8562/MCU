/*
 * This file is part of the 쨉OS++ distribution.
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

//#define RCC_AHB1ENR	 	(*(volatile unsigned long*)0x40023830)
#define RCC_AHB1ENR	 	(*(volatile unsigned long*)0x40023830)
#define GPIOC_MODER	 	(*(volatile unsigned long*)0x40020800)
#define GPIOC_OTYPER 	(*(volatile unsigned long*)0x40020804)
#define GPIOC_OSPEEDR 	(*(volatile unsigned long*)0x40020808)
#define GPIOC_PUPDR 	(*(volatile unsigned long*)0x4002080C)
#define GPIOC_ODR		(*(volatile unsigned long*)0x40020814)

// 지연루틴	-> 다른방법으로 가능
void ms_delay_int_count(volatile unsigned int nTime)
{
	nTime = (nTime * 14000);
	for(; nTime > 0; nTime--);
}

int
main(int argc, char* argv[])
{
	RCC_AHB1ENR  = 0x00000004;
	GPIOC_MODER  = 0x55550055;
	//GPIOC_MODER  = 0x00000000;
	GPIOC_OTYPER = 0x00000000;
	GPIOC_ODR	 = 0x00000000;

	// Infinite loop
	while (1)
	{
	  /* GPIOC_ODR = 0x00000601;			//1
	   ms_delay_int_count(1000);
	   GPIOC_ODR = 0x00005B02;			//2
	   ms_delay_int_count(1000);
	   GPIOC_ODR = 0x00004F04;			//3
	   ms_delay_int_count(1000);
	   GPIOC_ODR = 0x00006608;			//4
	   ms_delay_int_count(1000);
	   GPIOC_ODR = 0x00006D01;			//5
	   ms_delay_int_count(1000);
	   GPIOC_ODR = 0x00007D02;			//6
	   ms_delay_int_count(1000);
	   GPIOC_ODR = 0x00002704;			//7
	   ms_delay_int_count(1000);
	   GPIOC_ODR = 0x00007F08;			//8
	   ms_delay_int_count(1000);
	   GPIOC_ODR = 0x00006F04;			//9
	   ms_delay_int_count(1000);
		*/
	   for(int i = 0; i<4; i++)
	   {
		   GPIOC_ODR = 0x00000001 << i;
		   ms_delay_int_count(1000);
	   }

	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
