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

#define STOP 	0
#define START 	1

GPIO_InitTypeDef LED;
GPIO_InitTypeDef SW;

TIM_HandleTypeDef TimHandle1;
TIM_HandleTypeDef TimHandle10;

void LED_Config(void)
{
	LED.Pin 	= GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
	LED.Mode	= GPIO_MODE_OUTPUT_PP;
	LED.Pull 	= GPIO_NOPULL;
	LED.Speed 	= GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOC, &LED);
}

void TIMER_Config1(void)
{
	__HAL_RCC_TIM1_CLK_ENABLE();

	TimHandle1.Instance				=	TIM1;
	TimHandle1.Init.Prescaler		=	16800 - 1;
	TimHandle1.Init.Period			=	1000 - 1;
	TimHandle1.Init.ClockDivision	=	TIM_CLOCKDIVISION_DIV1;
	TimHandle1.Init.CounterMode		=	TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&TimHandle1);

	HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn , 0, 0);
	HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn );
}

void TIMER_Config10(void)
{
	__HAL_RCC_TIM10_CLK_ENABLE();

	TimHandle10.Instance			=	TIM10;
	TimHandle10.Init.Prescaler		=	16800 - 1;
	TimHandle10.Init.Period			=	3000 - 1;
	TimHandle10.Init.ClockDivision	=	TIM_CLOCKDIVISION_DIV1;
	TimHandle10.Init.CounterMode	=	TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&TimHandle10);

	HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn , 0, 0);
	HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn );
}

void EXTILine_Config(void)
{
	SW.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	SW.Mode = GPIO_MODE_IT_RISING;
	SW.Pull = GPIO_NOPULL;
	SW.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &SW);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}
unsigned char led0_sta = STOP;
unsigned char led1_sta = STOP;

int
main(int argc, char* argv[])
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	LED_Config();
	EXTILine_Config();
	TIMER_Config1();
	TIMER_Config10();

	while (1)
	{

	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM1){
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_0);
	}
	if(htim->Instance == TIM10){
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_1);
	}

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_12)
	{
		if(led0_sta == STOP)
		{
			HAL_TIM_Base_Start_IT(&TimHandle1);
			led0_sta = START;
		}
		else
		{
			HAL_TIM_Base_Stop_IT(&TimHandle1);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
			led0_sta = STOP;
		}
	}
	if(GPIO_Pin == GPIO_PIN_13)
	{
		if(led1_sta == STOP)
		{
			HAL_TIM_Base_Start_IT(&TimHandle10);
			led1_sta = START;
		}
		else
		{
			HAL_TIM_Base_Stop_IT(&TimHandle10);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
			led1_sta = STOP;
		}
	}

}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
