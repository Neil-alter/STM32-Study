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
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	//方法1，：将通道初始化部分复制一份，结构体定义不需复制
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;//改为通道2，注意！是２不是１！
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;//改为下降沿触发
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI;//交叉输入
	TIM_ICInit(TIM3,&TIM_ICInitStructure);
	/*方法2：使用TIM_PWMIConfig函数,可快捷地把电路配置成PWMI模式的标准结构，
	这个函数只支持通道1和2不支持通道3和4，和方法1的效果是一样的，只需传入一个通道的参数就行了，
	在函数里会自动把剩下的一个通道初始化成相反的配置（比如已经传入了通道1、直连、上升沿，那函数里就会顺带配置通道2、交叉、下降沿)*/
	//(TIM3, &TIM_ICnitStructure);						//将结构体变量交给TIM_PWMIConfig，配置TIM3的输入捕获通道
																	//此函数同时会把另一个通道配置为相反的配置，实现PWMI模式
	
	TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_Reset);
	
	TIM_Cmd(TIM3, ENABLE);
}

uint32_t IC_Getfreq(void)
{
	// fc=72M/(PSC+1)=1000000
	return 1000000 / (TIM_GetCapture1(TIM3) + 1);
}

uint32_t IC_GetDuty(void)
{
	return (TIM_GetCapture2(TIM3)+1)*100/(TIM_GetCapture1(TIM3)+1);
}