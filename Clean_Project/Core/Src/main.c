//hello!
#include "main.h"
#include "usb_device.h"
#include "usbd_midi.h"
#include "usbd_midi_if.h"
#include "stm32f1xx.h"
#include "init.h"
#include "gpio.h"
#include "debug_led.h"
#include "timer.h"
#include "dma.h"
#include <stdbool.h>
#include <math.h>

void
SystemClock_Config(void);

volatile uint8_t dma_buf[DMA_NUM_OF_TRANSACTIONS];
bool data_updated;

uint8_t buu = 0;
uint32_t tim = 0;

uint32_t Txx = 0;
uint32_t Tyy = 0;
uint32_t Tzz = 0;
uint8_t SPEEDTABLE[12000] = { };
uint8_t onx = 0;
uint8_t ony = 0;
uint8_t Velocity = 0;
uint8_t KEY = 0;

int main(void) {
	HAL_Init();
	SystemClock_Config();
	MX_USB_DEVICE_Init();

	InitRcc();
	InitGpio();
	InitSysTick();

	TurnLedOff();
	SetPin(RESET_PIN);
	delay_ms(1);
	ResetPin(RESET_PIN);
	delay_ms(1);

	InitInterrupt();
	InitDma((uint8_t*) dma_buf, &data_updated);
	InitTim3();
	InitTim2();


	TurnLedOn();
	for (uint32_t ti = 0; ti < 12000; ti++) {
		//float m = -0.01260126 * ti + 127.0126;
		//float m = 0.8379653 + 126.2461 / pow (2,(ti / 1041.046));
		float m = -14717.48 + ((12546.6 - (-14717.48)) / (1 + pow((ti / 156.281),0.7699161)));
		SPEEDTABLE[ti] = m;
	}
	TurnLedOff();
	tim = sys_tick;

	while (1) {

		if (sys_tick - tim > 0) {

			buu = dma_buf[16];

			if (buu == 254) {
				TurnLedOn();
			} else {
				TurnLedOff();
			}



			if (buu == 254 && onx == 0 && ony == 0) {
				onx = 1;
				Txx = sys_tick;
			}

			if (buu == 252 && onx == 1 && ony == 0) {
				ony = 1;
				Tyy = sys_tick;
				Tzz = Tyy - Txx;

				if (Tzz < 12000) {
				Velocity = SPEEDTABLE[Tzz];
				USBD_AddNoteOn(0, 1, 60, Velocity);
				USBD_SendMidiMessages();
				}
			}

			if (buu == 255 && onx == 1 && ony == 1) {
				onx = 0;
				ony = 0;
				USBD_AddNoteOff(0, 1, 60);
				USBD_SendMidiMessages();
			}
			tim = sys_tick;
		}
	}

























//		USBD_AddNoteOn(0, 1, 60, 60);
//		USBD_AddNoteOff(0, 1, 55);
//		USBD_SendMidiMessages();
//		HAL_Delay(200);

}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

//static void MX_TIM2_Init(void) {
//
//	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
//	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
//
//	htim2.Instance = TIM2;
//	htim2.Init.Prescaler = 7199;
//	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
//	htim2.Init.Period = 9999;
//	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
//	if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
//		Error_Handler();
//	}
//	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
//		Error_Handler();
//	}
//	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
//			!= HAL_OK) {
//		Error_Handler();
//	}
//}

//static void MX_GPIO_Init(void) {
//	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
//
//	__HAL_RCC_GPIOD_CLK_ENABLE();
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	__HAL_RCC_GPIOB_CLK_ENABLE();
//
//	HAL_GPIO_WritePin(GPIOA,
//			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
//					| GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_SET);
//
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//
//	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
//			| GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//	GPIO_InitStruct.Pin = GPIO_PIN_0;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//	GPIO_InitStruct.Pin = GPIO_PIN_9;
//	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//}

void Error_Handler(void) {
	__disable_irq();
	while (1) {
	}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */