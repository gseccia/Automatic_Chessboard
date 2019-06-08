#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "Chess.h"

// Genral settings
extern int MINIMAX_DEPTH;
extern int GAME;
extern int SETTINGS;
extern int GUI_MODE; // '0' - application runs in 'Console mode', '1' - 'Gui mode'
extern int TWO_PLAYERS_MODE; // '1' - two players mode, '0' - player vs. AI mode
extern int WHITE_TURN; // 0 - black, 1- white
extern int PLAYER_WHITE;
extern char board[BOARD_SIZE][BOARD_SIZE];
extern int GAME_STATUS;

// Game setiings
extern int CHECK_ON_WHITE;
extern int CHECK_ON_BLACK;
extern int board_cnt;



//						*************** minimax ***************

/** returns an int representing the score for the previously
  * executed move (that caused the a_board configuration),
  * according to the scoring function and depending on the depth.
  * this is a recursive function. */
int minmax(char a_board[BOARD_SIZE][BOARD_SIZE], int minim, int next_color, int depth, int alpha, int beta)
{	
	int true_color =  (minim) ? !next_color : next_color;
	move *moves = NULL;
	move *temp = NULL;
	int mini_score = INF_SCORE;
	int maxi_score = -INF_SCORE;
	// recursion ends
	if ( depth == MINIMAX_DEPTH )	return score_board(a_board, true_color);
	if ( board_cnt > TOO_MUCH ) return score_board(a_board, true_color); // (just to be safe)
	moves = get_moves(a_board, next_color);	//get the moves of the other player
	if ( moves == NULL )
	{
		int current_score = score_board(a_board, next_color);
		if (current_score == TIE_SCORE) return TIE_SCORE;
		if(minim) return WIN_SCORE;
		else return LOOSE_SCORE;
	}
	temp = moves;	//initialize temp
	while ( temp != NULL )
	{
		int temp_score;
		char board_copy[BOARD_SIZE][BOARD_SIZE];
		memcpy(board_copy, a_board, sizeof(board_copy));
		board_cnt++;
		DO_DEBUG2(if (board_cnt%100000 == 0) printf("%d millions of boards.\n", board_cnt/1000000); fflush(stdout);) 
		do_move(board_copy, temp); // now the board copy is updated 
		temp_score = minmax(board_copy, !minim, !next_color, depth+1, alpha, beta);
		
		if ( temp_score > maxi_score )
		{
			maxi_score = temp_score;
			if ( !minim ) alpha = maxi_score;
		}
		if ( temp_score < mini_score )
		{
			mini_score = temp_score;
			if ( minim ) beta = mini_score;
		}
		if ( alpha > beta ) break;
		temp = temp->next;
	}
	free_move(moves);
	if ( minim ){
		return mini_score;
	}
	else{
		return maxi_score;
	}
}

/** returns the best score move/s for the current player.
  * uses calls to minmax (recursive function). */
move *get_move_minmax(char board[BOARD_SIZE][BOARD_SIZE])
{
	char pretty_board[100];
	board_cnt = 0; // a board counter
	int is_best = 0;
	if (MINIMAX_DEPTH == BEST_DEPTH_VALUE)
	{
		MINIMAX_DEPTH = get_best_minmax_depth(board);
		is_best = 1;
	}
	move *moves = get_moves(board, WHITE_TURN); // our options.
	move *max_move = NULL;
	move *prev_max_move = NULL;
	move *temp = moves;
	int alpha = -INF_SCORE;
	int beta = INF_SCORE;
	int max_score = -INF_SCORE;
	int current_score;
	char board_copy[BOARD_SIZE][BOARD_SIZE];

	while ( temp != NULL ){
		
		// copy the board
		memcpy(board_copy, board, sizeof(board_copy));
		board_cnt++;
		//do move 
		do_move(board_copy, temp); // now the board copy is updated 

		// get score for the move, using minmax
		current_score = minmax(board_copy, 1 ,!WHITE_TURN , 1, alpha, beta);
		
		// update max if necessary 
		if ( current_score > max_score )
		{
			free_move(max_move);
			max_move = create_move(temp->from->row, temp->from->column,temp->to->row, temp->to->column);
			max_move->promote = temp->promote;
			max_score = current_score;
			alpha = max_score;
		}
		else if ( current_score == max_score )
		{
			prev_max_move = create_move(temp->from->row, temp->from->column,temp->to->row, temp->to->column);
			prev_max_move->promote = temp->promote;
			prev_max_move->next = max_move;
			max_move = prev_max_move;
		}
		DO_DEBUG2
		(
			printf("check move :\n");
			print_move(temp);
			print_board(board_copy,pretty_board);
			printf("current_score: %d\n",current_score );
			printf("max_score: %d\n",max_score );
			fflush(stdout);
		)
		temp = temp->next;
	}
	free_move(moves);
	DO_DEBUG2(printf("minmax depth : %d \nnum of boards is : %d\n",MINIMAX_DEPTH,board_cnt); fflush(stdout);)
	DO_DEBUG3(printf("minmax depth : %d \nnum of boards is : %d\n",MINIMAX_DEPTH,board_cnt); fflush(stdout);)

	if (is_best == 1) MINIMAX_DEPTH = BEST_DEPTH_VALUE;
	return max_move;
}


/** returns a best score move chosen randomly from the best score moves available.
  * calls get_move_minmax. */
