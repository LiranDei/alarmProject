/*
 * button.c
 *
 *  Created on: Oct 20, 2022
 *      Author: student
 */

#include "Button.h"
#include "main.h"


void buttonInit(Button* button, ButtonName name, GPIO_TypeDef* gpioPort, uint16_t gpioPin)
{
	button->name = name;
	button->btnState = UNPRESSED;
	button->gpioPort = gpioPort;
	button->gpioPin = gpioPin;

}

