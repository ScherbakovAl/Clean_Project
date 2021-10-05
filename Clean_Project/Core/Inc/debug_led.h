/*
    1. Configure LED_PIN as output push-pull
    2. Init RCC: RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
*/

// user
#include "gpio.h"

#ifndef __DEBUG_LED_H__
#define __DEBUG_LED_H__

#define LED_PIN                 GPIOC, 13

// Onboard LED
void TurnLedOn(void);

void TurnLedOff(void);

void BlinkLed(void);



#endif
