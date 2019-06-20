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

/**
  * @brief The “SERVO_HOOK” struct contains the TIM (timer) used to handle the PWM and two int value, “ccr_on” and “ccr_off”, to set the position of the motor.
  */

typedef struct SERVO_HOOK{
	TIM_HandleTypeDef *tim_pwm_handler;
	int ccr_on;
	int ccr_off;

}SERVO_HOOK;

/**@brief It creates a “SERVO_HOOK” instance taking 2 parameters: the timer used to handle the PWM and the control frequency.
 *@param the timer used to handle the PWM
 *@param the control frequency
 *@retval the SERVO_HOOK instance created
 */
SERVO_HOOK SERVO_HOOK_Init(TIM_HandleTypeDef *tim_pwm_handler,int frequency);

/**@brief This method set the position of the servo-motor to “on”, so move it up.
 *@param the instace of the servo-motor
 *@retval None
 */
void SERVO_HOOK_on(SERVO_HOOK *hook);

/**@brief This method set the position of the servo motor to “off”, so move it down.
 *@param the instace of the servo-motor
 *@retval None
 */
void SERVO_HOOK_off(SERVO_HOOK *hook);
