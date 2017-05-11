#include "LCD.h"

void LCD_WriteInstruction(uint8_t instr) //Write instruction to the LCD according to data sheet
{
	LCD_PORT = (1<<0);
	uint8_t out = instr & 0b11110000; //Upper bits
	
	LCD_PORT |= out; //Send the Upper four bits
	_delay_us (100); //Delay as needed
	LCD_PORT |= ENABLE; //Set the ENABLE PIN as needed
	_delay_us (100);
	LCD_PORT &= ~ENABLE; //Unset the ENABLE PIN
	_delay_us (50);
	
	LCD_PORT &= (1<<0);
	out = instr << 4; //Four Lower bits
	LCD_PORT |= out; //Send the bits
	_delay_us (100);
	LCD_PORT |= ENABLE; //Set the ENABLE PIN as needed
	_delay_us (100);
	LCD_PORT &= ~ENABLE; //Unset the ENABLE PIN
	_delay_us (50);
}

void LCD_WriteChar(unsigned char data)
{
	LCD_PORT &= (1<<0);
	unsigned char out = data & 0b11110000; //Upper four bits
	
	LCD_PORT |= (out | RS); //Send the data out and also set the RS PIN to high as needed
	_delay_us (100); //Delay as protocol suggests
	LCD_PORT |= ENABLE; //Set the Enable PIN to high as needed
	_delay_us (100); //Delay
	LCD_PORT &= ~ENABLE; 
	_delay_us (50);
	
	LCD_PORT &= (1<<0);	
	out = data << 4; //Lower four bits
	LCD_PORT |= (out | RS);
	_delay_us (100);
	LCD_PORT |= ENABLE; //Set the ENABLE to HIGH
	_delay_us (100);
	LCD_PORT &= ~ENABLE; //And now LOW
	_delay_us (50);
}

void LCD_WriteStr(char *str_data)
{
	uint8_t string_len = strlen(str_data);
	for(uint8_t i = 0; i < string_len; i++)
		LCD_WriteChar(str_data[i]);
}

void LCD_SetCursor (uint8_t x_position, uint8_t y_position)
{
	if(y_position == 1)
		y_position = 0;
	else if(y_position == 2)
		y_position = 0x40;
	
	#if LCD_ROWS > 2
		else if(y_position == 3) //These conditions for the 4 row LCD
			y_position = 0x14;
		else if(y_position == 4)
			y_position = 0x54;
	#endif
	LCD_WriteInstruction(LCD_SETDDRAM_ADRR_COMMAND | (x_position + y_position));
}

inline void LCD_ClearDisplay(void) //Clear display and reset cursor
{
	LCD_WriteInstruction(CLEAR_DISP_RES_CURS);
	_delay_ms(2);
}

void InitLCD (void)
{
	LCD_DDR |= ~(1<<0); // PORTx1:7 outputs (LCD)
	LCD_PORT &= (1<<0); //Start the LCD
	
	_delay_ms (20); //Power on delay needed in order for the internal circuits to stabilize
	uint8_t out = 0b00110000; //Indicate 4-bit long mode
	
	//First initialization as suggested from the data sheet
	LCD_PORT |= out;
	_delay_us (100);
	LCD_PORT |= ENABLE;
	_delay_us (100);
	LCD_PORT &= ~ENABLE;
	_delay_ms (5);
	
	//Second initialization as suggested from the data sheet
	LCD_PORT |= ENABLE;
	_delay_us (100);
	LCD_PORT &= ~ENABLE;
	_delay_us (200);
	
	//Third and final initialization as suggested from the data sheet
	LCD_PORT |= ENABLE;
	_delay_us (100);
	LCD_PORT &= ~ENABLE;
	_delay_ms (5);
	
	LCD_PORT &= (1<<0);
	out = 0b00100000; //Indicate that we want a 4-bit interface mode
	LCD_PORT |= out;
	_delay_us (100);
	LCD_PORT |= ENABLE;
	_delay_us (100);
	LCD_PORT &= ~ENABLE;
	_delay_us (50);
	
	//Finlay send the necessary instructions to finalize the LCD initialization. After this commands and the delay, the LCD is ready for use
	LCD_WriteInstruction(INTISL_DISPLAY_FUNC_SET | (1 << 2)); // 001, 4 bit, 2 lines 2x16 (4 lines 4x20), 5x11 dots (LCD),xx
	LCD_WriteInstruction(DISP_ON_CUR_NS_COMMAND & ~(1 << 2)); // 00001xxx, Display off, cursor off, blinking off (LCD)
	LCD_WriteInstruction(CURS_MOV_DIR_DISP_NOT_SFT); // 000001xx, Cursor increase, display not shift (LCD)
	LCD_WriteInstruction(CLEAR_DISP_RES_CURS); //Clear display, reset cursor (LCD)
	_delay_ms (2); //Give the needed time to the LCD to be initialized internally, in order to be ready to receive commands
	
	LCD_WriteInstruction(DISP_ON_CUR_NS_COMMAND); //Turn the Display on
}

void LCD_AutoScroll(void)
{
	LCD_WriteInstruction(CURS_MOV_DIR_DISP_NOT_SFT | (1 << 0));
	_delay_us(50);
}

void LCD_No_AutoScroll(void)
{
	LCD_WriteInstruction(CURS_MOV_DIR_DISP_NOT_SFT & ~(1 << 0));
	_delay_us(50);
}

void LCD_CursorBlink(void)
{
	LCD_WriteInstruction(DISP_ON_CUR_NS_COMMAND | (1 << 0));
	_delay_us(50);
}

void LCD_No_CursorBlink(void)
{
	LCD_WriteInstruction(DISP_ON_CUR_NS_COMMAND & ~(1 << 0));
	_delay_us(50);
}

void LCD_ShowCursor(void)
{
	LCD_WriteInstruction(DISP_ON_CUR_NS_COMMAND | (1 << 1));
	_delay_us(50);
}

void LCD_HideCursor(void)
{
	LCD_WriteInstruction(DISP_ON_CUR_NS_COMMAND & ~(1 << 1));
	_delay_us(50);
}

void LCD_Display_ON(void)
{
	LCD_WriteInstruction(DISP_ON_CUR_NS_COMMAND | (1 << 2));
	_delay_us(50);
}

void LCD_Display_OFF(void)
{
	LCD_WriteInstruction(DISP_ON_CUR_NS_COMMAND & ~(1 << 2));
	_delay_us(50);
}

void LCD_Text_Dir_RightToLeft(void)
{
	LCD_WriteInstruction(CURS_MOV_DIR_DISP_NOT_SFT & ~(1 << 1));
	_delay_us(50);
}

void LCD_Text_Dir_LeftToRight(void)
{
	LCD_WriteInstruction(CURS_MOV_DIR_DISP_NOT_SFT | (1 << 1));
	_delay_us(50);
}

void LCD_Scroll_Disp_Left(void)
{
	//If you want display to shift left, set the 0001x(n)00 (n) bit to zero
	LCD_WriteInstruction(DISP_SFT_AND_CURS_SFT | (1 << 3));
	_delay_us(50);
}

void LCD_Scroll_Disp_Right(void)
{
	//If you want display to shift right, set the 0001x(n)00 (n) bit to one
	LCD_WriteInstruction(DISP_SFT_AND_CURS_SFT & ~(1 << 3));
	_delay_us(50);
}