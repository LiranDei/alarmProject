/*
 * flash.h
 *
 *  Created on: Oct 23, 2022
 *      Author: student
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

HAL_StatusTypeDef erasePage(int bank, int page, int NbPages);
HAL_StatusTypeDef writeToPage(uint32_t TypeProgram, uint32_t Address, void* data, int dataSize);



#endif /* INC_FLASH_H_ */
