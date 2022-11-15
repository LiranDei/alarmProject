/*
 * alarmInfo.h
 *
 *  Created on: Oct 21, 2022
 *      Author: USER
 */

#ifndef INC_ALARMINFO_H_
#define INC_ALARMINFO_H_

#include <stdint.h>
#define ALARM_NAME_LENGTH 20
#define ALARM_IN_USED 1

typedef enum alarmState
{
	OFF = -1,
	ON
}AlarmState;

typedef struct DateTime
{
	uint8_t sec;
	uint8_t min;
	uint8_t hours;
	uint8_t weekDay;
	uint8_t day;
	uint8_t month;
	uint8_t year;
	uint8_t spare;

} DateTime;

typedef struct _Alarm
{
	uint8_t inUse;
	char alarmName[ALARM_NAME_LENGTH];
	DateTime alarmTime;
	AlarmState alarmSnooze;
	AlarmState alarmActive;
	uint8_t melodey;
	int alarmStartTime; // start time in second
	int alarmStopTime;
}Alarm;

#endif /* INC_ALARMINFO_H_ */
