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
		// 這邊使用的是查尋RXNE的模式，然後直接用函數看接收到的資料
		/*if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET)
		{
			RxData=USART_ReceiveData(USART1); // 讀完DR以後不用清標誌位，手冊的寄存器裡說的
			OLED_ShowHexNum(1,1,RxData,2);
		}*/
		
		// 這邊使用的是中斷
		if(Serial_GetFlag()==1)
		{
			RxData=Serial_GetRxData();
			Serial_SendByte(RxData);
			OLED_ShowHexNum(1,8,RxData,2);
			
		}
	}
	
}
