/*
 * scelta.h
 *
 *  Created on: 27 mag 2019
 *      Author: salva
 */

#ifndef SCELTA_H_
#define SCELTA_H_
#include "stm32f4xx_hal.h"
#include "String.h"
#include "../lcd_interface_stm32f4/i2c-lcd.h"

#define MAXCHAR 16
#define MAXMENU 4
#define MAXCHOICE 4

typedef struct menu{
	char title[MAXCHAR];
	int *variable;
	int chioces_len;
	char choices[MAXCHOICE][MAXCHAR];
}menu;

typedef struct menu_manager{
	ADC_HandleTypeDef handle;
	menu* menus[MAXMENU];
	int menu_len;
	uint32_t raw_values[2];
	int center_y;
	int center_x;

	int current_menu;
	int current_choice;
}menu_manager;


menu_manager* menu_manager_init(ADC_HandleTypeDef handle);

int createMenu(menu_manager* manager,char title[],char chioces[][MAXCHAR], int choice_len, int *var);

void change_choice(menu_manager* manager);
void change_menu(menu_manager* manager);
void next_menu(menu_manager* manager);

void show_menu(menu_manager* manager,int choice_menu,int choice);


#endif /* SCELTA_H_ */
