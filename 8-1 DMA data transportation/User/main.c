#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MyDMA.h"

// const uint8_t aa=0x66;
uint8_t DataA[]={0x01,0x02,0x03,0x04};
uint8_t DataB[]={0,0,0,0};

int main(void)
{
	OLED_Init();
	MyDMA_Init((uint32_t)DataA,(uint32_t)DataB,sizeof(DataA)/sizeof(uint8_t));
	
	/*
	// 地址一般習慣用十六進制表示
	OLED_ShowHexNum(1,1,aa,2);
	OLED_ShowHexNum(2,1,(uint32_t)&aa,8); // 取址後應該存在指針變數中，所以需要加上強制類型轉換
	OLED_ShowHexNum(1,1,(uint32_t)&ADC1->DR,8);*/
	// 變數0x2000.../常數是0x08000... 對照PPT裡101頁
	
	OLED_ShowString(1,1,"DataA");
	OLED_ShowString(3,1,"DataB");
	OLED_ShowHexNum(1,8,(uint32_t)DataA,8);
	OLED_ShowHexNum(3,8,(uint32_t)DataB,8);
	
	while(1)
	{
		DataA[0]++;
		DataA[1]++;
		DataA[2]++;
		DataA[3]++;
		
		OLED_ShowHexNum(2,1,DataA[0],2);
		OLED_ShowHexNum(2,4,DataA[1],2);
		OLED_ShowHexNum(2,7,DataA[2],2);
		OLED_ShowHexNum(2,10,DataA[3],2);
		OLED_ShowHexNum(4,1,DataB[0],2);
		OLED_ShowHexNum(4,4,DataB[1],2);
		OLED_ShowHexNum(4,7,DataB[2],2);
		OLED_ShowHexNum(4,10,DataB[3],2);
		
		Delay_ms(1000);
		
		MyDMA_Transfer();
		
		OLED_ShowHexNum(2,1,DataA[0],2);
		OLED_ShowHexNum(2,4,DataA[1],2);
		OLED_ShowHexNum(2,7,DataA[2],2);
		OLED_ShowHexNum(2,10,DataA[3],2);
		OLED_ShowHexNum(4,1,DataB[0],2);
		OLED_ShowHexNum(4,4,DataB[1],2);
		OLED_ShowHexNum(4,7,DataB[2],2);
		OLED_ShowHexNum(4,10,DataB[3],2);
		
		Delay_ms(1000);
	}
	
}
