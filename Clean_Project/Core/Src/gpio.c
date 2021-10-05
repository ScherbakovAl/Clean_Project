// cstd
#include <stdbool.h>

//user
#include "gpio.h"

void SetGpioMode(GPIO_TypeDef* port, int pin, int mode)
{
    if (pin > 7)
        port->CRH = (port->CRH & ~(0x0f << (pin-8)*4)) | (mode << (pin-8)*4);
    else
        port->CRL = (port->CRL & ~(0x0f << pin*4)) | (mode << pin*4);
}
bool GetPinState(GPIO_TypeDef* port, int pin)
{
    return ((port->IDR & (1 << pin)) != 0);
}
void SetPin(GPIO_TypeDef* port, int pin)
{
    port->BSRR = (1 << pin);
}
void ResetPin(GPIO_TypeDef* port, int pin)
{
    port->BSRR = (1 << (pin+16));
}
void TogglePin(GPIO_TypeDef* port, int pin)
{
    if (GetPinState(port, pin))
        ResetPin(port, pin);
    else
        SetPin(port, pin);
}
void SetMulPin(GPIO_TypeDef* port, int mask)
{
    port->BSRR = (mask & 0x0ffff);
}
void ResetMulPin(GPIO_TypeDef* port, int mask)
{
    port->BSRR = (mask & 0x0ffff) << 16;
}
