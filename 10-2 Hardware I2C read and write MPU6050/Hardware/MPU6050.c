#include "stm32f10x.h"                  // Device header
#include "MPU6050_Reg.h"

#define MPU6050_ADDRESS		0xD0

// 用於超時退出死循環之函數
void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout=10000;
	while(I2C_CheckEvent(I2Cx,I2C_EVENT)!=SUCCESS)
	{
		Timeout--;
		if(Timeout==0)
		{
			break; // 超時退出函數
		}
	}
}

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
//	MyI2C_Start();
//	MyI2C_SendByte(MPU6050_ADDRESS);
//	MyI2C_ReceiveAck(); // 暫時不對應答失敗做處理，因為後續處理麻煩。。。
//	MyI2C_SendByte(RegAddress);
//	MyI2C_ReceiveAck();
//	MyI2C_SendByte(Data);
//	MyI2C_ReceiveAck();
//	MyI2C_Stop();
	
	// 硬體I2C跟軟體I2C不同點在於"非阻塞式"，不管波形是否發行完畢，
	// 需要檢查標誌位來確保是否已完成操作
	I2C_GenerateSTART(I2C2,ENABLE);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);
	
	// 在函數中，接收/發送數據都自帶了接收應答的過程
	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Transmitter);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	// 發送數據
	I2C_SendData(I2C2,RegAddress);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	
	I2C_SendData(I2C2,Data);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	I2C_GenerateSTOP(I2C2,ENABLE);
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
//	
//	MyI2C_Start();
//	MyI2C_SendByte(MPU6050_ADDRESS);
//	MyI2C_ReceiveAck();
//	MyI2C_SendByte(RegAddress);
//	MyI2C_ReceiveAck();
//	
//	MyI2C_Start();
//	MyI2C_SendByte(MPU6050_ADDRESS | 0x01); // 讀寫位改為 1
//	MyI2C_ReceiveAck();
//	Data=MyI2C_ReceiveByte();
//	MyI2C_SendAck(1);
//	MyI2C_Stop();
	
	I2C_GenerateSTART(I2C2,ENABLE);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);
	
	// 接收/發送數據都自帶了接收應答的過程
	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Transmitter);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	I2C_SendData(I2C2,RegAddress);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	// 後半段才是PPT.144的內容唷!
	I2C_GenerateSTART(I2C2,ENABLE);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Receiver);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
	
	I2C_AcknowledgeConfig(I2C2,DISABLE);
	I2C_GenerateSTOP(I2C2,ENABLE);
	
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED);
	Data=I2C_ReceiveData(I2C2);
	
	I2C_AcknowledgeConfig(I2C2,ENABLE);
	
	return Data;
}

void MPU6050_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Ack=I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit; // 用於作為從機地址
	I2C_InitStructure.I2C_ClockSpeed=50000;
	I2C_InitStructure.I2C_DutyCycle=I2C_DutyCycle_2; // 時鐘大於100kHz才有用，小魚的話占空比是1:1
	I2C_InitStructure.I2C_Mode=I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1=0x00; // 用於作為從機地址
	I2C_Init(I2C2,&I2C_InitStructure);
	
	I2C_Cmd(I2C2,ENABLE);
	
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01); // 解除睡眠，並且最後三位，選擇X軸的陀螺儀的時鐘
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00); // 六軸均不待機
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x09); // 分頻為10，值越小越快
	MPU6050_WriteReg(MPU6050_CONFIG,0x06); // 濾波參數給最大
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG,18); // 最大量程
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18); // 最大量程
}

uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,
					 int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint16_t DataH,DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX=(DataH << 8) | DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY=(DataH << 8) | DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ=(DataH << 8) | DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX=(DataH << 8) | DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY=(DataH << 8) | DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ=(DataH << 8) | DataL;
}