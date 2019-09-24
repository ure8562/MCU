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

#define CCW 	1
#define STOP 	2
#define CW		3
#define BREAK	4

#define OFF		0
#define ON		1

GPIO_InitTypeDef MOTER;
GPIO_InitTypeDef E_MOTER;
GPIO_InitTypeDef SW;
GPIO_InitTypeDef LED;

void ms_delay_int_count(volatile unsigned int nTime)
{
	nTime = (nTime * 14000);
	for(; nTime > 0; nTime--);
}

void motor_cntl(int usr_case){

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
int
main(int argc, char* argv[])
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	LED.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	LED.Mode = GPIO_MODE_OUTPUT_PP;
	LED.Pull = GPIO_NOPULL;
	LED.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOC, &LED);

	SW.Pin = GPIO_PIN_12 | GPIO_PIN_13;
	SW.Mode = GPIO_MODE_INPUT;
	SW.Pull = GPIO_NOPULL;
	SW.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &SW);

	unsigned int led0_sta = OFF, led1_sta = OFF;

	unsigned int sw0_be_value = 0, sw0_now_value = 0;
	unsigned int sw1_be_value = 0, sw1_now_value = 0;

	while (1)
	{
		sw0_now_value = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12);
		sw1_now_value = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13);
		if(sw0_now_value == 1 && sw0_be_value == 0) // »ó½Â¿§Áö
		{
			if(led0_sta == OFF)
			{
				//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0 ,GPIO_PIN_RESET);
				//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0 ,GPIO_PIN_SET);
				led0_sta = ON;
			}
			else
			{
				//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0 ,GPIO_PIN_RESET);
				led0_sta = OFF;
			}
		}

		if(sw1_now_value == 1 && sw1_be_value == 0) // »ó½Â¿§Áö
		{
			if(led1_sta == OFF)
			{
				//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1 ,GPIO_PIN_RESET);
				//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1 ,GPIO_PIN_SET);
				led1_sta = ON;
			}
			else
			{
				//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1 ,GPIO_PIN_RESET);
				led1_sta = OFF;
			}
		}

		if(led0_sta == ON)
			{
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
			ms_delay_int_count(100);
			}
		else
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0 ,GPIO_PIN_RESET);
			}

		if(led1_sta == ON)
		{
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);
			ms_delay_int_count(100);

		}
		else
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1,GPIO_PIN_RESET);
		}

		EXTI15_10_IRQHandler()

		sw0_be_value = sw0_now_value;
		sw1_be_value = sw1_now_value;
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
