#include "stm32f10x.h"                  // Device header

void IC_Init(void)
{
	// 配置外設時鐘
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	// 配置 GPIO
	GPIO_InitTypeDef GPIO_InitTypeDefStructure;
	GPIO_InitTypeDefStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitTypeDefStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitTypeDefStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitTypeDefStructure);
	
	// 配置TIM時鐘源、時基單元
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitTypeDefStructure;
	TIM_TimeBaseInitTypeDefStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitTypeDefStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitTypeDefStructure.TIM_Period=65536-1; // 設大一點，防止溢出
	TIM_TimeBaseInitTypeDefStructure.TIM_Prescaler=720-1;
	TIM_TimeBaseInitTypeDefStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitTypeDefStructure);
	
	// 配置輸入捕獲單元
	TIM_ICInitTypeDef TIM_ICInitTypeDefStructure;
	TIM_ICInitTypeDefStructure.TIM_Channel=TIM_Channel_1;
	TIM_ICInitTypeDefStructure.TIM_ICFilter=0xF;
	TIM_ICInitTypeDefStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;
	TIM_ICInitTypeDefStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;
	TIM_ICInitTypeDefStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM3,&TIM_ICInitTypeDefStructure);
	
	// 選擇從模式觸發源，執行reset操作
	TIM_SelectInputTrigger(TIM3,TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_Reset);
	
	// 使能 TIM 定時器
	TIM_Cmd(TIM3,ENABLE);
}

uint32_t IC_Getfreq(void)
{
	// fc=72M/(PSC+1)=1000000，使用測周法進行測量
	return 1000000 / (TIM_GetCapture1(TIM3) + 1);
}