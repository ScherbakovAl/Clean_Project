// STM32F103
#include "stm32f1xx.h"

#ifndef __INIT_H__
#define __INIT_H__

// Clock source select;
// Peripheral clock enable
void InitRcc(void);

// Init interrupts
void InitInterrupt(void);

// Init GPIO
void InitGpio(void);


#endif
