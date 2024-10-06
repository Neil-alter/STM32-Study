#include "stm32f10x.h"

void MySPI_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)BitValue);
}

void MySPI_Init(void)
{
	// 外設時鐘配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	// GPIO配置
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP; // SS，推挽輸出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP; // SCK & MOSI，復用推挽輸出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU; // MISO，上拉輸入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	// SPI配置
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_128; // 72M/128=500...KHz
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;
	SPI_InitStructure.SPI_CRCPolynomial=7; // 無須在意
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b; // 8位數據幀
	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex; // 雙線全雙工
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB; // 高位先行
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master; // 主模式
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;
	SPI_Init(SPI1,&SPI_InitStructure);
	
	// SPI使能
	SPI_Cmd(SPI1,ENABLE);
	
	MySPI_W_SS(1);
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
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)!=SET);   // 發送寄存器為空的時候再寫
																// 可參考手冊，寫DR即清除標誌位
	SPI_I2S_SendData(SPI1,ByteSend);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)!=SET); // 發收完成=接收完成
	return SPI_I2S_ReceiveData(SPI1);
}
