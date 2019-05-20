/*
 * hook.h
 *
 *  Created on: 18 mag 2019
 *      Author: Giuseppe
 */

#ifndef HOOK_H_
#define HOOK_H_



#endif /* HOOK_H_ */

#include "tim.h"

typedef struct SERVO_HOOK{
	TIM_HandleTypeDef *tim_pwm_handler;
	int ccr_on;
	int ccr_off;

}SERVO_HOOK;

/**
 *
 */
SERVO_HOOK SERVO_HOOK_Init(TIM_HandleTypeDef *tim_pwm_handler,int frequency);

/**
 *
 */
void SERVO_HOOK_on(SERVO_HOOK *hook);

/**
 *
 */
void SERVO_HOOK_off(SERVO_HOOK *hook);
