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

GPIO_InitTypeDef STEP;

void ms_delay_int_count(volatile unsigned int nTime)
{
	nTime = (nTime * 14000);
	for(; nTime > 0; nTime--);
}

void STEP_Config(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	STEP.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	STEP.Mode = GPIO_MODE_OUTPUT_PP;
	STEP.Pull = GPIO_NOPULL;
	STEP.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &STEP);

	STEP.Pin = GPIO_PIN_11 | GPIO_PIN_12;
	HAL_GPIO_Init(GPIOA, &STEP);
}
#define One_Sang 	 	1
#define Two_Sang 	 	2
#define One_Two_Sang 	3

#define CW				1
#define CCW				2

void STEP_cntl(uint8_t mode, uint8_t dir, uint8_t speed)
{

	switch (mode){
	case Two_Sang:
		if(dir == CCW)
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 1);	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 1);
			ms_delay_int_count(speed);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 1);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 1);	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 0);
			ms_delay_int_count(speed);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 1);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 0);	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 0);
			ms_delay_int_count(speed);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 0);	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 1);
			ms_delay_int_count(speed);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 1);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 0);	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 0);
			ms_delay_int_count(speed);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 1);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 1);	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 0);
			ms_delay_int_count(speed);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 1);	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 1);
			ms_delay_int_count(speed);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 0);	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 1);
			ms_delay_int_count(speed);
		}
		break;
	}
}

int
main(int argc, char* argv[])
{
	STEP_Config();

	while (1)
	{
		STEP_cntl(Two_Sang, CCW, 100);
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
