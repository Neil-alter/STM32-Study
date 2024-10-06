#include "stm32f10x.h"                  // Device header

uint16_t MyDMA_Size;

void MyDMA_Init(uint32_t ADDrA, uint32_t ADDrB, uint16_t Size)
{
	MyDMA_Size=Size;
	// 開啟 RCC 外設時鐘
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	// DMA 初始化
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_BufferSize=Size; // 自動重裝值
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC; // A->B
	DMA_InitStructure.DMA_M2M=DMA_M2M_Enable; // 是否為軟體觸發
	DMA_InitStructure.DMA_MemoryBaseAddr=ADDrB;
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable; // 地址自增
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal; // 單次 or 循環重裝，軟體觸發所以先選 normal
	DMA_InitStructure.DMA_PeripheralBaseAddr=ADDrA;
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Enable; // 地址自增
	DMA_InitStructure.DMA_Priority=DMA_Priority_Medium; // 多個 DMA 通道才會用到，這邊先隨便選
	DMA_Init(DMA1_Channel1,&DMA_InitStructure); // 軟體觸發，所以先不管通道，先隨便設置為通道1
	
	// DMA 使能
	DMA_Cmd(DMA1_Channel1,DISABLE); // 等調用MyDMA_Tranfer之後再進行轉運
}

void MyDMA_Transfer(void)
{
	DMA_Cmd(DMA1_Channel1,DISABLE); // 要重新給傳輸計數器賦值前，需先把DMA失能(手冊規定)
	DMA_SetCurrDataCounter(DMA1_Channel1,MyDMA_Size);
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET);
	DMA_ClearFlag(DMA1_FLAG_TC1);
}