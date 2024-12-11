/*
 * leddrive.c
 *
 *  Created on: Jul 6, 2021
 *      Author: cndi
 */

#include "main.h"
#include "leddrive.h"

// 5)
void LEDInitRedLED()
{
  RedLEDAllOff();
}

// 6)
void LEDInitGreenLED()
{
  GreenLEDAllOff();
}

// 7)
void LEDInit()
{
  LEDInitRedLED();
  LEDInitGreenLED();
}

// 8)
void GreenLEDOn(int nIndex)
{
  HAL_GPIO_WritePin(GPIOC, (0x0001 << nIndex), GPIO_PIN_SET);
}
void RedLEDOn(int nIndex)
{
  HAL_GPIO_WritePin(GPIOD, (0x0100 << nIndex), GPIO_PIN_SET);
}

// 9)
void GreenLEDOff(int nIndex)
{
  HAL_GPIO_WritePin(GPIOC, (0x0001 << nIndex), GPIO_PIN_RESET);
}
void RedLEDOff(int nIndex)
{
  HAL_GPIO_WritePin(GPIOD, (0x0100 << nIndex), GPIO_PIN_RESET);
}

// 10)
void RedLEDAllOff()
{
  int i;
  for (i = 0; i < LEDn ;i++ )
    RedLEDOff(i);
}
void GreenLEDAllOff()
{
  int i;
  for (i = 0; i < LEDn ;i++ )
    GreenLEDOff(i);
}

// 11)
void LEDALLOff()
{
  RedLEDAllOff();
  GreenLEDAllOff();
}


