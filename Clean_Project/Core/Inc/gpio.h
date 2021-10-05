// STM32F103
#include "stm32f1xx.h"

// cstd
#include <stdbool.h>

#ifndef __GPIO_H__
#define __GPIO_H__

// Pin modes
#define PIN_MODE_IANALOG        0x00    // in analog
#define PIN_MODE_IFLOAT         0x04    // in floating

#define PIN_MODE_2MHZ_OPP       0x02    // out push-pull; speed 2 MHz
#define PIN_MODE_10MHZ_OPP      0x01    // out push-pull; speed 10 MHz
#define PIN_MODE_50MHZ_OPP      0x03    // out push-pull; speed 50 MHz

#define PIN_MODE_2MHZ_OOD       0x06    // out open-drain; speed 2 MHz
#define PIN_MODE_10MHZ_OOD      0x05    // out open-drain; speed 10 MHz
#define PIN_MODE_50MHZ_OOD      0x07    // out open-drain; speed 50 MHz

#define PIN_MODE_2MHZ_AFPP      0x0A    // alternate function push-pull; speed 2 MHz
#define PIN_MODE_10MHZ_AFPP     0x09    // alternate function push-pull; speed 10 MHz
#define PIN_MODE_50MHZ_AFPP     0x0B    // alternate function push-pull; speed 50 MHz

#define PIN_MODE_2MHZ_AFOD      0x0E    // alternate function open-drain; speed 2 MHz
#define PIN_MODE_10MHZ_AFOD     0x0D    // alternate function open-drain; speed 10 MHz
#define PIN_MODE_50MHZ_AFOD     0x0F    // alternate function open-drain; speed 50 MHz

#define PIN_MODE_IPULL          0x08    // in pull-up/pull-down (depending on ODR value)

// GPIO
void SetGpioMode(GPIO_TypeDef* port, int pin, int mode);

bool GetPinState(GPIO_TypeDef* port, int pin);

void SetPin(GPIO_TypeDef* port, int pin);

void ResetPin(GPIO_TypeDef* port, int pin);

void TogglePin(GPIO_TypeDef* port, int pin);

void SetMulPin(GPIO_TypeDef* port, int mask);

void ResetMulPin(GPIO_TypeDef* port, int mask);



#endif
