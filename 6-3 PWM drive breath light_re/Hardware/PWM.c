#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
	// 外設時鐘始能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			//开启TIM2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// GPIO 初始化
	GPIO_InitTypeDef GPIO_InitTypeDefStructure;
	GPIO_InitTypeDefStructure.GPIO_Mode=GPIO_Mode_AF_PP; // 輸出控制交由片上外設，因此選用復用推挽輸出
	GPIO_InitTypeDefStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitTypeDefStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitTypeDefStructure);
	
	// 配置時鐘源
	TIM_InternalClockConfig(TIM2);		//选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	// 配置時基單元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;					//计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;				//预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	// 配置輸出比較單元，只列出需要用的(高級定時器不需要)
	TIM_OCInitTypeDef TIM_OCInitTypeDefStructure;
	TIM_OCStructInit(&TIM_OCInitTypeDefStructure); // 給結構體賦予初始值，避免初始化不完整
	TIM_OCInitTypeDefStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitTypeDefStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitTypeDefStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitTypeDefStructure.TIM_Pulse=90; // CCR
	TIM_OC1Init(TIM2,& TIM_OCInitTypeDefStructure);
	
	// TIM 使能
	TIM_Cmd(TIM2, ENABLE);			//使能TIM2，定时器开始运行
}

void PWM_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare1(TIM2,Compare);
}
