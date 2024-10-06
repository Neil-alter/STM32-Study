#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

uint8_t Serial_TxPacket[4];
uint8_t Serial_RxPacket[4];
uint8_t Serial_RxFlag;

void Serial_Init(void)
{
	// 外設時鐘配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	// GPIO配置
	// 輸出的話一般使用推挽輸出，輸入的話因為串口狀態空閒是高電位，
	// 所以不使用下拉輸入，選擇上拉輸入
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 復用推挽輸出，忘記可以去複習6-4影片
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// USART配置
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=9600;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStructure);
	
	// 開啟中斷
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE); // 開啟RXNE標誌位到NVIC的輸出
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 分組
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn; // 中斷通道
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1,ENABLE);
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1,Byte);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
}

void Serial_SendArray(uint8_t *Array,uint16_t Length)
{
	uint16_t i;
	for(i=0;i<Length;i++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char* String)
{
	uint8_t i;
	for(i=0;String[i]!='\0';i++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X,uint32_t Y)
{
	uint32_t Result=1;
	while(Y--)
	{
		Result*=X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number,uint8_t Length)
{
	uint8_t i;
	for(i=0;i<Length;i++)
	{
		Serial_SendByte(Number / Serial_Pow(10,Length-i-1)%10 + 0x30); // 0x30('0')是ASCII碼對應的偏移量
	}
}

// fputc是printf的底層，在這裡把函數重定義(override)到串口，那printf自然就輸出到串口了
int fputc(int ch,FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

void Serial_Printf(char *format, ...)
{
	char String[100];				//定义字符数组
	va_list arg;					//定义可变参数列表数据类型的变量arg
	va_start(arg, format);			//从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);					//结束变量arg
	Serial_SendString(String);		//串口发送字符数组（字符串）
}

void Serial_SendPacket(void)
{
	Serial_SendByte(0xFF); // 發送包頭
	Serial_SendArray(Serial_TxPacket,4);
	Serial_SendByte(0xFE);
}

uint8_t Serial_GetRxFlag(void)
{
	if(Serial_RxFlag==1)
	{
		Serial_RxFlag=0;
		return 1;
	}
	return 0;
}

// USART1中斷函數
// 接收數據包，然後把載荷數據存在RxPacket陣列裡
void USART1_IRQHandler(void)
{
	static uint8_t RxState=0;
	static uint8_t pRxPacket=0;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
		uint8_t RxData=USART_ReceiveData(USART1);
		if(RxState==0)
		{
			// 進入等待包頭的程式
			if(RxData==0xFF)
			{
				RxState=1;
				pRxPacket=0;
			}
		}
		else if(RxState==1)
		{
			// 進入接收數據的程式，接收四個數據
			Serial_RxPacket[pRxPacket]=RxData;
			pRxPacket++;
			if(pRxPacket>=4)
			{
				RxState=2;
			}
		}
		else if(RxState==2)
		{
			// 進入等待包尾的程式
			if(RxData==0xFE)
			{
				RxState=0;
				Serial_RxFlag=1;
			}
		}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}
