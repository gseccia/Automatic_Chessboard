
#include "gpio.h"
#include "API_magneticGrid.h"


/** @brief  fetch the magnetic grid and build a mapping-matrix of the chess-men on the chess-board
  * @param  the matrix to fill
  * @retval None
  */
void fetch_magnetic_grid(uint8_t* magnetic_grid){
	  // column 1 activation


	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, SET);
	  HAL_Delay(100);
	  //now i read each row

	  magnetic_grid[0][0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);  // row 1 - black
	  magnetic_grid[1][0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);  // row 2 - white
	  magnetic_grid[2][0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);  // row 3 - gray
	  magnetic_grid[3][0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);  // row 4 - violet
	  magnetic_grid[4][0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);  // row 5- blue
	  magnetic_grid[5][0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);  // row 6- green
	  magnetic_grid[6][0] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);  // row 7- yellow
	  magnetic_grid[7][0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);  // row 8 - orange

	  HAL_Delay(100);
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, RESET);

	  //--------------------------------------------------------------------------

	  // column 2 activation
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, SET);
	  HAL_Delay(100);
	  //now i read each row

	  magnetic_grid[0][1] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);  // row 1 - black
	  magnetic_grid[1][1] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);  // row 2 - white
	  magnetic_grid[2][1] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);  // row 3 - gray
	  magnetic_grid[3][1] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);  // row 4 - violet
	  magnetic_grid[4][1] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);  // row 5- blue
	  magnetic_grid[5][1] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);  // row 6- green
	  magnetic_grid[6][1] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);  // row 7- yellow
	  magnetic_grid[7][1] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);  // row 8 - orange

	  HAL_Delay(100);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, RESET);

	  //--------------------------------------------------------------------------

	  // column 3 activation
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, SET);
	  HAL_Delay(100);
	  //now i read each row

	  magnetic_grid[0][2] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);  // row 1 - black
	  magnetic_grid[1][2] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);  // row 2 - white
	  magnetic_grid[2][2] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);  // row 3 - gray
	  magnetic_grid[3][2] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);  // row 4 - violet
	  magnetic_grid[4][2] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);  // row 5- blue
	  magnetic_grid[5][2] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);  // row 6- green
	  magnetic_grid[6][2] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);  // row 7- yellow
	  magnetic_grid[7][2] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);  // row 8 - orange

	  HAL_Delay(100);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, RESET);

	  //--------------------------------------------------------------------------

	  // column 4 activation
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, SET);
	  HAL_Delay(100);
	  //now i read each row

	  magnetic_grid[0][3] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);  // row 1 - black
	  magnetic_grid[1][3] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);  // row 2 - white
	  magnetic_grid[2][3] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);  // row 3 - gray
	  magnetic_grid[3][3] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);  // row 4 - violet
	  magnetic_grid[4][3] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);  // row 5- blue
	  magnetic_grid[5][3] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);  // row 6- green
	  magnetic_grid[6][3] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);  // row 7- yellow
	  magnetic_grid[7][3] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);  // row 8 - orange

	  HAL_Delay(100);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, RESET);

	  //--------------------------------------------------------------------------

	  // column 5 activation
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, SET);
	  HAL_Delay(100);
	  //now i read each row

	  magnetic_grid[0][4] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);  // row 1 - black
	  magnetic_grid[1][4] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);  // row 2 - white
	  magnetic_grid[2][4] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);  // row 3 - gray
	  magnetic_grid[3][4] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);  // row 4 - violet
	  magnetic_grid[4][4] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);  // row 5- blue
	  magnetic_grid[5][4] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);  // row 6- green
	  magnetic_grid[6][4] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);  // row 7- yellow
	  magnetic_grid[7][4] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);  // row 8 - orange

	  HAL_Delay(100);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, RESET);

	  //--------------------------------------------------------------------------

	  // column 6 activation
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, SET);
	  HAL_Delay(100);
	  //now i read each row

	  magnetic_grid[0][5] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);  // row 1 - black
	  magnetic_grid[1][5] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);  // row 2 - white
	  magnetic_grid[2][5] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);  // row 3 - gray
	  magnetic_grid[3][5] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);  // row 4 - violet
	  magnetic_grid[4][5] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);  // row 5- blue
	  magnetic_grid[5][5] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);  // row 6- green
	  magnetic_grid[6][5] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);  // row 7- yellow
	  magnetic_grid[7][5] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);  // row 8 - orange

	  HAL_Delay(100);
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, RESET);

	  //--------------------------------------------------------------------------

	  // column 7 activation
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, SET);
	  HAL_Delay(100);
	  //now i read each row

	  magnetic_grid[0][6] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);  // row 1 - black
	  magnetic_grid[1][6] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);  // row 2 - white
	  magnetic_grid[2][6] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);  // row 3 - gray
	  magnetic_grid[3][6] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);  // row 4 - violet
	  magnetic_grid[4][6] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);  // row 5- blue
	  magnetic_grid[5][6] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);  // row 6- green
	  magnetic_grid[6][6] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);  // row 7- yellow
	  magnetic_grid[7][6] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);  // row 8 - orange

	  HAL_Delay(100);
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, RESET);

	  //--------------------------------------------------------------------------

	  // column 8 activation
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, SET);
	  HAL_Delay(100);
	  //now i read each row

	  magnetic_grid[0][7] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);  // row 1 - black
	  magnetic_grid[1][7] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);  // row 2 - white
	  magnetic_grid[2][7] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);  // row 3 - gray
	  magnetic_grid[3][7] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);  // row 4 - violet
	  magnetic_grid[4][7] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);  // row 5- blue
	  magnetic_grid[5][7] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);  // row 6- green
	  magnetic_grid[6][7] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);  // row 7- yellow
	  magnetic_grid[7][7] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);  // row 8 - orange

	  HAL_Delay(100);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, RESET);

}
