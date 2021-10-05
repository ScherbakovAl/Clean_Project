
// user
#include "debug_led.h"
#include "gpio.h"

// Onboard LED
void TurnLedOn(void)
{
    ResetPin(LED_PIN);
}
void TurnLedOff(void)
{
    SetPin(LED_PIN);
}
void BlinkLed(void)
{
    TogglePin(LED_PIN);
}
