#include "stm32f10x.h"                  // Device header

/*int main(){
	RCC->APB2ENR=0x00000010; // RCC時鐘使能
	GPIOC->CRH=0x000300000; // 端口配置
	GPIOC->ODR=0x00002000;// 端口輸出數據
}*/

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // 外設時鐘使能寄存器
													      // 使用個個外設前必須開啟時鐘，否則對外社的操作無效
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 通用推挽輸出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; // PC13
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOC, GPIO_Pin_13); // 高電位
	GPIO_ResetBits(GPIOC, GPIO_Pin_13); // 低電位
	while(1) // 為了不讓程式結束，需要加上一個死循環
	{
	
	}
}
