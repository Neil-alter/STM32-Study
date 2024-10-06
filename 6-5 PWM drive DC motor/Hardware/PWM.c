#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
	// 開啟外設時鐘
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); //将PA0引脚初始化为复用推挽输出	
										   //受外设控制的引脚，均需要配置为复用模式
	
	// 選擇時鐘源
	TIM_InternalClockConfig(TIM2);
	
	// 配置時基單元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		// 时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数器模式，选择向上计数
	// CK_CNT_OV = CK_CNT / (ARR + 1) = CK_PSC / (PSC + 1) / (ARR + 1)
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;					// 计数周期，即ARR的值(自動重裝值)
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;				// 预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0; 			// 重複計數器，高級才要用
	// CNT 可以用SetCounter和GetCounter來操作計時器
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;// 模式
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;// 極性
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;// 輸出使能
	TIM_OCInitStructure.TIM_Pulse = 0;// 設置 CCR
	TIM_OC3Init(TIM2,&TIM_OCInitStructure); // PA2 調用 CH3
	
	TIM_Cmd(TIM2, ENABLE); // 使能定時器開始運行
}

void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM2,Compare);
}