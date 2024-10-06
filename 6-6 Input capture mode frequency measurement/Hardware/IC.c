#include "stm32f10x.h"                  // Device header

void IC_Init(void)
{
	// 開啟外設時鐘
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); //将PA0引脚初始化为复用推挽输出	
										   //受外设控制的引脚，均需要配置为复用模式
	
	// 選擇時鐘源
	TIM_InternalClockConfig(TIM3);
	
	// 配置時基單元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		// 时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数器模式，选择向上计数
	// CK_CNT_OV = CK_CNT / (ARR + 1) = CK_PSC / (PSC + 1) / (ARR + 1)
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;					// 计数周期，即ARR的值(自動重裝值)
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;				// 预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0; 			// 重複計數器，高級才要用
	// CNT 可以用SetCounter和GetCounter來操作計時器
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	// 初始化捕獲單元
	TIM_ICInitTypeDef TIM_InitStructure;
	TIM_InitStructure.TIM_Channel = TIM_Channel_1;
	TIM_InitStructure.TIM_ICFilter = 0xF;
	TIM_InitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_InitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_InitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM3, &TIM_InitStructure);
	
	TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_Reset);
	
	TIM_Cmd(TIM3, ENABLE);
}

uint32_t IC_Getfreq(void)
{
	// fc=72M/(PSC+1)=1000000
	return 1000000 / (TIM_GetCapture1(TIM3) + 1);
}