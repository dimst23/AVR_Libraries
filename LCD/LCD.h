#ifndef LCD_H_
#define LCD_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

/*
* All the LCD PINS are on one port
* Pin out for the PORT -> P1=R/W, P2=RS, P3=Enable, P4=D4, P5=D5, P6=D6, P7=D7
*/

#define LCD_PORT PORTD //Define the port of the MCU that the LCD is on
#define LCD_DDR DDRD //And also define the register of the PORT

#define RS 0b00000100 //RS PIN 
#define ENABLE 0b00001000 //Enable PIN

#define CLEAR_DISP_RES_CURS 0b00000001 //Clear Display and Reset Cursor (instruction)
#define DISP_ON_CUR_NS_COMMAND 0b00001100
#define INTISL_DISPLAY_FUNC_SET 0b00101000 // 001, 4 bit, 2 lines 2x16 (4 lines 4x20), 5x8 dots (LCD),xx
#define CURS_MOV_DIR_DISP_NOT_SFT 0b00000110 // 000001xx, Cursor increase, display not shift (LCD)
#define DISP_SFT_AND_CURS_SFT 0b00010000 //No shift of the display and no cursor shift
#define LCD_SETDDRAM_ADRR_COMMAND 0b10000000 //Command to send the cursor to a specific DDRAM address

#define LCD_COLS 20 //Number of the LCD columns
#define LCD_ROWS 4 //Number of the LCD rows

extern void LCD_WriteInstruction(uint8_t instr); //Function to write an instruction to LCD
extern void LCD_WriteChar(unsigned char data); //Function to write data (ASCII characters) to LCD
extern void LCD_SetCursor(uint8_t x_position, uint8_t y_position); //Send cursor to a specific address (0-80)
extern void InitLCD(void); //LCD initialization
extern void LCD_ClearDisplay(void); //Clear the display and reset cursor
extern void LCD_WriteStr(char *str_data); //Write a string on the LCD

//LCD Display formating functions
extern void LCD_AutoScroll(void); //Automatic display scrolling
extern void LCD_No_AutoScroll(void); //Stop the automatic display scrolling
extern void LCD_CursorBlink(void); //Show the cursor and make it blink
extern void LCD_No_CursorBlink(void); //Stop the blinking of the cursor and remove it
extern void LCD_ShowCursor(void); //Show the cursor as an underscore
extern void LCD_HideCursor(void); //Hide the cursor displayed
extern void LCD_Display_ON(void); //Turn the display on
extern void LCD_Display_OFF(void); //Turn the display off, without loosing the data
extern void LCD_Text_Dir_RightToLeft(void); //Set the cursor moving direction to the right
extern void LCD_Text_Dir_LeftToRight(void); //Set the cursor moving direction to the left
extern void LCD_Scroll_Disp_Left(void); //Scroll the display once to the left
extern void LCD_Scroll_Disp_Right(void); //Scroll the display once to the right

#endif