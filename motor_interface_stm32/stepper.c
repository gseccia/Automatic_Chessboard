/*
 * stepper.c
 *
 *  Created on: 06 giu 2019
 *      Author: Giuseppe
 */
#include "stepper.h"
#include "gpio.h"


Stepper stepper_init(){
	Stepper s;

	s.direction=1;
	s.number_of_steps=4;
	s.step_delay= 60 * 1000  / 4 / 30;
	s.step_number=0;

	return s;
}

/*
 * Sets the speed in revs per minute
 *
 * velocità accettabili sull'ordine delle migliaia (1000-5000)
 */
void setSpeed(Stepper* stepper, int whatSpeed){
	stepper->step_delay = 60 * 1000  / stepper->number_of_steps / whatSpeed; // * 1000L microsends???
}

// direction setter methood:
void setDirection(Stepper* stepper,int new_direction){
    stepper->direction = new_direction;
}

// mover method:

void move_n_steps(Stepper* stepper, int number_of_steps){

	int steps_left = number_of_steps;  // how many steps to take

	// decrement the number of steps, moving one step each time:
	while (steps_left > 0){
		HAL_Delay(stepper->step_delay);
		// move only if the appropriate delay has passed:

		// increment or decrement the step number,
		// depending on direction:
		if (stepper->direction == 1){
			stepper->step_number++;
			if (stepper->step_number == stepper->number_of_steps) {
			  stepper->step_number = 0;
			}
		}else {
			if (stepper->step_number == 0) {
			  stepper->step_number = stepper->number_of_steps;
			}
			stepper->step_number--;
		}
		// decrement the steps left:
		steps_left--;
		// step the motor to step number 0, 1, ..., {3 or 10}
		step(stepper->step_number);
	}
}

void step(int thisStep){
	switch (thisStep) {
	      case 0:  // 1010
	    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
	    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	    	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		  break;
	      case 1:  // 0110
	    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
	    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	    	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		  break;
	      case 2:  //0101
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		  break;
	      case 3:  //1001
	    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
	    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	    	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		  break;
	    }
}

void move_half_cell(Stepper* stepper){

}

void move_full_cell(Stepper* stepper){

}

void move_n_cells(Stepper* stepper, int num_cell){
	int i;
	for(i=0; i<num_cell; i++){
		move_full_cell(stepper);
	}

}