move *get_rand_move_minmax(char board[BOARD_SIZE][BOARD_SIZE])
{
	time_t t;
	srand((unsigned) time(&t));
	int rand_pick = -1;
	move *moves = get_move_minmax(board);
	int cnt = 0;
	move *chosen_temp = NULL;
	move *temp = moves;
	while(temp != NULL)
	{
		cnt++;
		temp = temp->next;
	}
	if (!cnt) return NULL;
	rand_pick = rand()%cnt;// which move to pick. values from 1-cnt
	temp = moves;
	for(int i = 0; i < cnt; i++)
	{
		if (i == rand_pick)
		{
			chosen_temp = create_move(temp->from->row, temp->from->column, temp->to->row, temp->to->column);
			chosen_temp->promote = temp->promote;
			free_move(moves);
			return chosen_temp;
		}
		temp = temp->next;
	}
	return NULL;
}

/** returns the minmax depth (an int) that should be used if playing in "best" difficulty.
  * averages the moves on the board for the two players + checks for special conditions s.a promotion and check.
  * the returned value may be higher than 4. */
int get_best_minmax_depth(char board[BOARD_SIZE][BOARD_SIZE])
{
	int num_of_moves = 0; 
	int num_of_move_enemy = 0;
	
	move *opt_moves = get_moves(board,WHITE_TURN);
	move *temp = opt_moves;
	for ( ; temp != NULL ; temp = temp->next)
	{
		num_of_moves += temp->promote != EMPTY ? 25:1; 
		if(board[temp->from->column][temp->from->row] == WHITE_P  &&  temp->from->row == 1) num_of_moves += 3;
		if(board[temp->from->column][temp->from->row] == BLACK_P  &&  temp->from->row == BOARD_SIZE-2) num_of_moves += 3;
	}
	free_move(opt_moves);
	WHITE_TURN = (WHITE_TURN+1)%2; 
	
	opt_moves = get_moves(board,WHITE_TURN);
	temp = opt_moves;
	for ( ; temp != NULL ; temp = temp->next)
	{
		num_of_move_enemy += temp->promote != EMPTY ? 25:1; 
		if(board[temp->from->column][temp->from->row] == WHITE_P  &&  temp->from->row == 1) num_of_move_enemy += 3;
		if(board[temp->from->column][temp->from->row] == BLACK_P  &&  temp->from->row == BOARD_SIZE-2) num_of_move_enemy += 3;
	}	
	free_move(opt_moves);
	WHITE_TURN = (WHITE_TURN+1)%2;
	
	num_of_moves = (num_of_moves + num_of_move_enemy)/2;
	
	int depth = 1;
	int num_of_boards = 0;
	int score = num_of_moves != 1 ? num_of_moves : 5;
	score = is_check(board,WHITE_TURN) ? 40 - num_of_moves: score;
	int prev_num = num_of_boards;
	for ( ; num_of_boards < 1000000 ; prev_num = num_of_boards, num_of_boards +=  (int) pow((double) score, depth), depth++ );
	DO_DEBUG3(printf("prev num of boards : %d , num_of_boards : %d , best_depth : %d\n",prev_num ,num_of_boards,depth);)
	return MAX_DEPTH_VALUE < depth-1 ? 4 : depth-1;
}

/** (>>>this functions is marked as alternative<<<)returns the minmax depth (an int) that should be used if playing in "best" difficulty.
  * sums the pieces on the board (same score as in the scoring function) with no distinction white/black.
  * the returned value may be higher than 4. */
int get_best_minmax_depth2(void)
{
	
	int black_score = 0;
	int white_score = 0;
	int depth = 1;
	int num_of_boards = 0;
	int pawn_cnt = 0;
	char current = EMPTY;
	location loc;

	for( int row = 0 ; row < BOARD_SIZE ; row++ )
	{
		for( int column = 0 ; column < BOARD_SIZE ; column++ )
		{
			//update current location and piece
			current = board[column][row];
			loc.row = row;
			loc.column = column;
			//update movability

			DO_DEBUG
			(
				printf("loc in score is; \trow:%d\tcolumn:%d\n", loc.row, loc.column);
				fflush(stdout);
			)
			//update score
			switch(current)
			{
				case (EMPTY):
					break;
				case (BLACK_P):
					black_score++;
					pawn_cnt++;
					break;
				case (BLACK_N):
					black_score += 8; 
					break;
				case (BLACK_K):
					black_score += 8; 
					break;
				case (BLACK_B):
					black_score += 14; 
					break;
				case (BLACK_R):
					black_score += 14; 
					break;
				case (BLACK_Q):
					black_score += 28; 
					break;
				case (WHITE_P):
					white_score++;
					pawn_cnt++;
					break;
				case (WHITE_N):
					white_score += 8; 
					break;
				case (WHITE_K):
					white_score += 8; 
					break;
				case (WHITE_B):
					white_score += 14; 
					break;
				case (WHITE_R):
					white_score += 14; 
					break;
				case (WHITE_Q):
					white_score += 28; 
					break;
				
			}
		}
	}

	int score = black_score + white_score;
	int prev_num = num_of_boards;
	score += pawn_cnt ? 42 : 0; 
	for ( ; num_of_boards < 1000000 ; prev_num = num_of_boards, num_of_boards +=  (int) pow((double) score, depth), depth++ );
	DO_DEBUG3(printf("prev num of boards : %d , num_of_boards : %d , best_depth : %d\n",prev_num ,num_of_boards,depth);)
	return MAX_DEPTH_VALUE < depth ? MAX_DEPTH_VALUE : depth;
}



