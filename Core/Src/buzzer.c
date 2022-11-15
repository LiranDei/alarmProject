/*
 * buzzer.c
 *
 *  Created on: Oct 22, 2022
 *      Author: USER
 */


#include "buzzer.h"
#include "cmsis_os.h"
#include "main.h"

#define MUSIC_SIZE 49
FREQUENCY music[MUSIC_SIZE] = {Sol, Mi, Mi, Fa, Re, Re, DO, Re, Mi, Fa, Sol, Sol, Sol, // little jonathan
								Sol, Mi, Mi, Fa, Re, Re, DO, Mi, Sol, Sol, DO,
								Re, Re, Re, Re, Re, Mi, Fa,
								Mi, Mi, Mi, Mi, Mi, Fa, Sol,
								Sol, Mi, Mi, Fa, Re, Re, DO, Mi, Sol, Sol, DO};


void buzzerInit(Buzzer * buzzer, TIM_HandleTypeDef* timer, int channel)
{
	buzzer->status = BUZZER_OFF;
	buzzer->nextNote = NONE;
	buzzer->pwmTimer = timer;
	buzzer->channel = channel;
	buzzer->delay = 500;
}


void buzzerTask (void* argu)
{
	Buzzer* buzz = (Buzzer*)argu;
	while(1)
	{
		while(buzz->status == BUZZER_OFF)
		{
			osThreadYield();
		}
		int freq = music[buzz->nextNote]/2;
		__HAL_TIM_SET_COUNTER(buzz->pwmTimer,0);
		__HAL_TIM_SET_AUTORELOAD(buzz->pwmTimer, freq);
		__HAL_TIM_SET_COMPARE(buzz->pwmTimer, buzz->channel, (freq/2));
		buzz->nextNote = (buzz->nextNote+1) % MUSIC_SIZE;
		osDelay(buzz->delay);
	}


}

void buzzerOn(Buzzer * buzzer)
{
	buzzer->status = BUZZER_ON;
	HAL_TIM_Base_Start(buzzer->pwmTimer);
	HAL_TIM_PWM_Start(buzzer->pwmTimer, buzzer->channel);
}

void buzzerOff(Buzzer* buzzer)
{
	buzzer->status = BUZZER_OFF;
	HAL_TIM_Base_Stop(buzzer->pwmTimer);
	HAL_TIM_PWM_Stop(buzzer->pwmTimer, buzzer->channel);
	buzzer->nextNote = NONE;
}
