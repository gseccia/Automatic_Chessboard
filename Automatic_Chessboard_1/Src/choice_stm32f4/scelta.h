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
#include "i2c-lcd.h"


/*typedef struct SCELTA{
	int nbOfScreen;
	char op_msg1[16];
	char op_msg2[16];
}SCELTA;*/


//SCELTA scelta_Init(int n, char msgA[], char msgB[]);

int createMenu(char* screen[], int cont, float y, int n);

void choose(int cont, char screen[]);


#endif /* SCELTA_H_ */
