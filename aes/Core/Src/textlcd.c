/*
 * textlcd.c
 *
 *  Created on: Jul 9, 2021
 *      Author: cndi
 */

#include "textlcd.h"  
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/***************************************************
read /write  sequence
write cycle
RS,(R/W) => E (rise) => Data => E (fall)

***************************************************/
// Common define
#define TRUE            1
#define FALSE           0

#define DELAY_S    HAL_Delay(1)
#define RW_HIGH   {HAL_GPIO_WritePin(LCD_SIGNAL_CON, (0x0100 << LCD_RW), GPIO_PIN_SET);HAL_Delay(1);}
#define RW_LOW   {HAL_GPIO_WritePin(LCD_SIGNAL_CON, (0x0100 << LCD_RW), GPIO_PIN_RESET);HAL_Delay(1);}
#define E_HIGH   {HAL_GPIO_WritePin(LCD_SIGNAL_CON, (0x0100 << LCD_E), GPIO_PIN_SET);HAL_Delay(1);}
#define E_LOW   {HAL_GPIO_WritePin(LCD_SIGNAL_CON, (0x0100 << LCD_E), GPIO_PIN_RESET);HAL_Delay(1);}
#define RS_HIGH   {HAL_GPIO_WritePin(LCD_SIGNAL_CON, (0x0100 << LCD_RS), GPIO_PIN_SET);HAL_Delay(1);}
#define RS_LOW   {HAL_GPIO_WritePin(LCD_SIGNAL_CON, (0x0100 << LCD_RS), GPIO_PIN_RESET);HAL_Delay(1);}

// 4)
// TRUE=> out  , FALSE => In
void dataportDirectionOut(int bTrue)
{
  GPIO_InitTypeDef  GPIO_InitStruct = {0};

  if ( bTrue)
  {
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  }
  else
  {
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
  }
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

// 5)
int IsBusy()
{
  int  rdata;
  dataportDirectionOut(FALSE);  // dataport mode: input set
  RW_HIGH
  RS_LOW

  E_HIGH

  rdata = HAL_GPIO_ReadPin(LCD_DATA_CON, (0x0001 << BUSY_BIT_nTH)); // data port read
  E_LOW

  RW_LOW

  if (rdata & Busy_Flag)       // Busy Flag(BF) bit check 
    return TRUE;

  return FALSE;
}

// 6)
void writeDataPort(int nData)
{
  int i;
  for ( i = 0; i < 8 ; i++)
  {
    if ( (nData>>i) & 1 )

      HAL_GPIO_WritePin(LCD_DATA_CON, (0x0001 << i), GPIO_PIN_SET);
    else
      HAL_GPIO_WritePin(LCD_DATA_CON, (0x0001 << i), GPIO_PIN_RESET);
  } 
}

// 7)
int writeCmd(int nCmd)
{
  if ( IsBusy())
    return FALSE;

  dataportDirectionOut(TRUE); // dataport output
  
  RW_LOW
    
  RS_LOW
    
  writeDataPort(nCmd);

  E_HIGH

  E_LOW
    
  dataportDirectionOut(FALSE); // dataport input

  return TRUE;
}

// 8)
int setDDRAMAddr(int x , int y)
{
  int cmd = 0;
  if(IsBusy())
    return FALSE;

  if ( y == 1 )
    cmd = DDRAM_ADDR_LINE_1 +x;
  else if(y == 2 )
    cmd = DDRAM_ADDR_LINE_2 +x;
  else
    return FALSE;

  if ( cmd >= 0x80)
    return FALSE;

  if (!writeCmd(cmd | SET_DDRAM_ADD_DEF))
    return FALSE;

  HAL_Delay(1);
  return TRUE;
}

// 9)
int displayMode(int bCursor, int bCursorblink, int blcd  )
{
  int cmd  = 0;

  if ( bCursor)
    cmd = DIS_CURSOR;

  if (bCursorblink )
    cmd |= DIS_CUR_BLINK;

  if ( blcd )
    cmd |= DIS_LCD;

  if (!writeCmd(cmd | DIS_DEF))
    return FALSE;

  return TRUE;
}

// 10)
int writeCh(int ch)
{
  if ( IsBusy())
    return FALSE;

  dataportDirectionOut(TRUE); // dataport output

  writeDataPort(ch);
  
  RS_HIGH

  E_HIGH

  E_LOW

  HAL_Delay(1);
  dataportDirectionOut(FALSE); // dataport input
  
  return TRUE;
}

int setCursorMode(int bMove , int bRightDir)
{
  int cmd = MODE_SET_DEF;

  if (bMove)
    cmd |=  MODE_SET_SHIFT;

  if (bRightDir)
    cmd |= MODE_SET_DIR_RIGHT;

  if (!writeCmd(cmd))
    return FALSE;
  return TRUE;
}

// 11)
int functionSet(void)
{
  int cmd = 0x0038; // 5*8 dot charater , 8bit interface , 2 line

  if (!writeCmd(cmd))
    return FALSE;
  return TRUE;
}

// 12)
int writeStr(char* str)
{
  int cdata;
  int i;
  for(i =0; i < strlen(str) ;i++ )
  {
    if (str[i] == '_')
      cdata = (int)' ';
    else
      cdata = (int)str[i];
    writeCh(cdata);
  }
  return TRUE;
}

// 13)
// 0=> all line, 1= 1th line , 2 = 2th line
int clearScreen(int nline)
{
  int i;
  if (nline == ALL_LINE)
  {
    if(IsBusy())                // Busy Flag Check
      return FALSE;

    if (!writeCmd(CLEAR_DISPLAY))       // clear display 실행 여부
      return FALSE;

    return TRUE;
  }
  else if (nline == LINE_1)
  {
    setDDRAMAddr(0,1);          // 0x80 - Set DDRAM Address
    for(i = 0; i <= COLUMN_NUM ;i++ )
      writeCh((unsigned char)' ');

    setDDRAMAddr(0,1);
  }
  else if (nline == LINE_2)
  {
    setDDRAMAddr(0,2);          // 0xC0 - Move into 2nd Line

    for(i = 0; i <= COLUMN_NUM ;i++ )
      writeCh((unsigned char)' ');

    setDDRAMAddr(0,2);
  }
  return TRUE;
}

// 14)
int initlcd(void)
{
  E_LOW
  
  functionSet();
  HAL_Delay(1);
  displayMode(1,1,1);

  return 1;
}

// 15)
void TextlcdInit()
{
  initlcd();
}

// 16)
// line argument =>must be 1 or 2 . 
int writeTextLine(int line ,const char* str)
{
  char strWtext[COLUMN_NUM+1];
  if (strlen(str) > COLUMN_NUM )
  {
    strncpy(strWtext,str,COLUMN_NUM);
    strWtext[COLUMN_NUM] = '\0';
  }
  else
  {
    strcpy(strWtext,str);
  }
  setDDRAMAddr(0, line);
  HAL_Delay(2);
  writeStr(strWtext);

  return 1;
}
