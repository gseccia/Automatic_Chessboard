/*
 * axis_manager.c
 *
 *  Created on: 07 giu 2019
 *      Author: Antonio
 */
#include "axis_manager.h"

typedef struct Axis_manager Axis_manager;


Axis_manager* axis_manager_init(TIM_HandleTypeDef *tim_pwm_handler,int frequency,GPIO_TypeDef* motorAgroup_dir,uint16_t motorAPin_dir,GPIO_TypeDef* motorAgroup_step,uint16_t motorAPin_step,uint32_t motorA_delay,GPIO_TypeDef* motorBgroup_dir,uint16_t motorBPin_dir,GPIO_TypeDef* motorBgroup_step,uint16_t motorBPin_step,uint32_t motorB_delay,GPIO_TypeDef* xgroup,GPIO_TypeDef* ygroup,uint16_t xpin,uint16_t ypin){
	Axis_manager* axis_manager = (Axis_manager*)malloc(sizeof(Axis_manager));

	// Initialize logic position
	struct cell current_position;
	current_position.row = UNKNOWN;
	current_position.column = UNKNOWN;
	axis_manager->current_position = current_position;

	// Initialize Steppers and servo
	axis_manager->x_stepper = stepper_init(motorAgroup_dir,motorAPin_dir,motorAgroup_step,motorAPin_step,motorA_delay,1);
	axis_manager->y_stepper = stepper_init(motorBgroup_dir,motorBPin_dir,motorBgroup_step,motorBPin_step,motorB_delay,0);
	axis_manager->hook = SERVO_HOOK_Init(tim_pwm_handler,frequency);

	// Initialize limit PIN
	axis_manager->xpin = xpin;
	axis_manager->xgroup_pin = xgroup;
	axis_manager->ypin = ypin;
	axis_manager->ygroup_pin = ygroup;

	return axis_manager;
}


void axis_manager_reset_position(Axis_manager* axis){
	SERVO_HOOK_off(&(axis->hook));

	/*while(!axis_manager_check_limit(axis,1) && !axis_manager_check_limit(axis,0)){
		move_half_cell(&(axis->x_stepper),FORWARD);
		move_half_cell(&(axis->y_stepper),FORWARD);
	}*/


	int readx = HAL_GPIO_ReadPin(axis->xgroup_pin,axis->xpin);
	while(readx == GPIO_PIN_RESET){
			move_n_steps(&(axis->x_stepper),1,FORWARD);
			readx = HAL_GPIO_ReadPin(axis->xgroup_pin,axis->xpin);
	}


	int ready = HAL_GPIO_ReadPin(axis->ygroup_pin,axis->ypin);
			while(ready == GPIO_PIN_RESET){
				move_n_steps(&(axis->y_stepper),1,FORWARD);
				ready = HAL_GPIO_ReadPin(axis->ygroup_pin,axis->ypin);
		}

	move_n_steps(&axis->x_stepper,200,BACKWARD);
	move_n_steps(&axis->y_stepper,405,BACKWARD);

	axis->current_position.row = 0;
	axis->current_position.column = 0;

}

void axis_manager_move(Axis_manager* axis,int start_row,int start_column,int end_row,int end_column){
	int drow,dcol;
	Step_direction dirx,diry;

	// Move the steppers to starting position
	drow = start_row - axis->current_position.row;
	dcol = start_column - axis->current_position.column;

	if(drow < 0){
		drow = -drow;
		dirx = BACKWARD;
	}
	else dirx = FORWARD;

	if(dcol < 0){
			dcol = -dcol;
			diry = BACKWARD;
	}
	else diry = FORWARD;

	move_n_cells(&(axis->x_stepper), drow,dirx);
	move_n_cells(&(axis->y_stepper), dcol,diry);

	// Active hook
	SERVO_HOOK_on(&(axis->hook));

	// Move on the boundary
	move_half_cell(&(axis->x_stepper),FORWARD);

	// Move the steppers to the end position
	drow = end_row - start_row;
	dcol = end_column - start_column;

	if(drow < 0){
			drow = -drow;
			dirx = BACKWARD;
	}
	else dirx = FORWARD;

	if(dcol < 0){
				dcol = -dcol;
				diry = BACKWARD;
	}
	else diry = FORWARD;

	move_n_cells(&(axis->x_stepper), drow,dirx);
	move_n_cells(&(axis->y_stepper), dcol,diry);

	// Move on the cell
	move_half_cell(&(axis->x_stepper),BACKWARD);

	// Set current position
	axis->current_position.row = end_row;
	axis->current_position.column = end_column;

	// Disable Hook
	SERVO_HOOK_off(&(axis->hook));
}

int axis_manager_check_limit(Axis_manager* axis,int x){
		int ready = HAL_GPIO_ReadPin(axis->ygroup_pin,axis->ypin);
		int readx = HAL_GPIO_ReadPin(axis->xgroup_pin,axis->xpin);
		return (x == 1 &&  readx == GPIO_PIN_SET) || (x == 0 && ready == GPIO_PIN_SET) ;
}

void axis_manager_destoy(Axis_manager* axis){
	free(axis);
}
