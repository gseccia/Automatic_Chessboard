/*
 * API_magneticGrid.h
 *
 *  Created on: 08 giu 2019
 *      Author: Gerry
 */

#ifndef SRC_API_MAGNETICGRID_H_
#define SRC_API_MAGNETICGRID_H_

/** @brief  fetch the magnetic grid and build a mapping-matrix of the chess-men on the chess-board
  * @param  the matrix to fill
  * @retval None
  */
#include "Chess.h"
#include "gpio.h"
#include <stdlib.h>

typedef struct magnetic_grid_manager{
	uint8_t magnetic_grid[8][8];
	uint8_t old_magnetic_grid[8][8];
} magnetic_grid_manager;


magnetic_grid_manager* init_magnetic_grid();
void update_magnetic_grid(magnetic_grid_manager* magnetic_grid_manager);
move* fetch_moves(magnetic_grid_manager* magnetic_grid_manager);


#endif /* SRC_API_MAGNETICGRID_H_ */
