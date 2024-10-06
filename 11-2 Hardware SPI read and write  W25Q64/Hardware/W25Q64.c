#include "stm32f10x.h"
#include "MySPI.h"
#include "W25Q64_Ins.h"

void W25Q64_Init(void)
{
	MySPI_Init();
}

void W25Q64_ReadID(uint8_t *MID,uint16_t *DID)
{
	MySPI_Start();
	MySPI_SwapByte(W25Q64_JEDEC_ID); // 這邊只是發送指令，返回直沒有意義
	*MID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID = MySPI_SwapByte(W25Q64_DUMMY_BYTE); // 接收到廠商ID
	*DID <<= 8; // 接收到高八位設備ID
	*DID |= MySPI_SwapByte(W25Q64_DUMMY_BYTE); // 接收到低八位設備ID
	MySPI_Stop();
}

void W25Q64_WriteEnable(void)
{
	MySPI_Start();
	MySPI_SwapByte(W25Q64_WRITE_ENABLE);
	MySPI_Stop();
}

void W25Q64_WaitBusy(void)
{
	uint32_t Timeout=100000;
	MySPI_Start();
	MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
	while((MySPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01)
	{
		Timeout--;
		if(Timeout==0)
			break;
	}
	MySPI_Stop();
}

void W25Q64_PageProgram(uint32_t Address,uint8_t *DataArray,uint16_t Count) // 調用一個陣列，陣列數量為0~255(256個)
{
	W25Q64_WriteEnable(); // 寫入操作前，必須寫始能
	
	uint16_t i;
	MySPI_Start();
	MySPI_SwapByte(W25Q64_PAGE_PROGRAM);
	MySPI_SwapByte(Address>>16);
	MySPI_SwapByte(Address>>8); // 高位會被捨棄
	MySPI_SwapByte(Address);
	for(i=0;i<Count;i++)
	{
		MySPI_SwapByte(DataArray[i]);
	}
	MySPI_Stop();
	
	W25Q64_WaitBusy(); // 等待
}

void W25Q64_SectorErase(uint32_t Address)
{
	W25Q64_WriteEnable(); // 寫入操作前，必須寫始能
	
	MySPI_Start();
	MySPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
	MySPI_SwapByte(Address>>16);
	MySPI_SwapByte(Address>>8); // 高位會被捨棄
	MySPI_SwapByte(Address);
	MySPI_Stop();
	
	W25Q64_WaitBusy(); // 等待
}

void W25Q64_ReadData(uint32_t Address,uint8_t *DataArray,uint16_t Count) // 讀取模式就沒有頁的數量限制了
{
	uint32_t i;
	MySPI_Start();
	MySPI_SwapByte(W25Q64_READ_DATA);
	MySPI_SwapByte(Address>>16);
	MySPI_SwapByte(Address>>8); // 高位會被捨棄
	MySPI_SwapByte(Address);
	for(i=0;i<Count;i++)
	{
		DataArray[i]=MySPI_SwapByte(W25Q64_DUMMY_BYTE); // 在每次調換交換讀取之後，存儲器內部地址指標都會自動自增
	}
	MySPI_Stop();
}
