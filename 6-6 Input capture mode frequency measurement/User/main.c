#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"
#include "IC.h"

uint8_t i;

int main(void)
{
	OLED_Init();
	PWM_Init();
	IC_Init();
	
	OLED_ShowString(1,1,"Freq:0000Hz");
	
	// 使用PWM模块提供输入捕获的测试信号(PA0)
	PWM_SetPrescaler(720-1); // Freq = 72M / (PSC + 1) / 100 = 1000 Hz
	PWM_SetCompare1(50); 	 // Duty = CCR / 100 = 50%
	
	while(1)
	{
		OLED_ShowNum(1,6,IC_Getfreq(),5);
	}
}
