/*
 * hook.c
 *
 *  Created on: 18 mag 2019
 *      Author: Giuseppe
 */

#include "hook.h"
#include "tim.h"

/**
 *
 */

/**
 *
 */
SERVO_HOOK SERVO_HOOK_Init(TIM_HandleTypeDef *tim_pwm_handler,int frequency){
	SERVO_HOOK servo ={
			.tim_pwm_handler=tim_pwm_handler,
			.ccr_off=40,
			.ccr_on=80
	};
	//sostituire con calcolo specifico basato sulla frequenza del pwm
	return servo;
}

/**
 *
 */
void SERVO_HOOK_on(SERVO_HOOK *hook){
	hook->tim_pwm_handler->Instance->CCR1 = hook->ccr_on;
	return;
}

/**
 *
 */
void SERVO_HOOK_off(SERVO_HOOK *hook){
	hook->tim_pwm_handler->Instance->CCR1 = hook->ccr_off;
	return;
}
