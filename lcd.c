#include "stm32f4xx.h"

GPIO_InitTypeDef GPIO_Init_Struct;

void ms_delay_int_count(volatile unsigned int nTime)
{
	nTime = (nTime * 14000);
	for(; nTime > 0; nTime--);
}

void us_delay_int_count(volatile unsigned int nTime)
{
	nTime = (nTime * 12);
	for(; nTime > 0; nTime--);
}

void CLCD_Config()
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_Init_Struct.Pin  	= GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
	GPIO_Init_Struct.Mode 	= GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOB, &GPIO_Init_Struct);

	GPIO_Init_Struct.Pin  	= GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	HAL_GPIO_Init(GPIOC, &GPIO_Init_Struct);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}
void CLCD_write(uint8_t rs, char data)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, rs);	//RS
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);	//ENABLE
	us_delay_int_count(2);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, (data>>4&0x01));
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, (data>>5&0x01));
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, (data>>6&0x01));
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, (data>>7&0x01));

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);	//ENABLE
	us_delay_int_count(2);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
	us_delay_int_count(2);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, (data>>0&0x01));
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, (data>>1&0x01));
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, (data>>2&0x01));
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, (data>>3&0x01));

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);
	us_delay_int_count(2);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
	ms_delay_int_count(2);

}

void CLCD_init(void)
{
	CLCD_write(0, 0x33); //0011 0011	4비트 설정 특수 명령
	CLCD_write(0, 0x32); //0011 0010	4비트 설정 특수 명령
	CLCD_write(0, 0x28); //0010 1000	set function
	CLCD_write(0, 0x0E); //0000 1111	set display
	CLCD_write(0, 0x01); //0000 0001	clcd clear
	CLCD_write(0, 0x06); //0000 0110	set entry mode
	CLCD_write(0, 0x02); //0000 0010	return home

}

void CLCD_PUT_STRING(char *str)
{
	while(*str)
	{
		CLCD_write(1, *str++);
	}
}
