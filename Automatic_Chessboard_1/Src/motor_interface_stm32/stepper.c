/*
 * stepper.c
 *
 *  Created on: 06 giu 2019
 *      Author: Giuseppe
 */
#include "stepper.h"
#include "gpio.h"


Stepper stepper_init(GPIO_TypeDef* group_pin_direction, uint16_t pin_direction, GPIO_TypeDef* group_pin_step, uint16_t pin_step, uint32_t step_delay,int x){
	Stepper s = {
	  .group_pin_direction = group_pin_direction,
	  .pin_direction = pin_direction,
	  .group_pin_step = group_pin_step,
	  .pin_step = pin_step,
	  .step_delay = step_delay,
	  .x =x,
	};
	return s;
}

// direction setter methood:
void setDirection(Stepper* s, Step_direction direction){
    if(direction == FORWARD)
    	HAL_GPIO_WritePin(s->group_pin_direction, s->pin_direction, GPIO_PIN_SET);
    else if(direction == BACKWARD)
    	HAL_GPIO_WritePin(s->group_pin_direction, s->pin_direction, GPIO_PIN_RESET);
}

// mover method:

void move_n_steps(Stepper* s, int number_of_steps, Step_direction direction){

	setDirection(s,direction);
	int i;
	for(i=0; i<number_of_steps; i++){
		HAL_GPIO_WritePin(s->group_pin_step, s->pin_step, GPIO_PIN_SET);
		HAL_Delay(s->step_delay);
		HAL_GPIO_WritePin(s->group_pin_step, s->pin_step, GPIO_PIN_RESET);
		HAL_Delay(s->step_delay);
	}
}


void move_half_cell(Stepper* stepper, Step_direction direction){
	if(stepper->x)move_n_steps(stepper, 143, direction);
	else move_n_steps(stepper, 160, direction);
}

void move_full_cell(Stepper* stepper, Step_direction direction){
	move_half_cell(stepper, direction);
	move_half_cell(stepper, direction);
}

void move_n_cells(Stepper* stepper, int num_cell, Step_direction direction){
	int i;
	for(i=0; i<num_cell; i++){
		move_full_cell(stepper,direction);
	}

}
