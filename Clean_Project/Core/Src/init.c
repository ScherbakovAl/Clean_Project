// STM32F103
#include "stm32f1xx.h"

// user
#include "init.h"
#include "main.h"
#include "gpio.h"
#include "debug_led.h"


void InitRcc(void)
{
    // port A
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    // port B
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    // port C
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    // TIM2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    // TIM3
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    // TIM4
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    // AFIO
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    // DMA1
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
}
void InitInterrupt(void)
{
    // DMA1 channel 3
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
}
void InitGpio(void)
{
    // Remap A15, B3, B4
    /*AFIO->MAPR |=   AFIO_MAPR_SWJ_CFG_2 * 0 |
                    AFIO_MAPR_SWJ_CFG_1 * 1 |
                    AFIO_MAPR_SWJ_CFG_0 * 0;*/
    // Remap TIM2
    AFIO->MAPR |= AFIO_MAPR_TIM2_REMAP_1;   // remap C3 to PB10; C4 to PB11
    
    // Pulses power for optical sensors and registers control
    SetGpioMode(UNLOCK_PIN, PIN_MODE_10MHZ_AFPP);
    SetGpioMode(LOCK_PIN, PIN_MODE_10MHZ_AFPP);
    // Pulse for multiplexer
    SetGpioMode(MUX_PIN, PIN_MODE_10MHZ_AFPP);
    // Reset input and output counters
    SetGpioMode(RESET_PIN, PIN_MODE_2MHZ_OPP);

    SetGpioMode(GPIOA, 0, PIN_MODE_IPULL);
    SetGpioMode(GPIOA, 1, PIN_MODE_IPULL);
    SetGpioMode(GPIOA, 2, PIN_MODE_IPULL);
    SetGpioMode(GPIOA, 3, PIN_MODE_IPULL);
    SetGpioMode(GPIOA, 4, PIN_MODE_IPULL);
    SetGpioMode(GPIOA, 5, PIN_MODE_IPULL);
    SetGpioMode(GPIOA, 6, PIN_MODE_IPULL);
    SetGpioMode(GPIOA, 7, PIN_MODE_IPULL);
    SetPin(GPIOA, 0);
    SetPin(GPIOA, 1);
    SetPin(GPIOA, 2);
    SetPin(GPIOA, 3);
    SetPin(GPIOA, 4);
    SetPin(GPIOA, 5);
    SetPin(GPIOA, 6);
    SetPin(GPIOA, 7);

    // Debug LED
    SetGpioMode(LED_PIN, PIN_MODE_10MHZ_OPP);
    SetGpioMode(GPIOC, 14, PIN_MODE_10MHZ_OPP);		// for test
}
