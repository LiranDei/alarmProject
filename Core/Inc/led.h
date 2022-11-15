/*
 * led.h
 *
 *  Created on: Oct 22, 2022
 *      Author: USER
 */

#ifndef INC_LED_H_
#define INC_LED_H_

typedef enum _led_STATE
{
	STATE_OFF,
	STATE_ON,
	STATE_BLINKING
}LedState;

typedef struct _led
{
	LedState state;
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
	int delay;
}Led;

void initBlinkLed();
void blinkLed(void* argo);
void changeDelay(Led* led, int newDelay);
void ledInit(Led* led, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void startLed(Led* led);
void stopLed(Led* led);
void ledInitPwm(Led*led);

#endif /* INC_LED_H_ */
