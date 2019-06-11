/*
 * scelta.c
 *
 *  Created on: 27 mag 2019
 *      Author: salva
 */
#include "scelta.h"
#include "i2c-lcd.h"
#include "adc.h"
#include "string.h"
#include "gpio.h"
#include "stdio.h"


/*SCELTA scelta_Init(int n, char msgA[], char msgB[]){
	SCELTA scelta = {
			scelta.nbOfScreen = n,
			strcpy(scelta.op_msg1[16], msgA),
			strcpy(scelta.op_msg2[16], msgB)
	};
	return scelta;
}*/

int createMenu(char* screen[], int cont, float y, int n){       //screen is the array containing the string of the menu, cont is the current screen
															   // 0 <= cont < n


		if (y < 10){
			cont = cont-1;
			if (cont < 0){
				cont = 0;
			}
		}else if (y> 800){
			cont = cont+1;
			if (cont > n-1){
				cont = n-1;
			}
		}
		lcd_clear();
		lcd_send_string(screen[cont],1);
		HAL_Delay(300);



		int stateOfPushButton = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
			if (stateOfPushButton == 1) {
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
			} else {
				choose(cont, screen[cont]);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
			}

	return cont;

}


void choose(int cont, char screen[]){
	char msg[] = "You have chosen";
	char choice[16];
	strcpy(choice, screen);
	lcd_send_string(msg,1);
	lcd_send_string(choice,2);
	HAL_Delay(2000);
	lcd_clear();
}




