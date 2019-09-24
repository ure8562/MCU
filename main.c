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
#define START	1
#define UP 		0
#define DOWN	1

unsigned int FND_DATA[] =
	{0x3F00, 0x0600, 0x5B00, 0x4F00, 0x6600,
	 0x6D00, 0x7D00, 0x0700, 0x7F00, 0x6F00 };

GPIO_InitTypeDef SW;
GPIO_InitTypeDef FND;
GPIO_InitTypeDef MOTER;

void ms_delay_int_count(volatile unsigned int nTime)
{
	nTime = (nTime * 14000);
	for(; nTime > 0; nTime--);

}

void fnd_cntl(unsigned char num)
{
	for(int i=8; i<16; i++)
	{
		if(FND_DATA[num] & (0x01<<i))
		{
			HAL_GPIO_WritePin(GPIOC, 0x01<<i , GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOC, 0x01<<i , GPIO_PIN_RESET);
		}
	}
}

void USR_FND_DATA(int i)
{
	switch (i){
		case 0 :
			HAL_GPIO_WritePin(GPIOC, FND.Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
					GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 ,  GPIO_PIN_SET);
			break;
		case 1 :
			HAL_GPIO_WritePin(GPIOC, FND.Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9 | GPIO_PIN_10, GPIO_PIN_SET);
			break;
		case 2 :
			HAL_GPIO_WritePin(GPIOC, FND.Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 |
				GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14, GPIO_PIN_SET);
			break;
		case 3 :
			HAL_GPIO_WritePin(GPIOC, FND.Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
					GPIO_PIN_11 | GPIO_PIN_14, GPIO_PIN_SET);
			break;
		case 4 :
			HAL_GPIO_WritePin(GPIOC, FND.Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9 | GPIO_PIN_10 |
					GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_SET);
			break;
		case 5 :
			HAL_GPIO_WritePin(GPIOC, FND.Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_10 | GPIO_PIN_11 |
					GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_SET);
			break;
		case 6 :
			HAL_GPIO_WritePin(GPIOC, FND.Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_10 | GPIO_PIN_11 |
					GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14,  GPIO_PIN_SET);
			break;
		case 7 :
				HAL_GPIO_WritePin(GPIOC, FND.Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
						GPIO_PIN_13, GPIO_PIN_SET);
			break;
		case 8 :
				HAL_GPIO_WritePin(GPIOC, FND.Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC, FND.Pin, GPIO_PIN_SET);
			break;
		case 9 :
			HAL_GPIO_WritePin(GPIOC, FND.Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
					GPIO_PIN_11 | GPIO_PIN_14 | GPIO_PIN_13 ,  GPIO_PIN_SET);
			break;


	}
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

void FND_Config(void)
{
	FND.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11
			| GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
	FND.Mode = GPIO_MODE_OUTPUT_PP;
	FND.Pull = GPIO_NOPULL;
	FND.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOC, &FND);
}

void MOTER_Config(void)
{
	MOTER.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	MOTER.Mode = GPIO_MODE_OUTPUT_PP;
	MOTER.Pull = GPIO_NOPULL;
	MOTER.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &MOTER);

	MOTER.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &MOTER);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
}

unsigned char FND_sta = UP , count = 0,FND_sta1, MOTER_sta = STOP;

int
main(int argc, char* argv[])
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();


	FND_Config();
	EXTILine_Config();
	MOTER_Config();

	while (1)
	{
		if(FND_sta1 == START)
		{
			for(;;)
			{

				if(FND_sta == UP)
				{
					fnd_cntl(count);
					count++;
					if(count == 10) count=0;
				}
				else if(FND_sta == DOWN)
				{
					fnd_cntl(count);
					if(count == 0)  count=10;
					count--;
				}
				ms_delay_int_count(300);
				if(FND_sta1 ==STOP)
					break;

			}

		}
		/*
		if(FND_sta == STOP)
		{
			HAL_GPIO_WritePin(GPIOC, FND.Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
					GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 , GPIO_PIN_SET);
		}
		*/


	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_12 )
	{
		if(FND_sta1 == STOP || FND_sta ==DOWN)
		{
			FND_sta1 = START;
			FND_sta = UP;
		}
		else
			FND_sta1 = STOP;
	}
	if(GPIO_Pin == GPIO_PIN_13)
	{
		if(FND_sta1 == STOP || FND_sta ==UP)
		{
			FND_sta1 = START;
			FND_sta = DOWN;
		}
		else
			FND_sta1 = STOP;
	}
	if(GPIO_Pin == GPIO_PIN_14)
	{
		if(MOTER_sta == STOP)
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
			MOTER_sta = START;
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
			MOTER_sta = STOP;
		}
	}

}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
