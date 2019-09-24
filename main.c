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
#include "stm32F4xx_HAL.h"
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

GPIO_InitTypeDef MOTER;
GPIO_InitTypeDef E_MOTER;



void ms_delay_int_count(volatile unsigned int nTime)
{
	nTime = (nTime * 14000);
	for(; nTime > 0; nTime--);
}

void MOTER_USR(int usr_case){

	switch(usr_case){
		case 1:
			HAL_GPIO_WritePin(GPIOB, MOTER.Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 1);
			break;
		case 2:
			HAL_GPIO_WritePin(GPIOB, MOTER.Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9 | GPIO_PIN_8, 0);
			break;
		case 3:
			HAL_GPIO_WritePin(GPIOB, MOTER.Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);
			break;
		case 4:
			HAL_GPIO_WritePin(GPIOB, MOTER.Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9 | GPIO_PIN_8, 1);
			break;
	}
}

#define CCW 	1
#define STOP 	2
#define CW 		3
#define BREAK 	4

int
main(int argc, char* argv[])
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	MOTER.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	MOTER.Mode = GPIO_MODE_OUTPUT_PP;
	MOTER.Pull = GPIO_NOPULL;
	MOTER.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &MOTER);

	E_MOTER.Pin = GPIO_PIN_3;
	E_MOTER.Mode = GPIO_MODE_OUTPUT_PP;
	E_MOTER.Pull = GPIO_NOPULL;
	E_MOTER.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOA, &E_MOTER);

	HAL_GPIO_WritePin(GPIOA, E_MOTER.Pin, 1);
	while (1)
	{
		MOTER_USR(CCW);
		ms_delay_int_count(300);

		MOTER_USR(STOP);
		ms_delay_int_count(300);

		MOTER_USR(CW);
		ms_delay_int_count(300);

		MOTER_USR(BREAK);
		ms_delay_int_count(300);

	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
