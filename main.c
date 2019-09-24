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
//#include "Base_Peri_F411.h"
//#include "stm32f4xx_it.h"
//#include "stm32f4xx_nucleo.h"

GPIO_InitTypeDef	GPIO_Init_Struct;

// ================================== delay ============================================
#define	usec	12
#define	msec	14000

void delay_int_count(volatile unsigned int nTime)
{
  for(; nTime > 0; nTime--);
}

void F405_LED_Config(void)
{
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_Init_Struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_2 | GPIO_PIN_3;			// GPIO에서 사용할 PIN 설정
	GPIO_Init_Struct.Mode = GPIO_MODE_OUTPUT_PP; 	// Output Push-Pull 모드
	GPIO_Init_Struct.Pull = GPIO_PULLUP; 		// Pull Up 모드
	GPIO_Init_Struct.Speed = GPIO_SPEED_FREQ_HIGH; 	// 동작속도를 HIGH로
	HAL_GPIO_Init(GPIOC, &GPIO_Init_Struct);
}

// ================================ SPI ================================================
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Uncomment this line to use the board as master, if not it is used as slave */
#define MASTER_BOARD

#define	WREN	0x06	// Set Write Enable Latch
#define	WRDI	0x04	// Reset Write Enable Latch
#define	RDSR	0x05	// Read status Register
#define	WRSR	0x01	// Write status Register
#define	READ	0x03	// Read Data from Memory Array
#define	WRITE	0x02	// Write Data from Memory Array

extern GPIO_InitTypeDef	GPIO_Init_Struct;

/* Private variables ---------------------------------------------------------*/
/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;

// -- SPI의 초기설정용 함수의 선언
void F405_SPI_Config(void);

// ================================ UART ===============================================
// -- <1> UART의 초기설정용 함수의 선언
void F405_UART_Config(void);

// -- <2> 주변장치 초기화용 구조체
UART_HandleTypeDef	UartHandle;

// -- <3> UART 통신을 위한 정의
#define TxBufferSize	(countof(TxBuffer) - 1)
#define RxBufferSize	0xFF
#define countof(a)	(sizeof(a) / sizeof(*(a)))

// -- <4> UART 통신용 변수 선언
uint8_t TxBuffer[50] = "\n\rUART Test message...!!! \n\r";
uint8_t RxBuffer[4];
uint8_t read_data[30] = "A";
uint8_t buff[30] = "A";
// =====================================================================================
// SPI Read
unsigned char* spi_Read(uint8_t *pBuff, int len)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_SPI_Receive(&SpiHandle, pBuff, len, 100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
	return pBuff;
}

// SPI Write
int spi_Write(uint8_t *pBuff, int len)
{
	int len_to_return = len;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&SpiHandle, pBuff, len, 100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
	return len_to_return;
}

void EEPROM_TEST_1(void)
{
	// Set Enable
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_Delay(0.001);
	TxBuffer[0] = WREN;
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*) &TxBuffer, 1, 100);
	HAL_Delay(0.001);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_Delay(1);

	// Write EEPROM
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_Delay(0.001);
	TxBuffer[0] = WRITE; TxBuffer[1] = 0x00; TxBuffer[2] = 0x41; TxBuffer[3] = 'F'; //TxBuffer[4] = 'F'; TxBuffer[5] = 'F';
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*) &TxBuffer, 4, 100);
	HAL_Delay(0.001);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_Delay(1);

	//Reset Enable
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_Delay(0.001);
	TxBuffer[0] = WRDI;
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*) &TxBuffer, 1, 100);
	HAL_Delay(0.001);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_Delay(1);

	// Read EEPROM
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_Delay(0.001);
	TxBuffer[0] = READ; TxBuffer[1] = 0x00; TxBuffer[2] = 0x41;	TxBuffer[3] = 0x00;
	HAL_SPI_Receive(&SpiHandle, (uint8_t*) &TxBuffer, 4, 100);
	HAL_Delay(0.001);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_Delay(1);

	buff[0] = TxBuffer[3];
	buff[1] = '\r';
	buff[2] = '\n';
	buff[3] = 0x00;

	HAL_UART_Transmit(&UartHandle, (uint8_t*)buff, 4, 0xFFFF);
}

