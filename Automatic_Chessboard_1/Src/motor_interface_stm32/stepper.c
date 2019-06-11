/*
 * stepper.c
 *
 *  Created on: 06 giu 2019
 *      Author: Giuseppe
 */
#include "stepper.h"
#include "gpio.h"


Stepper stepper_init(GPIO_TypeDef* group_pin_direction, uint16_t pin_direction, GPIO_TypeDef* group_pin_step, uint16_t pin_step, uint16_t step_delay){
	Stepper s;
	s.group_pin_direction = group_pin_direction;
	s.pin_direction = pin_direction;
	s.group_pin_step = group_pin_step;
	s.pin_step = pin_step;
	s.step_delay = step_delay;

	return s;
}

// direction setter methood:
void setDirection(Stepper* stepper, Step_direction direction){
    if(direction == FORWARD)
    	HAL_GPIO_WritePin(s.group_pin_direction, s.pin_direction, SET);
    else if(direction == BACKWARD)
    	HAL_GPIO_WritePin(s.group_pin_direction, s.pin_direction, RESET);
}

// mover method:

void move_n_steps(Stepper* stepper, uint16_t number_of_steps, Step_direction direction){

	stepper->setDirection(direction);

	for(int i=0; i<number_of_steps; i++){
		HAL_GPIO_WritePin(s.group_pin_step, s.pin_step, SET);
		HAL_Delay(stepper->step_delay);
		HAL_GPIO_WritePin(s.group_pin_step, s.pin_step, RESET);
		HAL_Delay(stepper->step_delay);
	}
}


void move_half_cell(Stepper* stepper, Step_direction direction){

}

void move_full_cell(Stepper* stepper, Step_direction direction){

}

void move_n_cells(Stepper* stepper, int num_cell){
	int i;
	for(i=0; i<num_cell; i++){
		move_full_cell(stepper);
	}

}
