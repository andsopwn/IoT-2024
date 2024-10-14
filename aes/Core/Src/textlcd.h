/*
 * textlcd.h
 *
 *  Created on: Jul 9, 2021
 *      Author: cndi
 */

#ifndef SRC_TEXTLCD_H_
#define SRC_TEXTLCD_H_

#include "main.h"

/*  H/W 
connector 0 
  1 pin => LCD_RS
  2 pin => LCD_RW
  3 pin => LCD_E

connector 1
  1 pin => LCD_D0
  2 pin => LCD_D1
  3 pin => LCD_D2
  4 pin => LCD_D3
  5 pin => LCD_D4
  6 pin => LCD_D5
  7 pin => LCD_D6
  8 pin => LCD_D7   
*/
#define LCD_SIGNAL_CON          GPIOD

#define LCD_RS                  0
#define LCD_RW                  1
#define LCD_E                   2

#define LCD_DATA_CON            GPIOC
/******************************************************************************
*
*      TEXT LCD FUNCTION
*
******************************************************************************/
#define CLEAR_DISPLAY   0x0001
#define CURSOR_AT_HOME  0x0002

// Entry Mode set
#define MODE_SET_DEF    0x0004
#define MODE_SET_DIR_RIGHT  0x0002
#define MODE_SET_SHIFT  0x0001

// Display on off
#define DIS_DEF       0x0008
#define DIS_LCD       0x0004
#define DIS_CURSOR    0x0002
#define DIS_CUR_BLINK 0x0001

// shift
#define CUR_DIS_DEF   0x0010
#define CUR_DIS_SHIFT 0x0008
#define CUR_DIS_DIR		0x0004

// set DDRAM  address
#define SET_DDRAM_ADD_DEF 0x0080

// read bit
#define BUSY_BIT_nTH      7
#define DDRAM_ADD_MASK		0x007F
#define Busy_Flag         0x0010

#define DDRAM_ADDR_LINE_1	0x0000
#define DDRAM_ADDR_LINE_2	0x0040

#define LINE_NUM    2
#define COLUMN_NUM  16

#define ALL_LINE    0
#define LINE_1      1
#define LINE_2      2

void TextlcdInit();
// line argument =>must be 1 or 2 .
int writeTextLine(int line ,const char* str);
int clearScreen(int nline);
int displayMode(int bCursor, int bCursorblink, int blcd  );
int setCursorMode(int bMove , int bRightDir);

#endif /* SRC_TEXTLCD_H_ */
