#include "stm32f10x.h"                  // Device header

extern uint16_t Num; // 可以在不同.c裡面使用

void Timer_Init(void)
{
	// 開啟外設時鐘
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	// 選擇時鐘源 = 外部時鐘
	TIM_ETRClockMode2Config(TIM2,TIM_ExtTRGPSC_OFF,TIM_ExtTRGPolarity_NonInverted,0x00); //选择外部时钟模式2，时钟从TIM_ETR引脚输入
																						 //注意TIM2的ETR引脚固定为PA0，无法随意更改
																						 //最后一个滤波器参数加到最大0x0F，可滤除时钟信号抖动
	
	// 配置時基單元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		// 时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数器模式，选择向上计数
	// CK_CNT_OV = CK_CNT / (ARR + 1) = CK_PSC / (PSC + 1) / (ARR + 1)
	TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;					// 计数周期，即ARR的值(自動重裝值)
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;				// 预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0; 			// 重複計數器，高級才要用
	// CNT 可以用SetCounter和GetCounter來操作計時器
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

uint16_t Timer_GetCounter(void)
{
	return TIM_GetCounter(TIM2);
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		Num++;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}