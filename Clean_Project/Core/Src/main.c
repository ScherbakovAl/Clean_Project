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

uint32_t txx[88] = { };
uint32_t tyy[88] = { };
uint32_t tzz = 0;
uint16_t speed_table[720] = { };
uint8_t on[88] = { };
uint8_t key = 0;
uint8_t key_dma[88][3] = { };

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

	for (uint16_t cu = 2; cu < 720; cu++) {

		// Curve: (polynomial fitting degree 2)
		//	1		127
		//	600		4
		//	720		1

		float m = (127.35583645435952 * pow(cu, 0)) - (0.35608727805498913 * pow(cu, 1))
				+ (2.5082369544962051E-4 * pow(cu, 2));
		speed_table[cu] = m;
	}
	speed_table[1] = 127;

	int bi = 0;
	for (uint8_t ke = 0; ke < 88; ke++) {
		key_dma[ke][0] = ke / 4;
		key_dma[ke][1] = bi;
		key_dma[ke][2] = bi + 1;
		bi += 2;
		if (bi > 7) {
			bi = 0;
		}
	}

	TurnLedOff();

	while (1) {

//		if ((dma_buf[17] & (1<<0)) == 0) {
//			GPIOC->BSRR |= 0x20000000;
//		} else {
//			GPIOC->BSRR |= 0x2000;
//		}
//
//		if ((dma_buf[17] & (1<<1)) == 0) {
//			GPIOC->BSRR |= 0x40000000;
//		} else {
//			GPIOC->BSRR |= 0x4000;
//		}

//		for (KEY = 0; KEY < 48; KEY++) {
////			for (int i = 0; i < 8; i += 2) {
//				if ((dma_buf[KEY] & (1 << 0)) == 0 && (dma_buf[KEY] & (1 << 1)) != 0 && (sys_tick - Tyy[KEY]) > 1000
//						&& on[KEY] == 0) {
//					Txx[KEY] = sys_tick;
//					on[KEY] = 1;
//				}
//
//				if ((dma_buf[KEY] & (1 << 0)) == 0 && (dma_buf[KEY] & (1 << 1)) == 0 && (sys_tick - Txx[KEY]) > 1
//						&& (sys_tick - Txx[KEY]) < 720 && on[KEY] == 1) {
//					Tyy[KEY] = sys_tick;
//					on[KEY] = 0;
//					USBD_AddNoteOn(0, 1, KEY + 30, SPEEDTABLE[Tyy[KEY] - Txx[KEY]]);
//					USBD_AddNoteOff(0, 1, KEY + 30);
//					USBD_SendMidiMessages();
//				}
//
//				if ((dma_buf[KEY] & (1 << 0)) != 0 && (dma_buf[KEY] & (1 << 1)) != 0 && (sys_tick - Tyy[KEY]) > 1000) {
//					on[KEY] = 0;
//				}
////			}
//		}

//		// в key_dma	[]			[]
//		//				нота
//									номер dma_buf
//									бит1
//									бит2

		for (key = 0; key < 88; key++) {
			if ((dma_buf[key_dma[key][0]] & 1 << key_dma[key][1]) == 0) {
				if (on[key] == 0) {
					if (sys_tick - tyy[key] > 1000) {
						txx[key] = sys_tick;
						on[key] = 1;
					}
				} else if ((dma_buf[key_dma[key][0]] & 1 << key_dma[key][2]) == 0) {
					if (sys_tick - txx[key] > 1 && sys_tick - txx[key] < 720) {
						tyy[key] = sys_tick;
						USBD_AddNoteOn(0, 1, key + 21, speed_table[tyy[key] - txx[key]]);
						USBD_AddNoteOff(0, 1, key + 21);
						USBD_SendMidiMessages();
						on[key] = 0;
					}
				}
			} else if (on[key] == 1) {
				if (sys_tick - tyy[key] > 1000) {
					on[key] = 0;
				}
			}
		}
//		GPIOC->BSRR |= 0x20000000;
//		GPIOC->BSRR |= 0x2000;
	}
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

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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
