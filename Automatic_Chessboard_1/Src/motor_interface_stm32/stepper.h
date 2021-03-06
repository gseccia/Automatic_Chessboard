/*
 * stepper.h
 *
 *  Created on: 06 giu 2019
 *      Author: Giuseppe
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Drives a bipolar stepper motor.
 *
 * When wiring multiple stepper motors to a microcontroller, you quickly run
 * out of output pins, with each motor requiring 4 connections.
 *
 *
 *
 * The sequence of control signals for 4 control wires is as follows:
 *
 * Step A+ A- B+ B-
 *    1  1  0  1  0
 *    2  0  1  1  0
 *    3  0  1  0  1
 *    4  1  0  0  1
 *
 */

#ifndef STEPPER_H_
#define STEPPER_H_

#include "gpio.h"

/**
  * @brief This enumeration represent the directions of movement of the motor.
  */

typedef enum {
	FORWARD  		= 1,
	BACKWARD        = -1,
} Step_direction;

/**
  * @brief The struct �Stepper� represent the step motor.
  * To control it we need to know which are the two  GPIO pins of the Board that allow us to set the direction and make a step;
  * the step delay is used to set the delay between two successive steps, so the speed of the motor.
  */

typedef struct Stepper{
	GPIO_TypeDef* group_pin_direction;
	uint16_t pin_direction;
	GPIO_TypeDef* group_pin_step;
	uint16_t pin_step;
	uint32_t step_delay;
	int x;
}Stepper;

//Initializer
Stepper stepper_init(GPIO_TypeDef* group_pin_direction, uint16_t pin_direction, GPIO_TypeDef* group_pin_step, uint16_t pin_step, uint32_t step_delay,int x);


// direction setter methood:
void setDirection(Stepper* stepper,Step_direction new_direction);

// motion metods:
void move_n_steps(Stepper* stepper, int number_of_steps, Step_direction direction);

void move_half_cell(Stepper* stepper, Step_direction direction);

void move_full_cell(Stepper* stepper,Step_direction direction);

void move_n_cells(Stepper* stepper, int num_cell, Step_direction direction);

#endif /* STEPPER_H_ */
