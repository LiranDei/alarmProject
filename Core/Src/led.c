/*
 * led.c
 *
 *  Created on: Oct 22, 2022
 *      Author: USER
 */

#include "main.h"
#include "cmsis_os.h"
#include "led.h"
#include <stdio.h>

#define DEFUALT_DELAY 500
#define LIGHT_SENSOR_RESOLUTION 1024.0
extern TIM_HandleTypeDef htim2;
extern  ADC_HandleTypeDef hadc2;
int ledStatus = 0; // for toggle between on \ off
uint32_t lightSensor;



void ledInit(Led* led, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	led->state = STATE_OFF;
	led->GPIOx = GPIOx;
	led->GPIO_Pin = GPIO_Pin;
	led->delay = DEFUALT_DELAY;
}
void ledInitPwm(Led*led)
{
	led->state = STATE_OFF;
	led->delay = DEFUALT_DELAY;
}


void blinkLed(void* argo)
{
	Led* led = (Led*) argo;
	for(;;)
	{
		while(led->state == STATE_OFF)
		{
			osThreadYield();
		}
		//HAL_GPIO_TogglePin(led->GPIOx, led->GPIO_Pin);
		if(ledStatus == 0 )
		{
			ledStatus = 1;
			int period = __HAL_TIM_GET_AUTORELOAD(&htim2);
			lightSensor = HAL_ADC_GetValue(&hadc2);
			int value = lightSensor/LIGHT_SENSOR_RESOLUTION * period;
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, value);

		}
		else // ledStatus == ON
		{
			ledStatus = 0;
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
		}
		osDelay(led->delay);
	}
}

void changeDelay(Led* led, int newDelay)
{
	led->delay = newDelay;
}

void startLed(Led* led)
{
	led->state = STATE_BLINKING;
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

void stopLed(Led* led)
{
	led->state = STATE_OFF;
	HAL_GPIO_WritePin(led->GPIOx, led->GPIO_Pin, 0);
	HAL_TIM_Base_Stop(&htim2);
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
}
