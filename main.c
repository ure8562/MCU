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


GPIO_InitTypeDef LED;
TIM_HandleTypeDef TimHandle2;
TIM_HandleTypeDef TimHandle3;
TIM_HandleTypeDef TimHandle4;
TIM_HandleTypeDef TimHandle5;

void LED_Config(void)
{
	LED.Pin 	= GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
	LED.Mode	= GPIO_MODE_OUTPUT_PP;
	LED.Pull 	= GPIO_NOPULL;
	LED.Speed 	= GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOC, &LED);
}

void TIMER_Config2(void)
{
	__HAL_RCC_TIM2_CLK_ENABLE();

	TimHandle2.Instance				=	TIM2;
	TimHandle2.Init.Prescaler		=	8400 - 1;
	TimHandle2.Init.Period			=	10000 - 1;
	TimHandle2.Init.ClockDivision	=	TIM_CLOCKDIVISION_DIV2;
	TimHandle2.Init.CounterMode		=	TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&TimHandle2);

	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void TIMER_Config3(void)
{
	__HAL_RCC_TIM3_CLK_ENABLE();

	TimHandle3.Instance				=	TIM3;
	TimHandle3.Init.Prescaler		=	8400 - 1;
	TimHandle3.Init.Period			=	5000 - 1;
	TimHandle3.Init.ClockDivision	=	TIM_CLOCKDIVISION_DIV2;
	TimHandle3.Init.CounterMode		=	TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&TimHandle3);

	HAL_NVIC_SetPriority(TIM3_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

void TIMER_Config4(void)
{
	__HAL_RCC_TIM4_CLK_ENABLE();

	TimHandle4.Instance				=	TIM4;
	TimHandle4.Init.Prescaler		=	8400 - 1;
	TimHandle4.Init.Period			=	10000 - 1;
	TimHandle4.Init.ClockDivision	=	TIM_CLOCKDIVISION_DIV2;
	TimHandle4.Init.CounterMode		=	TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&TimHandle4);

	HAL_NVIC_SetPriority(TIM4_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);
}

void TIMER_Config5(void)
{
	__HAL_RCC_TIM5_CLK_ENABLE();

	TimHandle5.Instance				=	TIM5;
	TimHandle5.Init.Prescaler		=	8400 - 1;
	TimHandle5.Init.Period			=	5000 - 1;
	TimHandle5.Init.ClockDivision	=	TIM_CLOCKDIVISION_DIV1;
	TimHandle5.Init.CounterMode		=	TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&TimHandle5);

	HAL_NVIC_SetPriority(TIM5_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(TIM5_IRQn);
}



int
main(int argc, char* argv[])
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	LED_Config();
	TIMER_Config2();
	TIMER_Config3();
	TIMER_Config4();
	TIMER_Config5();

	HAL_TIM_Base_Start_IT(&TimHandle2);
	HAL_TIM_Base_Start_IT(&TimHandle3);
	HAL_TIM_Base_Start_IT(&TimHandle4);
	HAL_TIM_Base_Start_IT(&TimHandle5);

	while (1)
	{

	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2){
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_0);
	}
	if(htim->Instance == TIM3){
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_1);
	}
	if(htim->Instance == TIM4){
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_2);
	}
	if(htim->Instance == TIM5){
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_3);
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
