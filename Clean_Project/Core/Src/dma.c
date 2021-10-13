// STM32F103
#include "stm32f1xx.h"

// cstd
#include <stdbool.h>

//user
#include "DMA.h"
#include "main.h"
#include "timer.h"
//#include "debug_led.h"

// Using channel 2 of DMA1 with TIM3 DMA request

// Buffer to return value and flag
uint8_t* buf_ptr;

// Data ready flag
static bool data_ready_flag = false;

// Local buffer
uint8_t tmp_buf[DMA_NUM_OF_TRANSACTIONS];

// Interrupt handler
void DMA1_Channel3_IRQHandler()
{
    // Clear interrupt flag
    DMA1->IFCR |= DMA_IFCR_CTCIF3;

    // Return data
    for (int i = 0; i < DMA_NUM_OF_TRANSACTIONS; i++) {
        buf_ptr[i] = tmp_buf[i];
    }
    data_ready_flag = true;

}
void InitDma(uint8_t * buf)
{
    // Return data configuration
    buf_ptr = buf;

    // Channel configuration
    // Peripheral address
    DMA1_Channel3->CPAR = (uint32_t)&(PARALLEL_PORT->IDR);

    DMA1_Channel3->CCR  =   DMA_CCR_MEM2MEM    * 0 |   // memory to memory mode
                            DMA_CCR_MSIZE_1    * 0 |   // memory size 00: 8 bit; 01:16; 11:32
                            DMA_CCR_MSIZE_0    * 0 |   //
                            DMA_CCR_PSIZE_1    * 0 |   // peripheral size 00: 8 bit; 01:16; 10:32
                            DMA_CCR_PSIZE_0    * 0 |   //
                            DMA_CCR_MINC       * 1 |   // memory increment mode
                            DMA_CCR_PINC       * 0 |   // peripheral increment mode
                            DMA_CCR_CIRC       * 1 |   // circular mode
                            DMA_CCR_DIR        * 0 |   // data transfer direction 0: read from peripheral; 1: from memory
                            DMA_CCR_TCIE       * 1;    // transfer complete interrupt enable

    // Memory address
    DMA1_Channel3->CMAR = (uint32_t)tmp_buf;
    // Number of transactions
    DMA1_Channel3->CNDTR = DMA_NUM_OF_TRANSACTIONS;

    // Channel enable
    DMA1_Channel3->CCR |= DMA_CCR_EN;
}

// function to check if data were copied to the buffer and reset flag
bool IsDataReady(void)
{
    if (data_ready_flag == true) {

        data_ready_flag = false;
        return true;
    }
    else
        return false;
}
