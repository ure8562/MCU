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
#include "lcd.h"
#include <math.h>
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

ADC_HandleTypeDef AdcHandler;
ADC_ChannelConfTypeDef sConfig;
GPIO_InitTypeDef GPIO_Init_Struct;

void ADC_Config(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_Init_Struct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_Init_Struct.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(GPIOA,&GPIO_Init_Struct);

	__HAL_RCC_ADC1_CLK_ENABLE();

	AdcHandler.Instance					=	ADC1;
	AdcHandler.Init.ClockPrescaler		=	ADC_CLOCKPRESCALER_PCLK_DIV2;
	AdcHandler.Init.Resolution			=	ADC_RESOLUTION_12B;
	AdcHandler.Init.DataAlign			=	ADC_DATAALIGN_RIGHT;
	AdcHandler.Init.ScanConvMode		=	DISABLE;
	AdcHandler.Init.ContinuousConvMode	=	ENABLE;
	AdcHandler.Init.NbrOfConversion		=	1;
	AdcHandler.Init.ExternalTrigConv	=	ADC_SOFTWARE_START;
	HAL_ADC_Init(&AdcHandler);

	sConfig.Channel 		= ADC_CHANNEL_0;
	sConfig.Rank			= 1;
	sConfig.SamplingTime	= ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&AdcHandler, &sConfig);
}

uint32_t	adc_value = 0;
uint32_t	voltage	  = 0;
char output_buffer[256] = {0,};

int
main(int argc, char* argv[])
{
	LED_Config();

	ADC_Config();

	CLCD_Config();

	CLCD_init();
  while (1)
    {

	  	  HAL_ADC_Start(&AdcHandler);
	  	  HAL_ADC_PollForConversion(&AdcHandler, 10);	//변화이 완료될떄 까지 대기 blocking 함수
	  	  adc_value = HAL_ADC_GetValue(&AdcHandler);

	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, 1);
	  	  us_delay_int_count(adc_value);


	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, 0);
	  	  us_delay_int_count(3000);

	  	  sprintf(output_buffer, "Value : %04d  ", adc_value);
	  	  CLCD_write(0,0x80);
	  	  CLCD_PUT_STRING(output_buffer);

	  	  voltage = round((5000.0/4096.0)*adc_value);
	  	  sprintf(output_buffer, "Voltage : %04dmV", (int)voltage);
	  	  CLCD_write(0,0xC0);
	  	  CLCD_PUT_STRING(output_buffer);
    }
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
