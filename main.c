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
#include "stm32f4xx_hal.h"

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

GPIO_InitTypeDef GPIO_Init_Struct;
TIM_HandleTypeDef TimHandle2;
TIM_OC_InitTypeDef TIM2_OCInit;

void ms_delay_int_count(volatile unsigned int nTime)
{
	nTime = (nTime * 14000);
	for(; nTime > 0; nTime--);
}

void PIEZO_Config(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_Init_Struct.Pin = GPIO_PIN_2;
	GPIO_Init_Struct.Mode = GPIO_MODE_AF_PP;
	GPIO_Init_Struct.Pull = GPIO_NOPULL;
	GPIO_Init_Struct.Speed = GPIO_SPEED_LOW;
	GPIO_Init_Struct.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(GPIOA, &GPIO_Init_Struct);

}

void TIMER2_config(void)
{
	__HAL_RCC_TIM2_CLK_ENABLE();

	// 타이머 설정 및 주기 설정
	TimHandle2.Instance 			= TIM2;
	TimHandle2.Init.Prescaler		= 84 - 1;
	TimHandle2.Init.Period			= 10000 - 1;
	TimHandle2.Init.ClockDivision 	= TIM_CLOCKDIVISION_DIV1;
	TimHandle2.Init.CounterMode		= TIM_COUNTERMODE_UP;
	HAL_TIM_PWM_Init(&TimHandle2);

	// 출력 비교 모드 및 CCR값 설정
	TIM2_OCInit.OCMode	=	TIM_OCMODE_PWM1;
	TIM2_OCInit.Pulse	=	2-1;
	HAL_TIM_PWM_ConfigChannel(&TimHandle2, &TIM2_OCInit, TIM_CHANNEL_3);
}

void EXTILine_Config(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_Init_Struct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_Init_Struct.Mode = GPIO_MODE_IT_RISING;
	GPIO_Init_Struct.Pull = GPIO_NOPULL;
	GPIO_Init_Struct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_Init_Struct);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}
uint32_t PIEZO_DATA4[8] = {3822, 3405, 3033, 2863, 2551, 2272, 2024, 1991};

void PEZIO_set(uint8_t count)
{
	TimHandle2.Init.Period			= PIEZO_DATA4[count] - 1;
	HAL_TIM_PWM_Init(&TimHandle2);

	TIM2_OCInit.Pulse	=	(PIEZO_DATA4[count]/2) - 1;
	HAL_TIM_PWM_ConfigChannel(&TimHandle2, &TIM2_OCInit, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&TimHandle2, TIM_CHANNEL_3);

}


#define STOP 	0
#define START 	1


//float PIEZO_DATA_4[8] = {261.6256, 293.6648, 329.6276, 349.2282,
//							391.9954, 440.0000, 493.8833, 523.2511 };
uint8_t count = 0, PEIZO_sta = STOP;
int main(int argc, char* argv[])
{

	PIEZO_Config();
	TIMER2_config();
	EXTILine_Config();



	while (1)
	{

	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_12)
	{
		if(count == 0)
			count = 0;
		else
			count--;

	}
	if(GPIO_Pin == GPIO_PIN_13)
	{
		if(count == 7)
			count = 7;
		else
			count++;
	}
	if(GPIO_Pin == GPIO_PIN_14)
	{
		if(PEIZO_sta == STOP)
			PEIZO_sta = START;
		else
			PEIZO_sta = STOP;
	}

	PEZIO_set(count);
}



#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
