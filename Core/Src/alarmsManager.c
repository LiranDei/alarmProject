/*
 * alarmsManager.c
 *
 *  Created on: Oct 21, 2022
 *      Author: USER
 */

#include "main.h"
#include "alarmManager.h"
#include "alarmInfo.h"
#include "rtc.h"
#include "button.h"
#include "cli.h"
#include "led.h"
#include "cmsis_os.h"
#include "buzzer.h"
#include "flash.h"
#include <string.h>
#include <stdio.h>

#define MAX_ALARMS 10
#define MAX_ALARM_TIME 30 // 30 seconds alarm until stop alarm
#define SNOOZE_DELAY_TIME 30
#define PAGE_256 0x08080000
#define BANK_IN_USED 2
Rtc rtc;
Button rightBtn;
Button leftBtn;
Led blueLed;
Buzzer buzzer;
osMutexId_t rtcMutex;
int alarmStatus = OFF;


extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim3;
extern  ADC_HandleTypeDef hadc2;
extern TIM_HandleTypeDef htim2;

Alarm alarmsArr[MAX_ALARMS];
static uint8_t countAlarms = 0;



static void startAlarm(int alarmIndex)
{
	printf("\r\nalarm %s on\r\n", alarmsArr[alarmIndex].alarmName);
	buzzerOn(&buzzer);
	startLed(&blueLed);
	alarmStatus = alarmIndex; // save the alarm index
	osMutexAcquire(rtcMutex, osWaitForever);
	alarmsArr[alarmIndex].alarmStartTime = rtcGetSeconds(&rtc);
	osMutexRelease(rtcMutex);
}

static int getCurrSec()
{
	int currSec;
	osMutexAcquire(rtcMutex, osWaitForever);
	currSec = rtcGetSeconds(&rtc);
	osMutexRelease(rtcMutex);
	return currSec;
}

void turnOffAlarm()
{
	buzzerOff(&buzzer);
	stopLed(&blueLed);
	LOG_DEBUGGER("alarm %s turned off\r\n", alarmsArr[alarmStatus].alarmName);
	alarmsArr[alarmStatus].alarmActive = OFF;
	alarmsArr[alarmStatus].alarmStartTime = 0;
	alarmStatus = OFF;
	//writeToFlash();// need to fix thats when interrupt calls this func, then we get some error.
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == rightBtn.gpioPin) // the user click on the btn for turn off the alarm
	{
		if(alarmStatus != OFF)// there is some alarm in the background in index alarmStatus
		{
			alarmsArr[alarmStatus].alarmSnooze = OFF;
			turnOffAlarm();
			LOG_DEBUGGER("right btn pressed - alarm stop%s \r\n", "");
		}
	}
	if(GPIO_Pin == leftBtn.gpioPin)
	{
		if(alarmStatus != OFF)
		{
			alarmsArr[alarmStatus].alarmSnooze = ON;
			alarmsArr[alarmStatus].alarmStopTime = getCurrSec();
			turnOffAlarm();
			LOG_DEBUGGER("left btn pressed - alarm snooze on%s \r\n", "");
		}
	}
}


void alarmAppMain()
{
	 buttonInit(&rightBtn, RIGHT_BTN, rightBtn_GPIO_Port, rightBtn_Pin);
	 buttonInit(&leftBtn, LEFT_BTN, leftBtn_GPIO_Port, leftBtn_Pin);
	 //ledInit(&blueLed,blueLed_GPIO_Port, blueLed_Pin); // before used in pwm
	 ledInitPwm(&blueLed);// for pwm led
	 rtcInit(&rtc, &hi2c1, 0XD0);
	 buzzerInit(&buzzer, &htim3, TIM_CHANNEL_1);
	 cliInit();
	 rtcMutex = osMutexNew(NULL);
	 HAL_ADC_Start_IT(&hadc2);
	 HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
	 HAL_ADC_Start_IT(&hadc2);
	 readFromFlash();
	 //printAlarms();
}

void readFromFlash()
{
	Alarm* data = (Alarm *)(PAGE_256);
	while(data->inUse == ALARM_IN_USED)
	{
		memcpy(&alarmsArr[countAlarms], data, sizeof(Alarm));
		countAlarms++;
		data++;
	}
}

void writeToFlash()
{
	HAL_StatusTypeDef status;
	status = erasePage(BANK_IN_USED, PAGE_256, 1);
	if(status != HAL_OK)
	{
		LOG_ERROR("error in erase page in line %d in file %s\r\n", __LINE__, __FILE__);
	}

	status = writeToPage(FLASH_TYPEPROGRAM_DOUBLEWORD, PAGE_256, &alarmsArr, (sizeof(Alarm)) * MAX_ALARMS);
	if(status != HAL_OK)
	{
		LOG_ERROR("error in write to page in line %d in file %s\r\n", __LINE__, __FILE__);
	}
	else
	{
		LOG_DEBUGGER("saved in flash in line %d in file %s \r\n", __LINE__, __FILE__);
	}
}

