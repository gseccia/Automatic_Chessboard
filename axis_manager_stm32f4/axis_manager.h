#ifndef AXIS_MANAGER_
#define AXIS_MANAGER_

#include "Stepper.h";
#include "hook.h";
#include "gpio.h";

struct cell{
	int row;
	int column;
};
typedef struct cell cell;

struct Axis_manager{
	struct Stepper x_stepper;
	struct Stepper y_stepper;
	struct SERVO_HOOK hook;
	cell current_position;

	uint16_t x_back,x_front;
	uint16_t y_back,y_front;
};
typedef struct Axis_manager Axis_manager;

#define UNKNOWN -2
#define ORIGIN -1

/**
 * This fuction creates a new axis manager.
 *  front_x, back_x, front_y, back_y are the PIN to read the limits.
 */
Axis_manager* axis_manager_init(TIM_HandleTypeDef *tim_pwm_handler,int frequency,uint16_t front_x,uint16_t back_x,uint16_t front_y,uint16_t back_y );

/**
 * This function resets the current logical and hardware position of Steppers to the Origin.
 */
void axis_manager_reset_position(Axis_manager* axis);

/**
 * This function moves the steppers from current position to (row,column) cell
 */
void axis_manager_move(Axis_manager* axis,int row,int column);

/**
 * This function return 1 if the axis is arrived to the limit (x limit x parameter 1, y limit otherwise)
 */
int axis_manager_check_limit(Axis_manager* axis,int x,int back);

#endif
