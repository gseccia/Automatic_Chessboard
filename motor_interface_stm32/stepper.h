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

/**
 * THIS LIBRARY WILL CONSIDER
 * IN1 -> PA0
 * IN2 -> PA1
 * IN3 -> PA4
 * IN4 -> PB0
 *
 * change
 */

typedef struct Stepper{
	int direction;
	double step_delay;
	int number_of_steps; // total number of steps
	int step_number;          // which step the motor is on
}Stepper;

//Initializer
Stepper stepper_init();

// speed setter method: velocità accettabili sull'ordine delle migliaia (1000-5000)
void setSpeed(Stepper* stepper, int whatSpeed);

// direction setter methood:
void setDirection(Stepper* stepper,int new_direction);

// mover method:
void step(int thisStep);

void move_n_steps(Stepper* stepper, int number_of_steps);

void move_half_cell(Stepper* stepper);

void move_full_cell(Stepper* stepper);

void move_n_cells(Stepper* stepper, int num_cell);

#endif /* STEPPER_H_ */
