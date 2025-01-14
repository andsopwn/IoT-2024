/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "textlcd.h"
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef unsigned char u8;
typedef unsigned int u32;
typedef char bool;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define False 0
#define True 1
/* USER CODE END PD */
bool INPUT_FLAG = False;
u8 input;
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#define xtimes(a) (a<<1)^(a&0x80?0x1b:0)
#define xtimes_2(a) xtimes((xtimes(a)))
#define xtimes_3(a) xtimes((xtimes((xtimes(a)))))
#define xtimes_4(a) xtimes_2((xtimes_2(a)))
/*
#define poly_degree(poly) ( \
    ((poly) & 0x80) ? 7 : \
    ((poly) & 0x40) ? 6 : \
    ((poly) & 0x20) ? 5 : \
    ((poly) & 0x10) ? 4 : \
    ((poly) & 0x08) ? 3 : \
    ((poly) & 0x04) ? 2 : \
    ((poly) & 0x02) ? 1 : \
    ((poly) & 0x01) ? 0 : -1 )
*/
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
u8 mul(u8 a, u8 b) {
	u8 c = 0;
	c ^= (a & 0x01) ? b : 0;
	b = xtimes(b);
	c ^= (a & 0x02) ? b : 0;
	b = xtimes(b);
	c ^= (a & 0x04) ? b : 0;
	b = xtimes(b);
	c ^= (a & 0x08) ? b : 0;
	b = xtimes(b);
	c ^= (a & 0x10) ? b : 0;
	b = xtimes(b);
	c ^= (a & 0x20) ? b : 0;
	b = xtimes(b);
	c ^= (a & 0x40) ? b : 0;
	b = xtimes(b);
	c ^= (a & 0x80) ? b : 0;
	return c;
}

u8 squa(u8 a) {
    u8 ah = 0, al = 0;
    ah = ((a & 0x80) >> 1) | ((a & 0x40) >> 2) | ((a & 0x20) >> 3) | ((a & 0x10) >> 4);
    al = ((a & 0x8) << 3) | ((a & 0x4) << 2) | ((a & 0x2) << 1) | (a & 0b1);

    return xtimes_4(ah) ^ xtimes_3(ah) ^ xtimes(ah) ^ ah ^ al;
}

u8 itoh(u8 a) {
	u8 a0, a1;

	a0 = mul(a, squa(a));
	a1 = mul(a0, squa(squa(a0)));
	a1 = mul(a0, squa(squa(a1)));
	a1 = squa(mul(a, squa(a1)));

	return a1;
}

u32 poly_degree(u32 poly) {
    int deg = -1;
    while(poly) {
        poly >>= 1;
        deg++;
    }
    return deg;
}

u8 gf_mod(u8 a) {
    while(poly_degree(a) >= 8) {
        int shift = poly_degree(a) - 8;
        a ^= 0x11b << shift;
    }
    return a;
}


u8 eea(u8 a) {
    if (a == 0)
        return 0;

    u32 u = a;
    u32 v = 0x11b;
    u32 g1 = 1;
    u32 g2 = 0;

    for (int i = 0; i < 8; i++) {
        int deg_u = poly_degree(u);
        int deg_v = poly_degree(v);

        if (u == 1)
            break;

        if (deg_u < deg_v) {
            // Swap u and v
            u32 temp_u = u; u = v; v = temp_u;
            // Swap g1 and g2
            u32 temp_g = g1; g1 = g2; g2 = temp_g;
            deg_u = poly_degree(u);
            deg_v = poly_degree(v);
        }

        u8 shift = deg_u - deg_v;

        // u = u + v * x^shift
        u ^= v << shift;
        gf_mod(u);
        // g1 = g1 + g2 * x^shift
        g1 ^= g2 << shift;
        gf_mod(g1);
    }

    return (u8)g1;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	bool START_FLAG = True;
	int num_input;
	u8 tmp[8] = { 0x00, };
	u8 num = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
	SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
	MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
	TextlcdInit();
	clearScreen(ALL_LINE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while(1) {
		num = 0;
		num_input = 0;

		if(START_FLAG == True) {
			START_FLAG == False;
			clearScreen(0);
			writeTextLine(LINE_1, "EEA  >> ");
		}


		while (num_input < 2) {
			if(INPUT_FLAG == True) {
				INPUT_FLAG = False;
				if(num_input == 0) num = input & 0x0f;
				else num = (num << 4) | (input & 0x0F);

				//num ^= (num_input % 2 == 1) ? (input << 4) : input;

			    sprintf(tmp, "1.0x%02x", num);
				writeTextLine(LINE_2, tmp);

				if (num_input % 2 == 0 && num_input != 0) {
					sprintf(tmp, "2.0x%02x", num);
					writeTextLine(LINE_2, tmp);

					HAL_Delay(1000);
					clearScreen(LINE_2);

				}
				num_input++;
				HAL_Delay(300);
			}
			HAL_Delay(100);
		}
		clearScreen(ALL_LINE);
		sprintf(tmp, "inv(%02x)", num);
		writeTextLine(LINE_1, tmp);
		sprintf(tmp, "-> 0x%02x", eea(num));
		writeTextLine(LINE_2, tmp);

		num = 0;
		num_input = 0;

		HAL_Delay(2500);
	}




    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
}
  /* USER CODE END 3 */

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE2 PE3 PE4 PE5
                           PE6 PE7 PE8 PE9
                           PE10 PE11 PE12 PE13
                           PE14 PE15 PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 PC2 PC3
                           PC4 PC5 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 PD10 PD11
                           PD12 PD13 PD14 PD15 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	for (int i = 0; i < 16; i++)
	{
		if ((GPIO_Pin >> i) == 0x0001)
		{
			input = i;
			INPUT_FLAG = True;
		}
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
