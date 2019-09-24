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
#include "stm32f4xx_hal.h"
#include "diag/Trace.h"

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

GPIO_InitTypeDef GPIO_Init_Struct;
UART_HandleTypeDef UartHandler;

uint8_t TxBuffer[]	= "UART Example1(Transmission Success !!\n\r";
uint8_t TxBuffer_size = sizeof(TxBuffer);
uint8_t RxBuffer[256] ;

void UART1_Config(void)
{
	//	GPIO setup
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_Init_Struct.Pin 		= GPIO_PIN_9|GPIO_PIN_10;
	GPIO_Init_Struct.Mode		= GPIO_MODE_AF_PP;
	GPIO_Init_Struct.Alternate	= GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_Init_Struct);

	//	USART1 setup
	__HAL_RCC_USART1_CLK_ENABLE();
	UartHandler.Instance		= USART1;
	UartHandler.Init.BaudRate	= 9600;
	UartHandler.Init.WordLength	= UART_WORDLENGTH_8B;
	UartHandler.Init.StopBits	= UART_STOPBITS_1;
	UartHandler.Init.Parity		= UART_PARITY_NONE;
	UartHandler.Init.HwFlowCtl	= UART_HWCONTROL_NONE;
	UartHandler.Init.Mode		= UART_MODE_TX_RX;
	UartHandler.Init.OverSampling	= UART_OVERSAMPLING_16;

	HAL_UART_Init(&UartHandler);

	HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}

int main(int argc, char* argv[])
{
	UART1_Config();

	HAL_UART_Transmit(&UartHandler, TxBuffer, TxBuffer_size, 0xffff);

	HAL_UART_Receive_IT(&UartHandler, RxBuffer, 1);

	while (1)
	{

	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		HAL_UART_Transmit(huart, RxBuffer, 1, 5);
		HAL_UART_Receive_IT(huart, RxBuffer, 1);
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
