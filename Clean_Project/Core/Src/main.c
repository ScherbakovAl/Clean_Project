//hello2!
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
uint8_t speed_table[180] = { };
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
	InitDma((uint8_t*) dma_buf);
	InitTim3();
	InitTim2();

	TurnLedOn();

	for (uint16_t cu = 1; cu < 180; cu++) {

		// Curve: (polynomial fitting degree 2)
		//	1		127
		//	150		4
		//	180		1

		float m = (128.43 * pow(cu, 0)) - (1.43 * pow(cu, 1)) + (0.00405 * pow(cu, 2));
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

//		// в key_dma	[]			[]
//						нота
//									номер dma_buf
//									бит1
//									бит2

//		GPIOC->BSRR |= 0x20000000;
//		GPIOC->BSRR |= 0x2000;

		if (IsDataReady()) {
			for (key = 0; key < 88; key++) {

//				if ((dma_buf[17] & 1 << 0) == 0) {
//					GPIOC->BSRR |= 0x20000000;
//				} else {
//					GPIOC->BSRR |= 0x2000;
//				}
//				if ((dma_buf[17] & 1 << 1) == 0) {
//					GPIOC->BSRR |= 0x40000000;
//				} else {
//					GPIOC->BSRR |= 0x4000;
//				}

				if ((dma_buf[key_dma[key][0]] & 1 << key_dma[key][1]) == 0) {
					if (on[key] == 0) {
						if ((sys_tick - tyy[key]) > 200) {
							txx[key] = sys_tick;
							on[key] = 1;
						}
					} else if ((dma_buf[key_dma[key][0]] & 1 << key_dma[key][2]) == 0) {
						if (on[key] == 1) {
							if ((sys_tick - txx[key]) > 1 && (sys_tick - txx[key]) < 180) {
								tyy[key] = sys_tick;
								USBD_AddNoteOn(0, 1, key + 21, speed_table[tyy[key] - txx[key]]);
								USBD_AddNoteOff(0, 1, key + 21);
								USBD_SendMidiMessages();
								on[key] = 2;
							}

						}
					}
				} else if (on[key] != 0) {
					if (sys_tick - tyy[key] > 200) {
						on[key] = 0;
					}
				}
			}
		}			//if ready
	}				//while
}					//main

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
