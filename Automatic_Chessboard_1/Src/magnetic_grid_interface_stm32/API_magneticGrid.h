/*
 * API_magneticGrid.h
 *
 *  Created on: 08 giu 2019
 *      Author: Gerry
 */

#ifndef SRC_API_MAGNETICGRID_H_
#define SRC_API_MAGNETICGRID_H_


#include "../chess_engine/Chess.h"
#include "gpio.h"
#include "usart.h"
#include <stdlib.h>

/**
  * @brief It contains two mapping matrix of the chess-men on the chess-board, one for the current state and one for the previous state
  */

typedef struct magnetic_grid_manager{
	uint8_t magnetic_grid[8][8];
	uint8_t old_magnetic_grid[8][8];
	move* move_list;
} magnetic_grid_manager;


/** @brief  It creates a �magnetic_grid_manager� instance and initialize all the element of the matrix �magnetic_grid� to zero.
  * @retval The pointer to the magnetic_grid_manager built
  */

magnetic_grid_manager* init_magnetic_grid();

/** @brief  fetch the magnetic grid and build a mapping-matrix of the chess-men on the chess-board
  * @param  the magnetic_grid_manager that manages the mapping matrix of the chess-men on the chess-board
  * @param  is used as a boolean, if his value is 0 this method perform a simple fetch od the grid, otherwise fetch the grid and capture a variation
  * @retval number of variation in the current state respect the previous one
  */

int read_magnetic_grid(magnetic_grid_manager* magnetic_grid_manager,int variation);

/** @brief  It update magnetic_grid_manager putting the current state of the magnetic grid in �Magnetic_grid�, but first bring the content of �magnetic_grid� in �old_magnetic_grid� to preserve the value of the previous state.
  * @param  the the magnetic_grid_manager that manages the mapping matrix of the chess-men on the chess-board
  * @retval None
  */

void update_magnetic_grid(magnetic_grid_manager* magnetic_grid_manager);

/** @brief  It reset magnetic_grid_manager
  * @param  the the magnetic_grid_manager that manages the mapping matrix of the chess-men on the chess-board
  * @retval None
  */

void reset_magnetic_grid(magnetic_grid_manager* magnetic_manager);

/** @brief  It extract the move done by the user looking at the differences between the current state and the old state of the magnetic grid
  * @param  the the magnetic_grid_manager that manages the mapping matrix of the chess-men on the chess-board
  * @retval The move done by the user
  */

move* fetch_moves(magnetic_grid_manager* magnetic_grid_manager);

int check_restoring(magnetic_grid_manager* magnetic_grid_manager);

#endif /* SRC_API_MAGNETICGRID_H_ */
