#ifndef AXIS_MANAGER_
#define AXIS_MANAGER_

#include "../motor_interface_stm32/Stepper.h"
#include "../hook_interface_stm32f4/hook.h"
#include "gpio.h"

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

	GPIO_TypeDef* xgroup_pin;
	GPIO_TypeDef* ygroup_pin;
	uint16_t xpin;
	uint16_t ypin;
};
typedef struct Axis_manager Axis_manager;

#define UNKNOWN -1
#define ORIGIN 0
#define OUT_CHESSBOARD 8

/**
 * This fuction creates a new axis manager.
 *  front_x, back_x, front_y, back_y are the PIN to read the limits.
 */
Axis_manager* axis_manager_init(TIM_HandleTypeDef *tim_pwm_handler,int frequency,GPIO_TypeDef* motorAgroup_dir,uint16_t motorAPin_dir,GPIO_TypeDef* motorAgroup_step,uint16_t motorAPin_step,uint32_t motorA_delay,GPIO_TypeDef* motorBgroup_dir,uint16_t motorBPin_dir,GPIO_TypeDef* motorBgroup_step,uint16_t motorBPin_step,uint32_t motorB_delay,GPIO_TypeDef* xgroup,GPIO_TypeDef* ygroup,uint16_t xpin,uint16_t ypin);

/**
 * This function resets the current logical and hardware position of Steppers to the Origin.
 */
void axis_manager_reset_position(Axis_manager* axis);

/**
 * This function moves the steppers from current position to (row,column) cell
 */
void axis_manager_move(Axis_manager* axis,int start_row,int start_column,int end_row,int end_column);

/**
 * This function return 1 if the axis is arrived to the limit (x limit x parameter 1, y limit otherwise)
 */
int axis_manager_check_limit(Axis_manager* axis,int x);

/**
 * This function destoy the object
 */
void axis_manager_destoy(Axis_manager* axis);

#endif
