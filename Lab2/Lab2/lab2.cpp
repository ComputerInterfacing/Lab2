#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <NetworkDebug.h>
#include "keypad.h"
#include "error_wrapper.h"
#include "LCD.h"
#include <string.h>

extern "C"
{
	void UserMain(void *pd);
}

const char *AppName = "Kathleen and Abuni";
Keypad myKeypad;
LCD myLCD;
OS_Q myQueue;
// sprite pattern
BYTE sprite[] = {0xAA, 0x55,0xAA, 0x55,0xAA, 0x55,0xAA};
int LCD_loc;

#define COLUMN_BEGIN 0
#define COLUMN_END 77
#define COLUMN_INTERVAL 1
#define ROW_INTERVAL 12

void UserMain(void *pd)
{
	BYTE err = OS_NO_ERR;
	InitializeStack();
	OSChangePrio(MAIN_PRIO);
	EnableAutoUpdate();
	StartHTTP();
	EnableTaskMonitor();

#ifndef _DEBUG
	EnableSmartTraps();
#endif

#ifdef _DEBUG
	InitializeNetworkGDB_and_Wait();
#endif



	/* initialize keypad with interrupt mode */
	myKeypad.Init(KEYPAD_INT_MODE);
	myLCD.Init();

	iprintf("Application started %s\n", AppName);
	myLCD.Clear();
	/* initial drawing for the sprite */
	LCD_loc = LINE1_ORIGIN;
	myLCD.DrawChar(sprite, char_index[LCD_loc]);

	while (1)
	{
		/* block and wait forever for the data about the pressed button from myQueue */
		const char *pData = (const char *)OSQPend(&myQueue, WAIT_FOREVER, &err);
		/* no data, this shouldn't happen */
		if (pData == NULL)
		{
			iprintf("null\n"); // output for debug
		}
		/* received data from myQueue */
		else
		{
			iprintf("pend: %s\n", pData); // output for debug
			/* pressed down button (bottom-right corner of keypad) */
			if (strcmp(pData, "down") == 0)
			{
				/* if didn't reach the bottom row (line 6), move sprite down */
				if (LCD_loc < LINE6_ORIGIN)
				{
					myLCD.Clear();								 // clear screen
					LCD_loc += ROW_INTERVAL;					 // move down
					myLCD.DrawChar(sprite, char_index[LCD_loc]); // draw sprite
				}
			}
			/* pressed up button (top-right corner of keypad) */
			else if (strcmp(pData, "up") == 0)
			{
				/* if didn't reach the top row (line 1), move sprite up */
				if (LCD_loc > LINE1_END)
				{
					myLCD.Clear();								 // clear screen
					LCD_loc -= ROW_INTERVAL;					 // move up
					myLCD.DrawChar(sprite, char_index[LCD_loc]); // draw sprite
				}
			}
			/* pressed left button (top-left corner of keypad) */
			else if (strcmp(pData, "left") == 0)
			{
				/* if didn't reach the leftmost column (column 1), move sprite left */
				if (char_index[LCD_loc].col != COLUMN_BEGIN)
				{
					myLCD.Clear();								 // clear screen
					LCD_loc -= COLUMN_INTERVAL;					 // move left
					myLCD.DrawChar(sprite, char_index[LCD_loc]); // draw sprite
				}
			}
			/* pressed right button (bottom-left corner of keypad) */
			else if (strcmp(pData, "right") == 0)
			{
				/* if didn't reach the rightmost column (column 12), move sprite right */
				if (char_index[LCD_loc].col != COLUMN_END)
				{
					myLCD.Clear();								 // clear screen
					LCD_loc += COLUMN_INTERVAL;					 // move right
					myLCD.DrawChar(sprite, char_index[LCD_loc]); // draw sprite
				}
			}
		}
	}
}
