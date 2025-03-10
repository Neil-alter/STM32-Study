#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"

uint8_t RxData;

int main(void)
{
	OLED_Init();
	Serial_Init();
	
	OLED_ShowString(1,1,"RxData:");
	
	while(1)
	{
		/*if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET)
		{
			RxData=USART_ReceiveData(USART1);
			OLED_ShowHexNum(1,1,RxData,2);
		}*/
		if(Serial_GetFlag()==1)
		{
			RxData=Serial_GetRxData();
			Serial_SendByte(RxData);
			OLED_ShowHexNum(1,8,RxData,2);
			
		}
	}
	
}
