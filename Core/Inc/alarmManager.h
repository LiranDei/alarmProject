/*
 * alarmManager.h
 *
 *  Created on: Oct 21, 2022
 *      Author: USER
 */

#ifndef INC_ALARMMANAGER_H_
#define INC_ALARMMANAGER_H_

#include "alarmInfo.h"

#define VERBOSITY_RUN 0
#define VERBOSITY_ERROR 1
#define VERBOSITY_DEBUGGER 2
#define VERBOSITY VERBOSITY_DEBUGGER

#if VERBOSITY >= VERBOSITY_RUN
#define LOG_RUN(FORMAT, ...) printf(FORMAT, __VA_ARGS__)
#else
#define LOG_RUN(FORMAT, ...)
#endif

#if VERBOSITY >= VERBOSITY_ERROR
#define LOG_ERROR(FORMAT, ...) printf(FORMAT, __VA_ARGS__)
#else
#define LOG_ERROR(FORMAT, ...)
#endif

#if VERBOSITY >= VERBOSITY_DEBUGGER
#define LOG_DEBUGGER(FORMAT, ...) printf(FORMAT, __VA_ARGS__)
#else
#define LOG_DEBUGGER(FORMAT, ...)
#endif

void alarmAppMain();
void addAlarm(Alarm* alarm);
void deleteAlarm(char* name);
void printAlarms();
int nameAlreadyExists(char *alarmName);
void alarmTask(void *argument);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void turnOffAlarm();
void clearList();
void readFromFlash();
void writeToFlash();
#endif /* INC_ALARMMANAGER_H_ */