void EEPROM_TEST_2(void)
{
	// Set Enable
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_Delay(0.001);
	TxBuffer[0] = WREN;
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*) &TxBuffer, 1, 100);
	HAL_Delay(0.001);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_Delay(1);

	// Write EEPROM
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_Delay(0.001);
	TxBuffer[0] = WRITE; TxBuffer[1] = 0x00; TxBuffer[2] = 0x41; TxBuffer[3] = 'G'; //TxBuffer[4] = 'F'; TxBuffer[5] = 'F';
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*) &TxBuffer, 4, 100);
	HAL_Delay(0.001);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_Delay(1);

	//Reset Enable
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_Delay(0.001);
	TxBuffer[0] = WRDI;
	HAL_SPI_Transmit(&SpiHandle, (uint8_t*) &TxBuffer, 1, 100);
	HAL_Delay(0.001);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_Delay(1);

	// Read EEPROM
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_Delay(0.001);
	TxBuffer[0] = READ; TxBuffer[1] = 0x00; TxBuffer[2] = 0x41;	TxBuffer[3] = 0x00;
	HAL_SPI_Receive(&SpiHandle, (uint8_t*) &TxBuffer, 4, 100);
	HAL_Delay(0.001);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_Delay(1);

	buff[0] = TxBuffer[3];
	buff[1] = '\r';
	buff[2] = '\n';
	buff[3] = 0x00;

	HAL_UART_Transmit(&UartHandle, (uint8_t*)buff, 4, 0xFFFF);
}

int main(int argc, char* argv[])
{
  /* Configure LED_1(PA15), LED_2(PB7) */
  F405_LED_Config();

  /* Configure UART */
  F405_UART_Config();
  HAL_UART_Transmit(&UartHandle, (uint8_t*)TxBuffer, TxBufferSize, 0xFFFF);

  /* Configure SPI */
  F405_SPI_Config();

  /* Infinite loop */
  while (1)
  {
	  EEPROM_TEST_1();
	  HAL_Delay(2000);
	  EEPROM_TEST_2();
	  HAL_Delay(2000);
  }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* Turn LED4 on: Transfer in transmission process is correct */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
  /* Turn LED6 on: Transfer in reception process is correct */
  //BSP_LED_On(LED6);
}

 void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  /* Turn LED5 on: Transfer error in reception/transmission process */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
}

// -- SPI의 초기설정을 위한 함수
void F405_SPI_Config(void)
{
	/** Configure pins as
	     PA4   ------> SPI1_CS
	     PA5   ------> SPI1_SCK
	     PA6   ------> SPI1_MISO
	     PA7  ------> SPI1_MOSI
	*/
	// -- SPI의 클럭을 활성화
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* Enable SPI clock */
	__HAL_RCC_SPI1_CLK_ENABLE();

    /** SPI1 GPIO Configuration **/
	GPIO_Init_Struct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	GPIO_Init_Struct.Mode = GPIO_MODE_AF_PP;
	GPIO_Init_Struct.Pull = GPIO_PULLUP;
	GPIO_Init_Struct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_Init_Struct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_Init_Struct);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

	/* NVIC for SPI */
	HAL_NVIC_SetPriority(SPI1_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(SPI1_IRQn);

	// -- SPI의 동작 조건 설정
	SpiHandle.Instance = SPI1;
	SpiHandle.Init.Mode = SPI_MODE_MASTER;
	SpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
	SpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
	SpiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
	SpiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
	SpiHandle.Init.NSS = SPI_NSS_SOFT;
	SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	SpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
	SpiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
	SpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SpiHandle.Init.CRCPolynomial = 7;

	// -- SPI 구성정보를 SpiHandle에 설정된 값으로 초기화 함
	HAL_SPI_Init(&SpiHandle);
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

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
