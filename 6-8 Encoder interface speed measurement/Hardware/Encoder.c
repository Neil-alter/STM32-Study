#include "stm32f10x.h"                  // Device header

void Encoder_Init(void)
{
	// 開啟外設時鐘
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	// 不用，因為編碼器接口就是一個但方向控制的外部時鐘
	//TIM_InternalClockConfig(TIM3);
	
	// 配置時基單元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		// 时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 沒用，被編碼器接口託管
	// CK_CNT_OV = CK_CNT / (ARR + 1) = CK_PSC / (PSC + 1) / (ARR + 1)
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;					// 计数周期，即ARR的值(自動重裝值)
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;				// 预分频器，即PSC的值，不分頻(因為編碼器的時鐘直接驅動計數器)
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0; 			// 重複計數器，高級才要用
	// CNT 可以用SetCounter和GetCounter來操作計時器
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	// 初始化捕獲單元
	TIM_ICInitTypeDef TIM_InitStructure;
	TIM_ICStructInit(&TIM_InitStructure); // 默認初始值
	TIM_InitStructure.TIM_Channel = TIM_Channel_1;
	TIM_InitStructure.TIM_ICFilter = 0xF;
	//TIM_InitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 編碼器上下沿都有效，這裡代表的是高低電位極性不反轉
	TIM_ICInit(TIM3, &TIM_InitStructure);
	TIM_InitStructure.TIM_Channel = TIM_Channel_2;
	TIM_InitStructure.TIM_ICFilter = 0xF;
	//TIM_InitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 編碼器上下沿都有效，這裡代表的是高低電位極性不反轉
	TIM_ICInit(TIM3, &TIM_InitStructure);
	
	// 配置編碼器接口
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	
	TIM_Cmd(TIM3,ENABLE);
}

int16_t Encoder_Get(void)
{
	int16_t Temp;
	Temp=TIM_GetCounter(TIM3);
	TIM_SetCounter(TIM3,0);
	return Temp;
}