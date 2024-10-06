#include "stm32f10x.h"                  // Device header

void AD_Init(void)
{
	// 外設時鐘配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); // ADCCLK = 72MHz / 6 = 12 MHz
	
	// GPIO 配置
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN; // 模擬輸入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	// ADC配置，連續轉換非掃描模式
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5); // 規則組通道
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None; // 不使用外部觸發，也就是軟體觸發
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;
	ADC_InitStructure.ADC_NbrOfChannel=1; // 掃描模式才有用
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	// ADC 使能
	ADC_Cmd(ADC1,ENABLE);
	
	// ADC 校準
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1)==SET);
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1)==SET);
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}

uint16_t AD_GetValue(void)
{
	return ADC_GetConversionValue(ADC1);
}

/*uint16_t AD_GetValue(void) 這個是非連續轉換
{
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==RESET);
	return ADC_GetConversionValue(ADC1);
}*/