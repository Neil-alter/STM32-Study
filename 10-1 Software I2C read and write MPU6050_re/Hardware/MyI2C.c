#include "stm32f10x.h"                  // Device header
#include "Delay.h"

// 封裝函數方便使用
void MyI2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_10,(BitAction)BitValue);
	Delay_us(10); // 其實不加延時引腳翻轉速度，MPU6050也能跟得上
}

void MyI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_11,(BitAction)BitValue);
	Delay_us(10);
}

uint8_t MyI2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11);
	Delay_us(10);
	return BitValue;
}

void MyI2C_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// GPIO初始化，使用STM GPIO端口模擬I2C輸出
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // SCL和SDA都初始化為開漏輸出模式，此模式仍可以輸入，
													 // 輸入時，先輸出1(釋放SDA線)，再直接輸入數據寄存器即可
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11); // 把SCL和SDA置為高電位，此時I2C總線默認為空閒狀態
}
// 起始
void MyI2C_Start(void)
{
	// 順序不能搞錯，要兼容S和SR起始條件
	// 先拉低SCL和SDA
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	// 然後依序拉低SDA和SCL
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
}
// 終止
void MyI2C_Stop(void)
{
	// 先拉低SDA，再釋放SCL、SDA
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}
// 發送
// 基本上除了終止條件，SCL以高電位結束
// 所有的單元都會保證以低電位結束，這樣方便各個單元的拼接
void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		MyI2C_W_SDA(Byte & (0x80 >> i)); // 取出最高位
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);
	}
}
// 接收
uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t Byte=0x00;
	uint8_t i;
	MyI2C_W_SDA(1); // 主機釋放SDA
	for(i=0;i<8;i++)
	{
		MyI2C_W_SCL(1);
		if(MyI2C_R_SDA()==1) // 我們是在通信，這個數據是從機控制的
		{
			Byte |= (0x80 >> i);
		}
		MyI2C_W_SCL(0);
	}
	return Byte;
}
// 發送應答
void MyI2C_SendAck(uint8_t AckBit)
{
	MyI2C_W_SDA(AckBit);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}
// 接收應答
uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit;
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	AckBit=MyI2C_R_SDA();
	MyI2C_W_SCL(0);
	return AckBit;
}