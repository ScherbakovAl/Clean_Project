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

void DMA1_Channel3_IRQHandler();

void InitDma(uint8_t * buf, bool* upd_flag);







#endif
