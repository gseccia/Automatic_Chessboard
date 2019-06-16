#include "API_magneticGrid.h"

typedef struct magnetic_grid_manager magnetic_grid_manager;
extern int PLAYER_WHITE;
extern char board[BOARD_SIZE][BOARD_SIZE];

uint16_t out_pins[8] = {GPIO_PIN_8,GPIO_PIN_10,GPIO_PIN_4,GPIO_PIN_5,GPIO_PIN_3,GPIO_PIN_10,GPIO_PIN_4,GPIO_PIN_13};
GPIO_TypeDef* out_str_pins[8] = {GPIOA,GPIOB,GPIOB,GPIOB,GPIOB,GPIOA,GPIOC,GPIOB};

uint16_t in_pins[8] = {GPIO_PIN_8,GPIO_PIN_9,GPIO_PIN_5,GPIO_PIN_6,GPIO_PIN_7,GPIO_PIN_6,GPIO_PIN_7,GPIO_PIN_9};
GPIO_TypeDef* in_str_pins[8] = {GPIOB,GPIOB,GPIOA,GPIOA,GPIOA,GPIOB,GPIOC,GPIOA};


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

int check_restoring(magnetic_grid_manager* magnetic_grid_manager){
	int i,j;
	int not_restored;
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
				not_restored = ((magnetic_grid_manager->old_magnetic_grid)[i][j] != (magnetic_grid_manager->magnetic_grid)[i][j]);
				if(not_restored)return 0;
		}
	}
	return 1;
}

void update_magnetic_grid(magnetic_grid_manager* magnetic_grid_manager){

	int i,j;
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			(magnetic_grid_manager->old_magnetic_grid)[i][j] = (magnetic_grid_manager->magnetic_grid)[i][j];
		}
	}
}


int read_magnetic_grid(magnetic_grid_manager* magnetic_grid_manager){

	int i,j,tmp,assigned,counter=0;
	move* curr;
	/**TMP PIN**/
	int sim[8][8];
	for(i=0;i<8;i++)
		for(j=0;j<8;j++)
				sim[i][j] = (magnetic_grid_manager->magnetic_grid)[i][j];
	/***********/

	for(i=0;i<8;i++){

		HAL_GPIO_WritePin(out_str_pins[i], out_pins[i], GPIO_PIN_SET);
		//HAL_Delay(5);

		for(j = 0;j<8;j++){
			// Get new cell state
			tmp = (HAL_GPIO_ReadPin(in_str_pins[j], in_pins[j]) == GPIO_PIN_SET)? 1:0;
			tmp = sim[j][i];

			//  Check variation
			if((magnetic_grid_manager->magnetic_grid)[j][i] != tmp){

				// Piece added!
				if((magnetic_grid_manager->magnetic_grid)[j][i] - tmp > 0){
					curr = magnetic_grid_manager->move_list;
					assigned = 0;

					while(curr != NULL && !assigned)
					{
						if ( curr->to->column == -1 && curr->to->row == -1 ){
							// It's my piece

							curr->to->column = i;
							curr->to->row = j;

							assigned = 1;
						}
						curr = curr->next;
					}

					if(!assigned){
						// ERROR no piece miss
						counter = 2;
					}
				}
				else{

					// Piece miss!

					if(magnetic_grid_manager->move_list == NULL) {

						if((IS_WHITE(board[j][i]) && !PLAYER_WHITE) || (PLAYER_WHITE && IS_BLACK(board[j][i]))){
							magnetic_grid_manager->move_list = create_move(j,i,10,10); // Enemy piece selected(OUT board)
						}
						else magnetic_grid_manager->move_list = create_move(j,i,-1,-1);// My Piece selected
					}
					else{
						curr = magnetic_grid_manager->move_list;
						while(curr->next != NULL) curr = curr->next;

						if((IS_WHITE(board[j][i]) && !PLAYER_WHITE) || (PLAYER_WHITE && IS_BLACK(board[j][i])))
							magnetic_grid_manager->move_list->next = create_move(j,i,10,10); // Enemy Piece out of chessboard
						else{
							magnetic_grid_manager->move_list->next = create_move(j,i,-1,-1);
						}
					}
				}

				// Store new possible cell state
				(magnetic_grid_manager->magnetic_grid)[j][i] = tmp;
				// Add a variation!
				counter++;
			}
		}

		//HAL_Delay(5);
		HAL_GPIO_WritePin(out_str_pins[i], out_pins[i], GPIO_PIN_RESET);
	}
	return counter;
}


move* fetch_moves(magnetic_grid_manager* magnetic_grid_manager){
	int i,j;

	move *curr,*prev;
	int count = 0;

	curr = magnetic_grid_manager->move_list;
	prev = NULL;

	// Deletion "No movement!"
	while(curr->next != NULL){
		if(curr->from->column == curr->to->column && curr->from->row == curr->to->row){
			if(prev == NULL){
				prev = magnetic_grid_manager->move_list->next;
				free_location(curr->from);
				free_location(curr->to);
				free(curr);
				curr = prev;
				prev = NULL;
			}
			else {
				prev->next = curr->next;
				free_location(curr->from);
				free_location(curr->to);
				free(curr);
				curr = prev->next;
			}
		}
		else count++;
	}


	if(count == 1){// Simple movement
		return magnetic_grid_manager->move_list;
	}
	else if(count == 2){// Take a piece o Arrocco
		move *first,*second;
		first=magnetic_grid_manager->move_list;
		second=magnetic_grid_manager->move_list->next;

		/*if((first->to->column != 10 && first->to->row != 10) && (second->to->column != 10 && second->to->row != 10) ){
			// ARROCCO
		}*/
		if(first->to->column == 10 && first->to->row == 10){
			// Possible Take a piece
			second->to->column = first->from->column;
			second->to->row = first->from->row;

			free_location(first->from);
			free_location(first->to);
			free(first);
			magnetic_grid_manager->move_list = second;
			second->next = NULL;
		}
		else if(second->to->column == 10 && second->to->row == 10){
			// Possible Take a piece
			first->to->column = second->from->column;
			first->to->row = second->from->row;

			free_location(second->from);
			free_location(second->to);
			free(second);
			magnetic_grid_manager->move_list = first;
			first->next = NULL;
		}


		return magnetic_grid_manager->move_list;
	}
	else{
		free_move(magnetic_grid_manager->move_list);
		return NULL; // No possible movement with more of 2 moves
	}


	/*move* l = NULL;

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
	}*/
}
