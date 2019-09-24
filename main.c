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

#define START	1
#define STOP	0
#define OFF		0
#define ON		1
unsigned int FND_DATA[] =
	{0x3F00, 0x0600, 0x5B00, 0x4F00, 0x6600,
	 0x6D00, 0x7D00, 0x0700, 0x7F00, 0x6F00 };

/* 지연루틴*/
void ms_delay_int_count(volatile unsigned int nTime)
{
	nTime = (nTime * 14000);
	for(; nTime > 0; nTime--);
}

unsigned int fnd_data[] =
		{0x3F00, 	0x0600, 0x5B00, 0x4F00, 0x6600,
		0x6D00, 0x7D00, 0x2700, 0x7F00, 0x6F00};

void fnc_cnt1(int num){
	GPIOC->ODR = FND_DATA[num];
}
// fnd cntl func
void fnd_cntl(int num)
{
	GPIOC->ODR = FND_DATA[num];
}

int main(int argc, char* argv[])
{
	// clock enable
	RCC->AHB1ENR = 0x00000007;

	// SW config
	GPIOB->MODER &= 0x00FFFFFF;

	// FND config
	GPIOC->MODER = 0x55550000;
	GPIOC->MODER |= 0x00000055;

	unsigned int data;
	//unsigned int state_fnd = STOP;
	unsigned int count = 0;
	unsigned int flag = OFF,led0_state = OFF;
	while (1)
	{
		data = GPIOB->IDR;

		// sw0
		if(data & (0x01<<12))
		{
			if(flag == OFF)
			{
				flag = ON;
			}else if (flag == ON)
			{
				flag = OFF;
			}
			ms_delay_int_count(300);
		}

		if(data & (0x01<<13))
		{
			if(led0_state == OFF)
			{
				led0_state = ON;
			}else if (led0_state == ON)
			{
				led0_state = OFF;
			}
			ms_delay_int_count(300);
		}


		// 상태값에 따른 동작을 기술
		if(flag == ON)
		{
			fnd_ctn1(count);
			++count;
			ms_delay_int_count(1000);
		}else if(flag == OFF)
		{
			fnd_ctl(0);
			count = 0;
		}

		if(count == 10){
			count = 0;
		}

		if(led0_state == OFF){
			GPIOC->BSRR = 0x00000001 << 16;
		}else if(led0_state == ON){
			GPIOC->BSRR = 0x00000001;
			ms_delay_int_count(100);
			GPIOC->BSRR = 0x00000001 << 16;
			ms_delay_int_count(100);
		}
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
