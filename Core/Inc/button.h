/*
 * button.h
 *
 *  Created on: Oct 20, 2022
 *      Author: student
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"

typedef enum ButtonName_
{
	RIGHT_BTN,
	LEFT_BTN
} ButtonName;

typedef enum ButtonState_
{
	UNPRESSED,
	PRESSED,
}ButtonState;

typedef struct  Button_
{
	ButtonName name;
	ButtonState btnState;
	GPIO_TypeDef* gpioPort;
	uint16_t gpioPin;
} Button;

void buttonInit(Button* button, ButtonName name, GPIO_TypeDef* gpioPort, uint16_t gpioPin);
void buttonOnInterrupt(Button* button,uint16_t pin);

#endif /* INC_BUTTON_H_ */
