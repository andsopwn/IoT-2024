/*
 * leddriver.h
 *
 *  Created on: Jul 6, 2021
 *      Author: cndi
 */

#ifndef SRC_LEDDRIVE_H_
#define SRC_LEDDRIVE_H_

#define LEDn		8       // 포트별 LED 개수

void LEDInitRedLED();
void LEDInitGreenLED();
void LEDInit();
void GreenLEDOn(int nIndex);
void RedLEDOn(int nIndex);
void RedLEDAllOff();
void GreenLEDAllOff();
void GreenLEDOff(int nIndex);
void RedLEDOff(int nLed);
void LEDALLOff();

#endif /* SRC_LEDDRIVE_H_ */
