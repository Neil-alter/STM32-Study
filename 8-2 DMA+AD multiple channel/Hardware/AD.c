#include "stm32f10x.h"                  // Device header

uint16_t ADValue[4];

void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	// ADC初始化
	// 點菜菜單放這裡，因為是單次掃描模式(ADC掃描模式+DMA轉運)
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_55Cycles5); // Rank 永遠是1
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,3,ADC_SampleTime_55Cycles5); // Rank 永遠是1
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,4,ADC_SampleTime_55Cycles5);  // Rank 永遠是1
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;
	ADC_InitStructure.ADC_NbrOfChannel=4; // 掃描模式才有用，告訴廚師我點了4道菜
	ADC_InitStructure.ADC_ScanConvMode=ENABLE;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	// DMA初始化
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_BufferSize=4; // 自動重裝值
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC; // A->B
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t)ADValue; 
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable; // 地址自增
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal; // 單次/循環重裝
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)&ADC1->DR; // 4001244C
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord; // 只需要低16位的，高16位的是ADC2(STM32沒有)不用管
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable; // 地址不自增，因為是始終轉運ADC->DR暫存器的數據
	DMA_InitStructure.DMA_Priority=DMA_Priority_Medium; // 多個通道才會用到，這邊先隨便選
	DMA_Init(DMA1_Channel1,&DMA_InitStructure); // 硬體觸發，只能選通道1，對應到PPT的104頁
	
	
	DMA_Cmd(DMA1_Channel1,DISABLE); // 等調用MyDMA_Tranfer之後再進行轉運(硬體觸發才會工作)
	ADC_DMACmd(ADC1,ENABLE); // 開啟ADC的DMA觸發信號
	ADC_Cmd(ADC1,ENABLE); // 開啟ADC電源
	// ADC校準
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1)==SET);
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1)==SET);
}

void AD_GetValue(void)
{
	DMA_Cmd(DMA1_Channel1,DISABLE); // 要重新給傳輸計數器賦值前，需先把DMA失能
	DMA_SetCurrDataCounter(DMA1_Channel1,4);
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE); // 軟體觸發ADC開始
	// 因為轉運總是在轉換之後，所以不需要判斷ADC轉換是否完成
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET);
	DMA_ClearFlag(DMA1_FLAG_TC1);
}