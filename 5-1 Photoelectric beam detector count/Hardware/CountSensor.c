#include "stm32f10x.h"                  // Device header
#include "Delay.h"

uint16_t CountSensor_Count;

void CountSensor_Init(void)
{
	// 配置 RCC 時鐘
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// EXTI(不知道為什麼，可能有設計考量) 和 NVIC 不需要配置時鐘，因為一直都是打開著的，NVIC是核心的外設，
	// 核心的外設都不需要開啟時鐘
	
	// 配置 GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// 配置 AFIO
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14); // AFIO 函式這邊包在 GPIO 裡面
	
	// 配置 NVIC
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 下降沿觸發
	EXTI_Init(&EXTI_InitStructure);
	
	// 在 misc.h 核心雜項
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 分組，基本上整個程式指需要執行一次就行
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
}

uint16_t CountSensor_Get(void)
{
	return CountSensor_Count;
}

// 中斷函數自動執行，都是無參數無返回值的
void EXTI15_10_IRQHandler(void) // 在 Start 資料夾裡面找中斷向量表
{
	if(EXTI_GetITStatus(EXTI_Line14) == SET) // 看 Pin14 的 Flag 是不是 1，看有沒有進入這個中斷
	{
		Delay_ms(50);
		// 不要讓程式卡死在中斷函數裡，所以要把 Flag 清掉
		EXTI_ClearITPendingBit(EXTI_Line14);
		CountSensor_Count++;
	}
}