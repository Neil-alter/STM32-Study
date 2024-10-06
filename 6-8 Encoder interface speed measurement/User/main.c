#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"
#include "Encoder.h"

uint16_t Num;
int16_t Speed;

int main(void)
{
	OLED_Init();
	Encoder_Init();
	//Timer_Init();
	
	OLED_ShowString(1,1,"Speed:");
	
	while(1)
	{
		OLED_ShowSignedNum(1,5,Encoder_Get(),5);
	}
	
}