void addAlarm(Alarm* alarm)
{
	if(countAlarms >= MAX_ALARMS)
	{
		LOG_RUN("list is full, please remove any alarm before adding %s \r\n","");
		return;
	}
	memcpy(&alarmsArr[countAlarms], alarm, sizeof(Alarm));
	countAlarms++;
	LOG_RUN("added alarm successfully %s \r\n", "");
	writeToFlash();
}

void deleteAlarm(char* name)
{
	int delete = 0;
	for(int i = 0; i < countAlarms; i++)// fix thats method while using nameAlreadyExists
	{
		if(strcmp(alarmsArr[i].alarmName,name) == 0)
		{

			alarmsArr[i] = alarmsArr[countAlarms-1];
			memset(&alarmsArr[countAlarms-1], 0, sizeof(Alarm));
			countAlarms--;
			delete = 1;
			break;
		}
	}
	delete > 0? LOG_RUN("deleted alarm successfully %s\r\n",""): LOG_RUN("Name not exists %s\r\n","");
	writeToFlash();
}

void clearList()
{
	int numAlarms = countAlarms-1;
	for(int i = numAlarms; i >= 0; i--)
	{
		memset(&alarmsArr[i], 0, sizeof(Alarm));
		countAlarms--;
		//printf("countAlarms == %u\r\n", countAlarms);
	}
	writeToFlash();
	LOG_RUN("list clear %s\r\n", "");

}

void printAlarms()
{
	printf("\r\n--------------------LIST--------------------\r\n");
	if(countAlarms == 0)
	{
		LOG_RUN("Alarms list is empty %s \r\n", "");
		printf("--------------------------------------------\r\n");
		return;
	}
	for(int i = 0; i < countAlarms; i++)
	{
		LOG_RUN("%d.%s: %02d:%02d:%02d ", i+1, alarmsArr[i].alarmName, alarmsArr[i].alarmTime.hours,
											alarmsArr[i].alarmTime.min, alarmsArr[i].alarmTime.sec);
		alarmsArr[i].alarmActive == ON ? printf("alarm ON\r\n"): printf("alarm OFF\r\n");
	}
	printf("--------------------------------------------\r\n");
}

int nameAlreadyExists(char *alarmName)
{
	int ret = -1;
	for(int i = 0 ; i < countAlarms; i++){
		if(strcmp(alarmsArr[i].alarmName,alarmName) == 0){
			ret = i;
			break;
		}
	}
	return ret;

}

static int isTimeToAlarm(DateTime* alarmTime, DateTime* currTime)
{
	if(alarmTime->year == currTime->year && alarmTime->month == currTime->month && alarmTime->day == currTime->day)
	{
		if(alarmTime->hours == currTime->hours && alarmTime->min == currTime->min)
		{
			return 1;
		}
	}
	return 0;
}


void alarmTask(void *argument)
{
	DateTime currTime;
	while(1)
	{
		osMutexAcquire(rtcMutex, osWaitForever);
		rtcGetTime(&rtc, &currTime);
		osMutexRelease(rtcMutex);
		if(alarmStatus == OFF) // there is no alarm in the background
		{
			for(int i = 0; i < countAlarms; i++)
			{
				if(alarmsArr[i].alarmSnooze == ON)
				{
					int currSec = getCurrSec();
					int alarmDelayTime = currSec - alarmsArr[i].alarmStopTime;
					LOG_DEBUGGER("alarm delay time - %d seconds\r\n", alarmDelayTime);
					if(alarmDelayTime >= SNOOZE_DELAY_TIME)
					{
						LOG_DEBUGGER("alarm %s snooze\r\n", alarmsArr[i].alarmName);
						alarmsArr[i].alarmSnooze = OFF;
						startAlarm(i);
						break;
					}
				}
				if(isTimeToAlarm(&alarmsArr[i].alarmTime, &currTime))
				{
					if(alarmsArr[i].alarmActive == ON ) // start the alarm only if this alarm on
					{
						startAlarm(i);
						break;
					}
				}
			}
		}
		else // alarm is on
		{
			int currSec;
			int alarmRuningTime;
			currSec = getCurrSec();
			alarmRuningTime = currSec - alarmsArr[alarmStatus].alarmStartTime;
			LOG_DEBUGGER("alarm running time - %d seconds\r\n", alarmRuningTime);
			if(alarmRuningTime >= MAX_ALARM_TIME )
			{
				turnOffAlarm();
			}

		}
		osDelay(1000);
	}
}

