/*
 * commTask.h
 *
 *  Created on: Oct 20, 2022
 *      Author: student
 */

#ifndef INC_COMMTASK_H_
#define INC_COMMTASK_H_

#include "main.h"


typedef void (*HandlerFunc)(void*, char*);

typedef struct Command_
{
	const char * name;
	HandlerFunc func;
	void * obj;
} Command;

void registerCommand(const char * name, HandlerFunc func, void * obj);
int commTaskReady(UART_HandleTypeDef* huart2);
void handleCommand();
void initCommTask();
void commTask(void *argument);

#endif /* INC_COMMTASK_H_ */
