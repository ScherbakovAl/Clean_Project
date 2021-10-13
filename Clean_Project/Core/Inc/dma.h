/*
    1. Define and configure PARALLEL_PORT
    2. Init RCC: RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    3. Enable DMAx_Channely interrupt
    4. Call function InitDMA();
*/

// STM32F103
#include "stm32f1xx.h"

// cstd
#include <stdbool.h>

#ifndef __DMA_H__
#define __DMA_H__


// buf is a pointer to the buffer to copy received data
void InitDma(uint8_t * buf);

// function to check if data were copyed to the buffer and reset flag
bool IsDataReady(void);


#endif
