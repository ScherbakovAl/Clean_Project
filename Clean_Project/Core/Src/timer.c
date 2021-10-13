// STM32F103
#include "stm32f1xx.h"

// user
#include "main.h"
#include "timer.h"

volatile uint32_t sys_tick = 0;

// SysTick timer interrupt
void SysTick_Handler()
{
	sys_tick++;										// every 1ms
}
void InitSysTick(void)
{
	SysTick->LOAD = SYS_TIMER_TICK;
	SysTick->VAL = 0;
	SysTick->CTRL =	SysTick_CTRL_CLKSOURCE_Msk |
	SysTick_CTRL_TICKINT_Msk	|					// interrupt enable
	SysTick_CTRL_ENABLE_Msk;
}
// Proceed SysTick value and reset counter
bool IfDelayPassed(uint32_t* cnt, uint32_t delay_ms)
{
	if ((sys_tick - *cnt) > delay_ms) {
		*cnt = sys_tick;
		return true;
	}
	else
		return false;
}
// Reset counter
void ResetCounter(uint32_t* cnt)
{
    *cnt = sys_tick;
}
void InitTim2(void)
{
	// Timer
	TIM2->CR1 = TIM_CR1_ARPE	* 0 |	// Auto-reload preload enable
				TIM_CR1_CMS_1	* 0 |	// Center-aligned mode selection : edge-aligned mode
				TIM_CR1_CMS_0	* 0 |
				TIM_CR1_DIR		* 0;	// Direction : upcounter
	TIM2->PSC = 35;                     // CK_CNT = 2MHz
	TIM2->ARR = PHASE_PULSE - 1;        // 16us period
	TIM2->EGR = TIM_EGR_UG;             // update event enable
	// DMA
	//TIM2->DIER  |= TIM_DIER_UDE;        // update DMA request enable
	// PWM
	TIM2->CCMR2 =	TIM_CCMR2_OC3M_2	* 1 |   // C3 PWM mode 2 (inactive -> active)
					TIM_CCMR2_OC3M_1	* 1 |
                    TIM_CCMR2_OC3M_0    * 1 |
                    TIM_CCMR2_OC4M_2    * 1 |   // C4 PWM mode 2 (inactive -> active)
                    TIM_CCMR2_OC4M_1    * 1 |
                    TIM_CCMR2_OC4M_0    * 1;
    TIM2->CCER  =   TIM_CCER_CC3E       * 1 |   // C3 out enable
                    TIM_CCER_CC3P       * 0 |   // C3 active high
                    TIM_CCER_CC4E       * 1 |   // C4 out enable
                    TIM_CCER_CC4P       * 1;    // C4 active low
    
    TIM2->CCR3  = PHASE_PULSE - UNLOCK_PULSE;   // start register unlock pulse at 15 us
    TIM2->CCR4  = PHASE_PULSE - LOCK_PULSE;     // end register unlock pulse at 15.5 us
    
    // Trigger output TRGO1 (active signal while TIM2 enabled)
    TIM2->CR2   =   TIM_CR2_MMS_2       * 0 |   // CNT_EN signal as trigger
                    TIM_CR2_MMS_1       * 0 |
                    TIM_CR2_MMS_0       * 1;
    
    // Start
    TIM2->EGR = TIM_EGR_UG;         // update
    TIM2->CR1 |= TIM_CR1_CEN;       // start
    
}
void InitTim3(void)
{
    // Timer
    TIM3->CR1 = TIM_CR1_ARPE    * 0 |   // Auto-reload preload enable
                TIM_CR1_CMS_1   * 0 |   // Center-aligned mode selection : edge-aligned mode
                TIM_CR1_CMS_0   * 0 |
                TIM_CR1_DIR     * 0;    // Direction : upcounter
    TIM3->PSC = 35;                     // CK_CNT = 2MHz
    TIM3->ARR = MUX_PULSE - 1;          // 1MHz
    TIM3->EGR = TIM_EGR_UG;             // update event enable
    // DMA
    TIM3->DIER  |= TIM_DIER_CC4DE;        // C4 DMA request enable
    // PWM
    TIM3->CCMR2 =   TIM_CCMR2_OC4M_2    * 1 |   // C4 PWM mode 1 (active -> inactive)
                    TIM_CCMR2_OC4M_1    * 1 |
                    TIM_CCMR2_OC4M_0    * 0;
    TIM3->CCER  =   TIM_CCER_CC4E       * 1 |   // C4 out enable
                    TIM_CCER_CC4P       * 0;    // C4 active high
    
    TIM3->CCR4  = MUX_PULSE / 2;        // 50%

    // Slave mode
    TIM3->SMCR  =   TIM_SMCR_TS_2       * 0 |   // internal trigger 1 (TIM2)
                    TIM_SMCR_TS_1       * 0 |
                    TIM_SMCR_TS_0       * 1 |
                    TIM_SMCR_SMS_2      * 1 |   // gated mode
                    TIM_SMCR_SMS_1      * 0 |
                    TIM_SMCR_SMS_0      * 1;
    
    // Start
    TIM3->EGR = TIM_EGR_UG;         // update
    TIM3->CR1 |= TIM_CR1_CEN;       // start
    
}
/*void DisableTim3(void)
{
    TIM3->CR1 &= ~TIM_CR1_CEN;      // stop
    TIM3->EGR = TIM_EGR_UG;         // update
    //TIM2->CCER &= ~TIM_CCER_CC1E;   // out disable
}*/

// Delay TIM4
void delay_ms(int time)
{
    TIM4->CR1 = 0;
    TIM4->SR = 0;
    TIM4->PSC = 35999;                  // 2kHz
    TIM4->ARR = time * 2 - 1;
    TIM4->EGR = TIM_EGR_UG;
    TIM4->SR &= ~TIM_SR_UIF;
    TIM4->CR1 |= TIM_CR1_CEN;
    while (!(TIM4->SR & TIM_SR_UIF))
        ;
    TIM4->CR1 &= ~TIM_CR1_CEN;
}
void delay_us(int time)
{
    TIM4->CR1 = 0;
    TIM4->SR = 0;
    TIM4->PSC = 35;                     // 2MHz
    TIM4->ARR = time * 2 - 1;
    TIM4->EGR = TIM_EGR_UG;
    TIM4->SR &= ~TIM_SR_UIF;
    TIM4->CR1 |= TIM_CR1_CEN;
    while (!(TIM4->SR & TIM_SR_UIF))
        ;
    TIM4->CR1 &= ~TIM_CR1_CEN;
}
