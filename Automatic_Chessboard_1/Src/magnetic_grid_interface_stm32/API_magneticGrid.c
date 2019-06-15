#include "API_magneticGrid.h"

typedef struct magnetic_grid_manager magnetic_grid_manager;


magnetic_grid_manager* init_magnetic_grid(){
	struct magnetic_grid_manager* magnetic_manager = (magnetic_grid_manager*)malloc(sizeof(magnetic_grid_manager));
	int i,j;
	/* read_magnetic_grid */
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			(magnetic_manager->magnetic_grid)[i][j] = (i<=1 || i>=7)? 1:0;
		}
	}
	update_magnetic_grid(magnetic_manager);
	return magnetic_manager;
}

void reset_magnetic_grid(magnetic_grid_manager* magnetic_manager){
	int i,j;
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			(magnetic_manager->magnetic_grid)[i][j] = 0;
		}
	}
	update_magnetic_grid(magnetic_manager);
}

void update_magnetic_grid(magnetic_grid_manager* magnetic_grid_manager){

	int i,j;
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			(magnetic_grid_manager->old_magnetic_grid)[i][j] = (magnetic_grid_manager->magnetic_grid)[i][j];
		}
	}
}

/** @brief  fetch the magnetic grid and build a mapping-matrix of the chess-men on the chess-board
  * @param  the matrix to fill
  * @retval None
  */
void read_magnetic_grid(magnetic_grid_manager* magnetic_grid_manager){

	int i,j;

	uint16_t pins[8] = {GPIO_PIN_8,GPIO_PIN_10,GPIO_PIN_4,GPIO_PIN_5,GPIO_PIN_3,GPIO_PIN_10,GPIO_PIN_4,GPIO_PIN_13};
	GPIO_TypeDef* str_pins[8] = {GPIOA,GPIOB,GPIOB,GPIOB,GPIOB,GPIOA,GPIOC,GPIOB};


	for(i=0;i<8;i++){

		HAL_GPIO_WritePin(str_pins[i], pins[i], SET);
		HAL_Delay(50);
					  //now i read each row

		(magnetic_grid_manager->magnetic_grid)[0][i] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);  // row 1 - black
		(magnetic_grid_manager->magnetic_grid)[1][i] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);  // row 2 - white
		(magnetic_grid_manager->magnetic_grid)[2][i] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);  // row 3 - gray
		(magnetic_grid_manager->magnetic_grid)[3][i] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);  // row 4 - violet
		(magnetic_grid_manager->magnetic_grid)[4][i] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);  // row 5- blue
		(magnetic_grid_manager->magnetic_grid)[5][i] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);  // row 6- green
		(magnetic_grid_manager->magnetic_grid)[6][i] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);  // row 7- yellow
		(magnetic_grid_manager->magnetic_grid)[7][i] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);  // row 8 - orange

		HAL_Delay(50);
	    HAL_GPIO_WritePin(str_pins[i], pins[i], RESET);

	}
}

// Da rifare... Non funzionerà mai. C'è aliasing
move* fetch_moves(magnetic_grid_manager* magnetic_grid_manager){
	int i,j;

	move* l = NULL;

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
