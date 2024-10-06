#include "stm32f10x.h"

void MySPI_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)BitValue);
}

void MySPI_W_SCK(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_5,(BitAction)BitValue);
}

void MySPI_W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_7,(BitAction)BitValue);
}

uint8_t MySPI_R_MISO(void)
{
	return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
}

void MySPI_Init(void)
{
	// 外設時鐘配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	// GPIO配置
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP; // MOSI和其他pin腳為為推挽輸出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU; // MISO腳為為上拉輸入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	// 默認不選中從機
	MySPI_W_SS(1);
	// 模式0，默認低電平
	MySPI_W_SCK(0);
}

// 起始信號
void MySPI_Start(void)
{
	MySPI_W_SS(0);
}

// 起始信號
void MySPI_Stop(void)
{
	MySPI_W_SS(1);
}

uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
	// 通過交換一個字節接收到的數據
	uint8_t ByteReceive=0x00;
	uint8_t i;
	// 在程式中，從機那裏移入的MISO不歸我們管，主須只需讀取MISO的數據即可
	for(i=0;i<8;i++)
	{
		MySPI_W_MOSI(ByteSend & (0x80 >> i));
		MySPI_W_SCK(1);
		if(MySPI_R_MISO()==1)
		{
			ByteReceive |= (0x80 >> i);
		}
		MySPI_W_SCK(0);
	}
	
	return ByteReceive;
}
