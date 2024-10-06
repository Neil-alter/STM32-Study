#include "stm32f10x.h"                  // Device header

extern uint16_t Num; // 可以在不同.c裡面使用，用的是 main.c 裡面的變數

void Timer_Init(void)
{
	// 開啟時鐘
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	// 選擇時基單元，選擇內部時鐘
	TIM_InternalClockConfig(TIM2);
	
	// 配置時基單元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; // 濾波才需要
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period=10000-1; // CK_CNT_OV = CK_CNT / (ARR + 1)
					                              // = CK_PSC / (PSC + 1) / (ARR + 1)

	TIM_TimeBaseInitStructure.TIM_Prescaler=7200-1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0; // 高級定時器用
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	// 中斷輸出配置
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	// NVIC 配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//配置NVIC为分组2
																//即抢占优先级范围：0~3，响应优先级范围：0~3
																//此分组配置在整个工程中仅需调用一次
																//若有多个中断，可以把此代码放在main函数内，while循环之前
																//若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	NVIC_InitTypeDef NVIC_InitTypeDefStructure;
	NVIC_InitTypeDefStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitTypeDefStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitTypeDefStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitTypeDefStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitTypeDefStructure);
	
	// TIM 使能
	TIM_Cmd(TIM2,ENABLE);
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		Num++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}