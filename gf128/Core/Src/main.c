/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body with GF(2^128) exponentiation timing
  ******************************************************************************
  * @attention
  * 기존 LED 이동 로직 제거. GF(2^128) 지수승 연산 시 PD8 LED를 이용한 Timing 시그널만 남김.
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
typedef struct {
    uint32_t data[4];  // data[0]: lower 32-bit, data[3]: higher 32-bit
} uint128_t;

/* Private define ------------------------------------------------------------*/
#define SQUARE_LED_PORT GPIOD
#define SQUARE_LED_PIN  GPIO_PIN_9  // 제곱 연산 시 LED
#define MUL_LED_PORT    GPIOD
#define MUL_LED_PIN     GPIO_PIN_8  // 일반 곱셈 연산 시 LED

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

void uint128_init(uint128_t *num, uint32_t value);
void uint128_copy(uint128_t *dest, const uint128_t *src);
void uint128_print(const uint128_t *num);
void gf2_128_add(uint128_t *result, const uint128_t *a, const uint128_t *b);
void gf2_128_mul(uint128_t *result, const uint128_t *a, const uint128_t *b);
void gf2_128_exp(uint128_t *result, const uint128_t *g, const uint128_t *e);

/* USER CODE BEGIN 0 */
void uint128_init(uint128_t *num, uint32_t value) {
    num->data[0] = value;
    num->data[1] = 0;
    num->data[2] = 0;
    num->data[3] = 0;
}

void uint128_copy(uint128_t *dest, const uint128_t *src) {
    memcpy(dest->data, src->data, sizeof(uint32_t) * 4);
}

void uint128_print(const uint128_t *num) {
    //printf("0x%08X%08X%08X%08X",num->data[3], num->data[2], num->data[1], num->data[0]);
}

void gf2_128_add(uint128_t *result, const uint128_t *a, const uint128_t *b) {
    for (int i = 0; i < 4; i++) {
        result->data[i] = a->data[i] ^ b->data[i];
    }
}

// gf2_128_mul에서는 LED 제어 제거. 단순 연산만 수행.
void gf2_128_mul(uint128_t *result, const uint128_t *a, const uint128_t *b) {
    uint128_t temp = {{0, 0, 0, 0}};
    uint128_t a_copy;
    uint128_copy(&a_copy, a);

    for (int i = 127; i >= 0; i--) {
        if ((b->data[i / 32] >> (i % 32)) & 1) {
            gf2_128_add(&temp, &temp, &a_copy);
        }

        uint32_t carry = a_copy.data[3] >> 31;
        for (int j = 3; j > 0; j--) {
            a_copy.data[j] = (a_copy.data[j] << 1) | (a_copy.data[j - 1] >> 31);
        }
        a_copy.data[0] <<= 1;

        if (carry) {
            a_copy.data[0] ^= 0x87;
        }
    }

    uint128_copy(result, &temp);
}
void gf2_128_exp(uint128_t *result, const uint128_t *g, const uint128_t *e) {
    uint128_t A;
    uint128_init(&A, 1);  // A ← 1

    int t = 127;
    while (t >= 0) {
        if ((e->data[t / 32] >> (t % 32)) & 1) {
            break;
        }
        t--;
    }

    for (int i = t; i >= 0; i--) {
        // 제곱 연산: A ← A · A
        // LED9 On/Off
        HAL_GPIO_WritePin(SQUARE_LED_PORT, SQUARE_LED_PIN, GPIO_PIN_SET);
        HAL_Delay(300);
        gf2_128_mul(&A, &A, &A);
        HAL_GPIO_WritePin(SQUARE_LED_PORT, SQUARE_LED_PIN, GPIO_PIN_RESET);
        HAL_Delay(300);

        // 만약 e_i == 1이면 A ← A · g (일반 곱셈)
        // LED8 On/Off
        if ((e->data[i / 32] >> (i % 32)) & 1) {
            HAL_GPIO_WritePin(MUL_LED_PORT, MUL_LED_PIN, GPIO_PIN_SET);
            HAL_Delay(300);
            gf2_128_mul(&A, &A, g);
            HAL_GPIO_WritePin(MUL_LED_PORT, MUL_LED_PIN, GPIO_PIN_RESET);
            HAL_Delay(300);
        }
    }

    uint128_copy(result, &A);
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    uint128_t g = {{0x89ABCDEF, 0x01234567, 0x89ABCDEF, 0x01234567}};
    // 0xAED = 1010 1110 1101 (12비트)
    uint128_t e;
    uint128_init(&e, 0xAED); // 하위 32비트에 0xAED를 넣고 나머지는 0으로 초기화
    uint128_t result;

    printf("GF(2^128) exponentiation start...\r\n");

    gf2_128_exp(&result, &g, &e);

    printf("Result: ");
    uint128_print(&result);

    while (1)
    {
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  // 기본 SystemClock 설정 (CubeMX에 의해 생성된 코드)
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage*/
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators*/
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks*/
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

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    // PD8, PD9 를 출력으로 설정
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
