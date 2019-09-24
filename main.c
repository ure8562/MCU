//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

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

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
//#include "Base_Peri_F405.h"
//#include "stm32f4xx_it.h"
//#include "stm32f4xx_nucleo.h"

unsigned char FND_data[10] = {0xFC,0x60,0xDA,0xF2,0x66,0xB6,0x3E,0xE0,0xFE,0xF6};

// ================================== delay ============================================
#define	usec	12
#define	msec	14000

void delay_int_count(volatile unsigned int nTime)
{
  for(; nTime > 0; nTime--);
}

// SHT20
#define	TEMP							0x00
#define HUMI							0x01
#define SHT20_I2C_ADDR             		0x40<<1
#define SHT20_I2C_T_MEASURE_HOLD 		0xE3
#define SHT20_I2C_RH_MEASURE_HOLD 		0xE5
#define SHT20_I2C_T_MEASURE_NOHOLD 		0xF3
#define SHT20_I2C_RH_MEASURE_NOHOLD 	0xF5
#define SHT20_WRITE_USER_REG       		0xE6
#define SHT20_READ_USER_REG       		0xE7
#define SHT20_SOFT_RESET           		0xFE

// ================================ I2C ================================================
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Uncomment this line to use the board as master, if not it is used as slave */
#define MASTER_BOARD
#define I2C_ADDRESS        SHT20_I2C_ADDR

GPIO_InitTypeDef	GPIO_Init_Struct;

/* Private variables ---------------------------------------------------------*/
/* I2C handler declaration */
I2C_HandleTypeDef I2CxHandle;

uint8_t I2C_TxBuffer[5];
uint8_t I2C_RxBuffer[5];

// -- I2C의 초기설정용 함수의 선언
void F405_I2C_Config(void);

int Read_SHT20(int sel);

// ================================ UART ===============================================
// -- <1> UART의 초기설정용 함수의 선언
void F405_UART_Config(void);

// -- <2> 주변장치 초기화용 구조체
UART_HandleTypeDef	UartHandle;

// -- <3> UART 통신을 위한 정의
#define TxBufferSize	(countof(UART_TxBuffer) - 1)
#define RxBufferSize	0xFF
#define countof(a)	(sizeof(a) / sizeof(*(a)))

// -- <4> UART 통신용 변수 선언
uint8_t UART_TxBuffer[50] = "\n\rUART Test message...!!! \n\r";
uint8_t UART_RxBuffer[4];
uint8_t read_data[30] = "A";
uint8_t buff[30] = "A";

