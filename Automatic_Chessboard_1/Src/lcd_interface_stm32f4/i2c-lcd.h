#ifndef I2C-LCD_H_
#define I2C-LCD_H_
#include "stm32f4xx_hal.h"
#include "String.h"

// commands
typedef enum {
	LCD_CLEARDISPLAY 	=	0x01,
	LCD_RETURNHOME   	=	0x02,
	LCD_ENTRYMODESET 	=	0x04,
	LCD_DISPLAYCONTROL 	=	0x08,
	LCD_CURSORSHIFT  	=	0x10,
	LCD_FUNCTIONSET 	=	0x20,
	LCD_SETCGRAMADDR 	=	0x40,
	LCD_SETDDRAMADDR 	=	0x80
} lcd_commands;

// flags for display entry mode
typedef enum {
	LCD_ENTRYRIGHT  		= 0x00,
	LCD_ENTRYLEFT  			= 0x02,
	LCD_ENTRYSHIFTINCREMENT = 0x01,
	LCD_ENTRYSHIFTDECREMENT = 0x00,
} lcd_display_flags;

// flags for display on/off control
typedef enum {
	LCD_DISPLAYON  		=	0x04,
	LCD_DISPLAYOFF 		=	0x00,
	LCD_CURSORON 		=	0x02,
	LCD_CURSOROFF 		=	0x00,
	LCD_BLINKON 		=	0x01,
	LCD_BLINKOFF 		=	0x00
} lcd_display_controls;

// flags for display/cursor shift
typedef enum {
	LCD_DISPLAYMOVE 	=	0x08,
	LCD_CURSORMOVE 		=	0x00,
	LCD_MOVERIGHT 		=	0x04,
	LCD_MOVELEFT 		=	0x00
} lcd_flags_shift;

// flags for function set
typedef enum {
	LCD_8BITMODE 	=		0x10,
	LCD_4BITMODE 	=		0x00,
	LCD_2LINE 		=		0x08,
	LCD_1LINE 		=		0x00,
	LCD_5x10DOTS 	=		0x04,
	LCD_5x8DOTS 	=		0x00
} lcd_function_set_flags;

// flags for backlight control
typedef enum {
	LCD_BACKLIGHT 		=	0x08,
	LCD_NOBACKLIGHT     =    0x00
}lcd_backlight_flags;

typedef enum {
	EN =  0b00000100, // Enable bit
	RW =  0b00000010,// Read/Write bit
	RS =  0b00000001 // Register select bit
} lcd_bitmask;
/*
 * Function:  lcd_init
 * --------------------
 *
 * DESCRIBE BETTER...
 */
void lcd_init (void);

/*
 * Function:  lcd_send_cmd
 * --------------------
 *
 * Send a byte representing the command to the LCD (take care, correct command should be send before
 * sending data)
 * DESCRIBE BETTER...
 */
void lcd_send_cmd (char cmd);

/*
 * Function:  lcd_send_data
 * --------------------
 *
 * Send a byte representing a data to the LCD (take care, correct command should be send before
 * sending data)
 * DESCRIBE BETTER...
 */
void lcd_send_data (char data);

/*
 * Function:  lcd_send_str
 * --------------------
 *
 * Send an entire string to the LCD (take care, correct command should be send before
 * sending data)
 * DESCRIBE BETTER...
 */
void lcd_send_string (char *str, int n_line);

/*
 * Function:  lcd_send_str
 * --------------------
 *
 * Send an entire string to the LCD (take care, correct command should be send before
 * sending data)
 * DESCRIBE BETTER...
 */
void lcd_clear(void);

#endif
