#include "API_magneticGrid.h"

typedef struct magnetic_grid_manager magnetic_grid_manager;

magnetic_grid_manager* init_magnetic_grid(){
	struct magnetic_grid_manager* magnetic_manager = (magnetic_grid_manager*)malloc(sizeof(magnetic_grid_manager));
	int i,j;
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			(magnetic_manager->magnetic_grid)[i][j] = 0;
		}
	}
	update_magnetic_grid(magnetic_manager);
	return magnetic_manager;
}

/** @brief  fetch the magnetic grid and build a mapping-matrix of the chess-men on the chess-board
  * @param  the matrix to fill
  * @retval None
  */
void update_magnetic_grid(magnetic_grid_manager* magnetic_grid_manager){

	int i,j;
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			(magnetic_grid_manager->old_magnetic_grid)[i][j] = (magnetic_grid_manager->magnetic_grid)[i][j];
		}
	}

	uint16_t pins[8] = {GPIO_PIN_8,GPIO_PIN_10,GPIO_PIN_4,GPIO_PIN_5,GPIO_PIN_3,GPIO_PIN_10,GPIO_PIN_4,GPIO_PIN_13};
	GPIO_TypeDef* str_pins[8] = {GPIOA,GPIOB,GPIOB,GPIOB,GPIOB,GPIOA,GPIOC,GPIOB};

	for(i=0;i<8;i++){
		HAL_GPIO_WritePin(str_pins[i], pins[i], SET);
		HAL_Delay(100);
					  //now i read each row

		(magnetic_grid_manager->magnetic_grid)[0][0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);  // row 1 - black
		(magnetic_grid_manager->magnetic_grid)[1][0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);  // row 2 - white
		(magnetic_grid_manager->magnetic_grid)[2][0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);  // row 3 - gray
		(magnetic_grid_manager->magnetic_grid)[3][0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);  // row 4 - violet
		(magnetic_grid_manager->magnetic_grid)[4][0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);  // row 5- blue
		(magnetic_grid_manager->magnetic_grid)[5][0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);  // row 6- green
		(magnetic_grid_manager->magnetic_grid)[6][0] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);  // row 7- yellow
		(magnetic_grid_manager->magnetic_grid)[7][0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);  // row 8 - orange

		HAL_Delay(100);
	    HAL_GPIO_WritePin(str_pins[i], pins[i], RESET);
	}
}

// Da rivedere per rilevare più mosse.
move* fetch_moves(magnetic_grid_manager* magnetic_grid_manager){
	int i,j;

	move* l;

	int len = 0;

	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			if((magnetic_grid_manager->magnetic_grid)[i][j] != (magnetic_grid_manager->old_magnetic_grid)[i][j]){

				if (len == 0) {
					l = create_move(0,0,0,0);
					len++;
				}

				if((magnetic_grid_manager->magnetic_grid)[i][j] - (magnetic_grid_manager->old_magnetic_grid)[i][j] > 0){
					l->to->row = i;
					l->to->column = j;
				}
				else{
					l->from->row = i;
					l->from->column = j;
				}
			}
		}
	}

	return l;
}




/**
 * 	// column 1 activation
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, SET);
	HAL_Delay(100);
		  //now i read each row

		  (magnetic_grid_manager->magnetic_grid)[0][0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);  // row 1 - black
		  (magnetic_grid_manager->magnetic_grid)[1][0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);  // row 2 - white
		  (magnetic_grid_manager->magnetic_grid)[2][0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);  // row 3 - gray
		  (magnetic_grid_manager->magnetic_grid)[3][0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);  // row 4 - violet
		  (magnetic_grid_manager->magnetic_grid)[4][0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);  // row 5- blue
		  (magnetic_grid_manager->magnetic_grid)[5][0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);  // row 6- green
		  (magnetic_grid_manager->magnetic_grid)[6][0] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);  // row 7- yellow
		  (magnetic_grid_manager->magnetic_grid)[7][0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);  // row 8 - orange

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
 */
