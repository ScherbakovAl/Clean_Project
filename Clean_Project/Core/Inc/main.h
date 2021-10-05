#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include <stdbool.h>


void Error_Handler(void);


// Config

// Pulses for optical sensors (settings for TIM2) =
// Switch to the next group of optical sensors
// X * 0.5 us
#define PHASE_PULSE                     32
// Pulse to lock/unlock register with optical sensors output signals (TIM2)
// X * 0.5 us before end of current phase (16 us)
#define UNLOCK_PULSE                    2
#define LOCK_PULSE                      1

// Pulse for multiplexer (settings for TIM3)
// X * 0.5 us
#define MUX_PULSE                       4
// 8 multiplexed outputs should be read
#define DMA_NUM_OF_TRANSACTIONS         48


// Pins
// Pulses power for optical sensors and registers control
#define UNLOCK_PIN          GPIOB, 10   // TIM2, C3
#define LOCK_PIN            GPIOB, 11   // TIM2, C4     switch next phase
// Pulse for multiplexer
#define MUX_PIN             GPIOB, 1    // TIM3, C4
// Reset input and output counters
#define RESET_PIN           GPIOB, 0

// DMA read data port
#define PARALLEL_PORT       GPIOA






#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
