/*
 * axis_manager.c
 *
 *  Created on: 07 giu 2019
 *      Author: Antonio
 */
#include "axis_manager.h"

typedef struct Axis_manager Axis_manager;


Axis_manager* axis_manager_init(TIM_HandleTypeDef *tim_pwm_handler,int frequency,uint16_t front_x,uint16_t back_x,uint16_t front_y,uint16_t back_y ){
	Axis_manager* axis_manager = (Axis_manager*)malloc(sizeof(Axis_manager));

	// Initialize logic position
	struct cell current_position;
	current_position.row = UNKNOWN;
	current_position.column = UNKNOWN;
	axis_manager->current_position = current_position;

	// Initialize Steppers and servo
	axis_manager->x_stepper = stepper_init(); // Lista dei PIN ?
	axis_manager->y_stepper = stepper_init();
	axis_manager->hook = SERVO_HOOK_Init(tim_pwm_handler,frequency);

	// Initialize limit PIN
	axis_manager->x_front = front_x;
	axis_manager->x_back = back_x;
	axis_manager->y_front = front_y;
	axis_manager->y_back = back_y;

	return axis_manager;
}


void axis_manager_reset_position(Axis_manager* axis){
	setDirection(&(axis->x_stepper),0);
	setDirection(&(axis->y_stepper),0);
	SERVO_HOOK_off(&(axis->hook));

	while(!axis_manager_check_limit(axis,1,1) && !axis_manager_check_limit(axis,0,1)){
		move_half_cell(&(axis->x_stepper));
		move_half_cell(&(axis->y_stepper));
	}

	while(!axis_manager_check_limit(axis,1,1)){
			move_half_cell(&(axis->x_stepper));
	}

	while(!axis_manager_check_limit(axis,0,1)){
			move_half_cell(&(axis->y_stepper));
	}

	axis->current_position.row = ORIGIN;
	axis->current_position.column = ORIGIN;
}

void axis_manager_move(Axis_manager* axis,int start_row,int start_column,int end_row,int end_column){
	int drow,dcol;

	// Move the steppers to starting position
	drow = start_row - axis->current_position.row;
	dcol = start_column - axis->current_position.column;

	if(drow < 0){
		drow = -drow;
		setDirection(&(axis->x_stepper),0);
	}
	else setDirection(&(axis->x_stepper),1);

	if(dcol < 0){
			dcol = -dcol;
			setDirection(&(axis->y_stepper),0);
	}
	else setDirection(&(axis->y_stepper),1);

	move_n_cells(&(axis->x_stepper), drow);
	move_n_cells(&(axis->y_stepper), dcol);

	// Active hook
	SERVO_HOOK_on(&(axis->hook));

	// Move on the boundary
	setDirection(&(axis->x_stepper),1);
	move_half_cell(&(axis->x_stepper));

	// Move the steppers to the end position
	drow = end_row - start_row;
	dcol = end_column - start_column;

	if(drow < 0){
			drow = -drow;
			setDirection(&(axis->x_stepper),0);
	}
	else setDirection(&(axis->x_stepper),1);

	if(dcol < 0){
				dcol = -dcol;
				setDirection(&(axis->y_stepper),0);
	}
	else setDirection(&(axis->y_stepper),1);

	move_n_cells(&(axis->x_stepper), drow);
	move_n_cells(&(axis->y_stepper), dcol);

	// Move on the cell
	setDirection(&(axis->x_stepper),0);
	move_half_cell(&(axis->x_stepper));

	// Set current position
	axis->current_position.row = end_row;
	axis->current_position.column = end_column;

	// Disable Hook
	SERVO_HOOK_off(&(axis->hook));
}

int axis_manager_check_limit(Axis_manager* axis,int x,int back){
	if(back){
		return (x == 1 && HAL_GPIO_ReadPin(GPIOA,axis->x_back) == GPIO_PIN_SET) || (x == 0 && HAL_GPIO_ReadPin(GPIOA,axis->y_back) == GPIO_PIN_SET) ; // da riverdere
	}
	else{
		return (x == 1 && HAL_GPIO_ReadPin(GPIOA,axis->x_front) == GPIO_PIN_SET) || (x == 0 && HAL_GPIO_ReadPin(GPIOA,axis->y_front) == GPIO_PIN_SET) ;
	}
}

void axis_manager_destoy(Axis_manager* axis){
	free(axis);
}
