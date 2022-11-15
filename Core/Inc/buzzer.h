/*
 * buzzer.h
 *
 *  Created on: Oct 22, 2022
 *      Author: USER
 */

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include "main.h"

typedef enum frequency_
{
	NONE = 0,
	DO = 382,
	DOs = 361,
	Re = 340,
	Rs = 322,
	Mi = 303,
	Fa = 286,
	Fs = 270,
	Sol = 255,
	Sols = 241,
	La = 227,
	Las = 219,
	Si = 202
}FREQUENCY;

typedef enum
{
	BUZZER_ON,
	BUZZER_OFF
}BUZZER_STATUS;

typedef struct
{
	BUZZER_STATUS status;
	FREQUENCY freq;
	TIM_HandleTypeDef* pwmTimer;
	int channel;
	int nextNote;
	int delay;
}Buzzer;

void buzzerInit(Buzzer * buzzer, TIM_HandleTypeDef* timer, int channel);
void buzzerTask (void* argu);
void buzzerInitTask();
void buzzerOn(Buzzer * buzzer);
void buzzerOff(Buzzer * buzzer);

#endif /* INC_BUZZER_H_ */
