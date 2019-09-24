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

GPIO_InitTypeDef GPIO_Init_Struct;

TIM_HandleTypeDef TimHandle2,TimHandle3;
TIM_OC_InitTypeDef TIM2_OCInit,TIM3_OCInit;

void LED_Config(void)
{
	GPIO_Init_Struct.Pin 	= GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_Init_Struct.Mode	= GPIO_MODE_OUTPUT_PP;
	GPIO_Init_Struct.Pull 	= GPIO_NOPULL;
	GPIO_Init_Struct.Speed 	= GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_Init_Struct);
}

void TIMER_Config2(void)
{
	__HAL_RCC_TIM2_CLK_ENABLE();

	// 타이머 설정 및 주기설정
	TimHandle2.Instance				=	TIM2;
	TimHandle2.Init.Prescaler		=	84 - 1;
	TimHandle2.Init.Period			=	10000 - 1;
	TimHandle2.Init.ClockDivision	=	TIM_CLOCKDIVISION_DIV1;
	TimHandle2.Init.CounterMode		=	TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&TimHandle2);

	//출력비교 모드 및 CCR값 설정
	TIM2_OCInit.OCMode		= TIM_OCMODE_TIMING;
	TIM2_OCInit.Pulse		= 2 - 1;
	HAL_TIM_OC_Init(&TimHandle2);
	HAL_TIM_OC_ConfigChannel(&TimHandle2, &TIM2_OCInit, TIM_CHANNEL_1);

	HAL_NVIC_SetPriority(TIM2_IRQn , 0, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn );
}

void TIMER_Config3(void)
{
	__HAL_RCC_TIM3_CLK_ENABLE();

	// 타이머 설정 및 주기설정
	TimHandle3.Instance				=	TIM3;
	TimHandle3.Init.Prescaler		=	8400 - 1;
	TimHandle3.Init.Period			=	10000 - 1;
	TimHandle3.Init.ClockDivision	=	TIM_CLOCKDIVISION_DIV1;
	TimHandle3.Init.CounterMode		=	TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&TimHandle3);

	//출력비교 모드 및 CCR값 설정
	TIM3_OCInit.OCMode		= TIM_OCMODE_TIMING;
	TIM3_OCInit.Pulse		= 2 - 1;
	HAL_TIM_OC_Init(&TimHandle3);
	HAL_TIM_OC_ConfigChannel(&TimHandle3, &TIM3_OCInit, TIM_CHANNEL_1);

	HAL_NVIC_SetPriority(TIM3_IRQn , 0, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn );

}

void EXTILine_Config(void)
{
	GPIO_Init_Struct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_Init_Struct.Mode = GPIO_MODE_IT_RISING;
	GPIO_Init_Struct.Pull = GPIO_NOPULL;
	GPIO_Init_Struct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_Init_Struct);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

int
main(int argc, char* argv[])
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	LED_Config();
	EXTILine_Config();		//SW0 LED 밝기 높이기 SW1 LED 발기 낮추기
	TIMER_Config2();
	TIMER_Config3();

	HAL_TIM_Base_Start_IT(&TimHandle2);
	HAL_TIM_OC_Start_IT(&TimHandle2,TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&TimHandle3);
	HAL_TIM_OC_Start_IT(&TimHandle3,TIM_CHANNEL_1);

	while (1)
	{
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_12)
	{
		if(TIM2_OCInit.Pulse < 1000)
		{
			TIM2_OCInit.Pulse = 2 - 1;
			HAL_TIM_OC_ConfigChannel(&TimHandle2, &TIM2_OCInit, TIM_CHANNEL_1);
		}
		else
		{
			TIM2_OCInit.Pulse -= 1000;
			HAL_TIM_OC_ConfigChannel(&TimHandle2, &TIM2_OCInit, TIM_CHANNEL_1);
		}
	}
	if(GPIO_Pin == GPIO_PIN_13)
	{
		if(TIM2_OCInit.Pulse > 9000)
		{
			TIM2_OCInit.Pulse = 10000 - 1;
			HAL_TIM_OC_ConfigChannel(&TimHandle2, &TIM2_OCInit, TIM_CHANNEL_1);
		}
		else
		{
			TIM2_OCInit.Pulse += 1000;
			HAL_TIM_OC_ConfigChannel(&TimHandle2, &TIM2_OCInit, TIM_CHANNEL_1);
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2 )
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	if(htim->Instance == TIM3)
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2 )
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
	if(htim->Instance == TIM3)
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
}



#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
