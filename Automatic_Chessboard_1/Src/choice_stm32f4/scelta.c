/*
 * scelta.c
 *
 *  Created on: 27 mag 2019
 *      Author: salva
 */
#include "scelta.h"
#include "adc.h"
#include "string.h"
#include "gpio.h"
#include "stdio.h"

typedef struct menu_manager menu_manager;
typedef struct menu menu;


menu_manager* menu_manager_init(ADC_HandleTypeDef handle){
	 menu_manager*  menu_man = (menu_manager*)malloc(sizeof(menu_manager));
	 menu_man->menu_len = 0;
	 menu_man->current_menu = -1;
	 menu_man->current_choice = -1;
	 menu_man->center_y = 1;
	 menu_man->center_x = 1;
	 menu_man->raw_values[0] = 0;
	 menu_man->raw_values[1] = 0;

	 lcd_init();
	 lcd_clear();
	 return menu_man;
}


int createMenu(menu_manager* manager,char title[],char chioces[][MAXCHAR], int choice_len,int* var,int valid){
	if(manager->menu_len < MAXMENU && choice_len >= 1){
		(manager->menus)[manager->menu_len] = (menu*)malloc(sizeof(menu));
		int i;

		strcpy((manager->menus)[manager->menu_len]->title,title);
		for(i=0;i<choice_len;i++)
			strcpy(((manager->menus)[manager->menu_len]->choices)[i],chioces[i]);
		(manager->menus)[manager->menu_len]->chioces_len=choice_len;
		(manager->menus)[manager->menu_len]->variable=var;
		(manager->menus)[manager->menu_len]->valid=valid;

		manager->menu_len++;
		return 0;
	}
	else return -1;
}

void set_invalid_menu(menu_manager* manager,int choice){
	(manager->menus)[choice]->valid = 0;
}

void set_valid_menu(menu_manager* manager,int choice){
	(manager->menus)[choice]->valid = 1;
}

int is_legal_menu(menu_manager* manager,int choice_menu){
	return (manager->menus)[choice_menu]->valid;
}

void show_menu(menu_manager* manager,int choice_menu,int choice){
	if(manager->menu_len <= choice_menu || choice_menu < 0 || choice < 0 || (manager->menus)[choice_menu]->chioces_len <= choice){
		lcd_clear();
		lcd_send_string("Error",1);
		lcd_send_string("Invalid choice",2);
	}
	else if(is_legal_menu(manager,choice_menu)){
		lcd_clear();
		lcd_send_string((manager->menus)[choice_menu]->title,1);
		lcd_send_string(((manager->menus)[choice_menu]->choices)[choice],2);

		manager->current_menu = choice_menu;
		manager->current_choice = choice;
	}

	HAL_Delay(10);
}

void change_choice(menu_manager* manager){
	uint32_t y;

	// x = manager->raw_values[0];
	y = manager->raw_values[0];

	if(manager->center_y){
		if (y < 10 && manager->current_choice-1 > -1){
			manager->current_choice--;
			show_menu(manager,manager->current_menu,manager->current_choice);
			manager->center_y = 0;
			if((manager->menus)[manager->current_menu]->variable != NULL)
				*((manager->menus)[manager->current_menu]->variable) = manager->current_choice;

		}else if (y> 1000 && manager->current_choice+1 < (manager->menus)[manager->current_menu]->chioces_len){
			manager->current_choice++;
			show_menu(manager,manager->current_menu,manager->current_choice);
			manager->center_y = 0;
			if((manager->menus)[manager->current_menu]->variable != NULL)
				*((manager->menus)[manager->current_menu]->variable) = manager->current_choice;
		}
	}
	else{
		manager->center_y = (y > 10 && y < 1000);
	}
}

void change_menu(menu_manager* manager){
	uint32_t x;
	x = manager->raw_values[1];

	if(manager->center_x){
		if (x < 10 && manager->current_menu-1 > -1){
			manager->current_menu--;
			manager->center_x = 0;
			show_menu(manager,manager->current_menu,0);
		}else if (x > 1000 && manager->current_menu+1 < manager->menu_len){
			manager->current_menu++;
			manager->center_x = 0;
			show_menu(manager,manager->current_menu,0);
		}
	}
	else {
		manager->center_x = (x>10 && x<1000);
	}
}

void next_menu(menu_manager* manager){
	show_menu(manager,manager->current_menu+1,0);
}




