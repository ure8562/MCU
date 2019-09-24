#include "stm32F4xx.h"
#include <stdio.h>
#include <stdlib.h>

void bong_fnd(int num)
{
	unsigned int fnd_data[] =
			{0x3F00, 0x0600, 0x5B00, 0x4F00, 0x6600,
			0x6D00, 0x7D00, 0x2700, 0x7F00, 0x6F00};
	GPIOC->ODR = fnd_data[num];
}

void bong_moter(int num)
{
	// 0~3 값을 받아드린다.
	// 0 : ccw, 1 : cw, 2 : stop, 3 : break 로 설정한다. 
	switch(num){
	case 0: GPIOB->ODR = 0x00000100;		//ccw
	break;
	case 1:	GPIOB->ODR = 0x00000200;		//cw
	break;
	case 2: GPIOB->ODR = 0x00000000;		//stop
	break;
	case 3:	GPIOB->ODR = 0x00000300;		//break
	break;
	}

}

void bong_led(int port_num, *C_ODR)
{
	*C_ODR |= 0x00000001 << port_num;
}
