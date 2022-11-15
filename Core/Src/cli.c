/*
 * cli.c
 *
 *  Created on: Oct 21, 2022
 *      Author: USER
 */

#include "cli.h"
#include "commTask.h"
#include "alarmInfo.h"
#include "alarmManager.h"
#include "cmsis_os.h"
#include "rtc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern Rtc rtc;
extern osMutexId_t rtcMutex;

static int setDateTime(DateTime *dateTime, char *param) {
	// if string is empty
	if (*param == '\0')
	{
		return 1;
	}
	if(strlen(param) != 17)
	{
		printf("date string length = %d\r\n",strlen(param));
		return 1;
	}
	int ret = 0;
	const char s[4] = ". :";
	char *token;

	/* get the first token */
	token = strtok(param, s);

	/* walk through other tokens */
	do {
		dateTime->day = atoi(token);
		if (dateTime->day < 0 || dateTime->day > 31)
		{
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		dateTime->month = atoi(token);
		if (dateTime->month < 0 || dateTime->month > 12)
		{
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		dateTime->year = atoi(token);
		if (dateTime->year < 0 || dateTime->year > 99)
		{
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		dateTime->hours = atoi(token);
		if (dateTime->hours < 0 || dateTime->hours > 23)
		{
			ret = 1;
			break;
		}

		token = strtok(NULL, s);
		dateTime->min = atoi(token);
		if (dateTime->min < 0 || dateTime->min > 59)
		{
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		dateTime->sec = atoi(token);
		if (dateTime->sec < 0 || dateTime->sec > 59)
		{
			ret = 1;
			break;
		}

	} while (0);

	return ret;

}

static void initDateTime(void* obj, char* param)
{
	Rtc *rtc = (Rtc*)obj;
	DateTime dateTime;
	HAL_StatusTypeDef status;
	char strHelp[] = "-h";
	if(strcmp(param,strHelp) == 0)
	{
		printf("date format\r\n");
		printf("dd.mm.yy HH:MM:SS\r\n");
		return;
	}
	if(setDateTime(&dateTime,param) == 0)
	{
		osMutexAcquire(rtcMutex, osWaitForever);
		status = rtcSetTime(rtc,&dateTime);
		osMutexRelease(rtcMutex);
		if(status == HAL_OK)
		{
			printf("date time set Successfully\r\n");
		}
		else
		{
			printf("Error set date time status code = %d\r\n",status);
		}
	}
	else
	{
		printf("Invalid date time, Try again \r\n");
	}
}

static void showDateCmd(void * obj, char * param)
{
	Rtc *rtc = (Rtc*)obj;
	(void)param;
	DateTime dateTime;
	osMutexAcquire(rtcMutex, osWaitForever);
	rtcGetTime(rtc, &dateTime);
	osMutexRelease(rtcMutex);
	printf("%02d:%02d:%02d %02d.%02d.%02d\r\n",
			dateTime.hours, dateTime.min, dateTime.sec,
			dateTime.day, dateTime.month, dateTime.year);

}

static int insertAlarmToArr(Alarm* alarm, char* param)
{
	int ret = 0;
	const char s[4] = ". :";
	char *token;

	/* get the first token */
	token = strtok(param, s);

	/* walk through other tokens */
	do {
		// CHECK LEN!!!!
		if (strlen(token) > ALARM_NAME_LENGTH){
			ret = 1;
			break;
		}
		if(nameAlreadyExists(token) != -1){
			printf("Alarm name must be unique!!\r\n");
			ret = 1;
			break;
		}
		strcpy(alarm->alarmName,token);
		token = strtok(NULL, s);
		alarm->alarmTime.day = atoi(token);
		if (alarm->alarmTime.day < 0 || alarm->alarmTime.day > 32) {
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		alarm->alarmTime.month = atoi(token);
		if (alarm->alarmTime.month< 0 || alarm->alarmTime.month > 12) {
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		alarm->alarmTime.year = atoi(token);
		if (alarm->alarmTime.year < 0 || alarm->alarmTime.year > 99) {
			ret = 1;
			break;
		}

		token = strtok(NULL, s);
		alarm->alarmTime.hours = atoi(token);
		if (alarm->alarmTime.hours < 0 || alarm->alarmTime.hours > 23) {
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		alarm->alarmTime.min = atoi(token);
		alarm->alarmTime.sec = 0;
		if (alarm->alarmTime.min < 0 || alarm->alarmTime.min > 59) {
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		alarm->melodey = atoi(token);
		if (alarm->melodey < 0 || alarm->melodey > 2) {
			ret = 1;
			break;
		}
		alarm->alarmActive = ON;
		alarm->alarmSnooze = OFF;
		alarm->inUse = 1;

	} while (0);


	return ret;
}

static void addAlarmCmd (void * obj, char* param)
{
	Alarm alarm;
	(void)obj;
	// if string is empty
	if (*param == '\0') {
		return;
	}
	char strHelp[] = "-h";
	if(strcmp(param,strHelp) == 0){
		printf("addAlarm format\r\n");
		printf("[AlarmName] dd.mm.yy HH:MM [melodey 0-2]\r\n");
		return;
	}
	int status;
	status = insertAlarmToArr(&alarm, param);
	if(status == 0)
	{
		addAlarm(&alarm);
	}
}

static void listCmd (void * obj, char* param)
{
	(void)obj;
	(void)param;
	printAlarms();
}

static void delCmd (void* obj, char* param)
{
	(void)obj;
	char strHelp[] = "-h";
	if(strcmp(param,strHelp) == 0){
		printf("delete [Alarm name]\r\n");
		return;
	}
	if (strlen(param) > ALARM_NAME_LENGTH){
		printf("alarm name to long\r\n");
		return;
	}
	deleteAlarm(param);
}

static void stopCmd (void* obj, char* param)
{
	(void)obj;
	(void)param;
	turnOffAlarm();
}

static void clearAllCmd (void* obj, char* param)
{
	(void)obj;
	(void)param;
	clearList();
}


void cliInit()
{
	registerCommand("date",initDateTime,&rtc);
	registerCommand("showDate",showDateCmd,&rtc);
	registerCommand("add",addAlarmCmd,NULL);
	registerCommand("list",listCmd,NULL);
	registerCommand("del",delCmd,NULL);
	registerCommand("stop",stopCmd,NULL);
	registerCommand("clearAll",clearAllCmd,NULL);
	//registerCommand("edit",editCmd,NULL);
	//registerCommand("melody",melodyCmd,NULL);
	//registerCommand("help", helpCmd, NULL);
}