int main(int argc, char* argv[])
{
	/* Configure UART */
	F405_UART_Config();
	HAL_UART_Transmit(&UartHandle, (uint8_t*)UART_TxBuffer, TxBufferSize, 0xFFFF);

	/* Configure I2C */
	F405_I2C_Config();

	I2C_TxBuffer[0] = SHT20_SOFT_RESET;
	HAL_I2C_Master_Transmit(&I2CxHandle, (uint16_t)I2C_ADDRESS, (uint8_t*)I2C_TxBuffer, 1, 100);
	HAL_Delay(0.01);

	int temp_value = 0;
	int humi_value = 0;
	int temp_1 = 0;

	/* Infinite loop */
	while (1)
	{
		temp_value = Read_SHT20(TEMP);
		sprintf(buff, "Temp : %03d, ", temp_value);
		HAL_UART_Transmit(&UartHandle, (uint8_t*)buff, (countof(buff) - 1), 0xFFFF);
		HAL_Delay(1000);

		humi_value = Read_SHT20(HUMI);
		sprintf(buff, "Humi : %03d\n\r", humi_value);

		HAL_UART_Transmit(&UartHandle, (uint8_t*)buff, (countof(buff) - 1), 0xFFFF);
		HAL_Delay(500);
	}
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
{
  /** Error_Handler() function is called when error occurs.
  * 1- When Slave don't acknowledge it's address, Master restarts communication.
  * 2- When Master don't acknowledge the last data transferred, Slave don't care in this example.
  */
  if (HAL_I2C_GetError(I2cHandle) != HAL_I2C_ERROR_AF)
  {
    //Error_Handler();
  }
}

// -- I2C의 초기설정을 위한 함수
void F405_I2C_Config(void)
{
	/** Configure pins as
	     PB6   ------> I2C2_SCL
	     PB7   ------> I2C2_SDA
	*/
	// I2C의 클럭을 활성화
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C1_CLK_ENABLE();	// I2C2

	// GPIO B포트 6,7번 핀을 I2C 전환기능으로 설정
	GPIO_Init_Struct.Pin = GPIO_PIN_6|GPIO_PIN_7;		// GPIO에서 사용할 PIN 설정
	GPIO_Init_Struct.Mode = GPIO_MODE_AF_OD;			// Alternate Function Push-Pull 모드
	GPIO_Init_Struct.Pull = GPIO_PULLUP;				// Pull Up 모드
	GPIO_Init_Struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init_Struct.Alternate = GPIO_AF4_I2C1;			// I2C2 Alternate Function mapping
	HAL_GPIO_Init(GPIOB, &GPIO_Init_Struct);

	// I2C의 동작 조건 설정
	I2CxHandle.Instance 			= I2C1;
	I2CxHandle.Init.ClockSpeed 		= 400000;
	I2CxHandle.Init.DutyCycle 		= I2C_DUTYCYCLE_16_9;
	I2CxHandle.Init.OwnAddress1 	= I2C_ADDRESS;
	I2CxHandle.Init.AddressingMode	= I2C_ADDRESSINGMODE_7BIT;
	I2CxHandle.Init.DualAddressMode	= I2C_DUALADDRESS_DISABLE;
	I2CxHandle.Init.OwnAddress2 	= 0;
	I2CxHandle.Init.GeneralCallMode	= I2C_GENERALCALL_DISABLE;
	I2CxHandle.Init.NoStretchMode 	= I2C_NOSTRETCH_DISABLE;

	// I2C 구성정보를 I2CxHandle에 설정된 값으로 초기화 함
	HAL_I2C_Init(&I2CxHandle);
}

int Read_SHT20(int sel)
{
	float sensor_value = 0.0;
		uint32_t val;

		if(sel == 0)	// TEMP
		{
			I2C_TxBuffer[0] = SHT20_I2C_T_MEASURE_NOHOLD;
			HAL_I2C_Master_Transmit(&I2CxHandle, (uint16_t)I2C_ADDRESS, (uint8_t*)I2C_TxBuffer, 1, 100);
			HAL_Delay(260);
			HAL_I2C_Master_Receive(&I2CxHandle, (uint16_t)I2C_ADDRESS, (uint8_t*)I2C_RxBuffer, 3, 100);
			val = I2C_RxBuffer[0]<<8 | I2C_RxBuffer[1];
			sensor_value = -46.85+175.72/65536*(int)val;
		}
		else if(sel == 1)	// HUMI
		{
			I2C_TxBuffer[0] = SHT20_I2C_RH_MEASURE_NOHOLD;
			HAL_I2C_Master_Transmit(&I2CxHandle, (uint16_t)I2C_ADDRESS, (uint8_t*)I2C_TxBuffer, 1, 100);
			HAL_Delay(260);
			HAL_I2C_Master_Receive(&I2CxHandle, (uint16_t)I2C_ADDRESS, (uint8_t*)I2C_RxBuffer, 3, 100);
			val = I2C_RxBuffer[0]<<8 | I2C_RxBuffer[1];
			sensor_value = -6.0+125.0/65536*(int)val;
		}

		return (int)sensor_value;
}

// -- UART의 초기설정을 위한 함수
void F405_UART_Config(void)		// USART1_TX(PA15), USART1_RX(PB7)
{
	// -- <9> UART의 클럭을 활성화
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART1_CLK_ENABLE();

	// -- <10> GPIO A포트 15번 핀을 UART Tx로 설정
	GPIO_Init_Struct.Pin	= GPIO_PIN_9 | GPIO_PIN_10;
	GPIO_Init_Struct.Mode	= GPIO_MODE_AF_PP;
	GPIO_Init_Struct.Pull	= GPIO_NOPULL;
	GPIO_Init_Struct.Speed	= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init_Struct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_Init_Struct);

	// -- <12> UART의 동작 조건 설정
	UartHandle.Instance			= USART1;
	UartHandle.Init.BaudRate	= 9600;
	UartHandle.Init.WordLength	= UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits	= UART_STOPBITS_1;
	UartHandle.Init.Parity		= UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl	= UART_HWCONTROL_NONE;
	UartHandle.Init.Mode		= UART_MODE_TX_RX;
	UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

	// -- <13> UART 구성정보를 UartHandle에 설정된 값으로 초기화 함
	HAL_UART_Init(&UartHandle);

	//USART2->BRR = 0x00001117;
}

// ----------------------------------------------------------------------------
