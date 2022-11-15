/*
 * flash.c
 *
 *  Created on: Oct 23, 2022
 *      Author: student
 */

#include "main.h"
#include "flash.h"
#include "alarmInfo.h"


HAL_StatusTypeDef erasePage(int bank, int page, int NbPages)
{
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef flashErase;
	flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
	flashErase.Banks = bank;
	flashErase.Page = page;
	flashErase.NbPages = NbPages;

	uint32_t pageError;
	HAL_FLASHEx_Erase(&flashErase, &pageError);
	if(pageError == 0xFFFFFFFF)
	{
		HAL_FLASH_Lock();
		return HAL_OK;
	}
	else
	{
		HAL_FLASH_Lock();
		return HAL_ERROR;
	}
}

HAL_StatusTypeDef writeToPage(uint32_t TypeProgram, uint32_t Address, void* data, int dataSize)
{
	HAL_StatusTypeDef status;
	HAL_FLASH_Unlock();
	int index = 0;
	while (index < dataSize)
	{
		uint64_t Data =*(uint64_t*)(data+index);
		status = HAL_FLASH_Program(TypeProgram, Address+index, Data);
		if(status != HAL_OK)
		{
			return status;
		}
		index += sizeof(uint64_t);
	}
	HAL_FLASH_Lock();
	//printf("%d\r\n", status);
	return status;
}

