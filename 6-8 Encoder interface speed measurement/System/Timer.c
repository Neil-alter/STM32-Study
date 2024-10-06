#include "stm32f10x.h"                  // Device header
#include "Encoder.h"

extern uint16_t Num; // 可以在不同.c裡面使用
extern int16_t Speed;

void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_InternalClockConfig(TIM2); // 時鐘源，選擇為內部時鐘
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	// CK_CNT_OV = CK_CNT / (ARR + 1) = CK_PSC / (PSC + 1) / (ARR + 1)
	TIM_TimeBaseInitStructure.TIM_Period = 10000-1; // ARR 自動重裝器
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200-1; // PSC 預分頻器
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0; // 重複計數器，高級才要用
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);//TIM_TimeBaseInit函數末尾，手動產生了更新事件
										 //若不清除此標誌位，則開啟中斷後，會立刻進入一次中斷
										 //如果不介意此問題，則不清除此標誌位也可
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // 開啟TIM2的更新中斷
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE); // 使能定時器開始運行
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		Speed = Encoder_Get();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}