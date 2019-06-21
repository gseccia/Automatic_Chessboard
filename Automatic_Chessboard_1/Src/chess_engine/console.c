#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "Chess.h"

#include "../axis_manager_stm32f4/axis_manager.h"
#include "../magnetic_grid_interface_stm32/API_magneticGrid.h"
#include "../lcd_interface_stm32f4/i2c-lcd.h"


extern Axis_manager* axis_manager;
extern struct magnetic_grid_manager* grid_manager;
extern char error_message[16];
extern char info_message[16];

// Genral settings
extern int MINIMAX_DEPTH;
extern int GAME;
extern int SETTINGS;
// extern int GUI_MODE; // '0' - application runs in 'Console mode', '1' - 'Gui mode'
extern int TWO_PLAYERS_MODE; // '1' - two players mode, '0' - player vs. AI mode
extern int WHITE_TURN; // 0 - black, 1- white
extern int PLAYER_WHITE;
extern char board[BOARD_SIZE][BOARD_SIZE];
extern int GAME_STATUS;
extern int ch_i,ch_j;

// Game setiings
extern int CHECK_ON_WHITE;
extern int CHECK_ON_BLACK;
int board_cnt = 0;


//						*************** Board init & print methods ****************

void print_line(char line[]){
	int i;
	sprintf(line," |");
	for ( i = 1; i < BOARD_SIZE*4; i++ ){
		strcat(line,"-");
	}
	strcat(line,"|\r\n");
}

void print_board( char board[BOARD_SIZE][BOARD_SIZE],char pretty_board[])
{
	int column,row;
	char tmp[20];
	print_line(pretty_board);
	for (row = BOARD_SIZE-1; row >= 0 ; row--)
	{
		sprintf(tmp,(row < 9 ? "%d" : "%d"), row+1);
		strcat(pretty_board,tmp);
		for ( column = 0; column < BOARD_SIZE; column++ )
		{
			sprintf(tmp,"| %c ", board[column][row]);
			strcat(pretty_board,tmp);
		}
		strcat(pretty_board,"|\r\n");
		print_line(tmp);
		strcat(pretty_board,tmp);
	}
	strcat(pretty_board,"  ");
	for ( column = 0; column < BOARD_SIZE; column++ )
	{
		sprintf(tmp," %c  ", (char)('a' + column));
		strcat(pretty_board,tmp);
	}
	strcat(pretty_board,"\r\n\r\n\0");
}

/** initializes a full board chess board; placing the appropriate char (piece type&color)
  * in the 'board' array. */
void init_board( char board[BOARD_SIZE][BOARD_SIZE])
{
	int column,row;
	char tmp;
	int transform; // delta from 'white' to 'black'
	for (row = 0; row < BOARD_SIZE; row++)
	{
		for (column = 0; column < BOARD_SIZE; column++)
		{
            tmp = EMPTY;
            transform = 'a'-'A';
			if ( row == 0 || row == 7)
			{
				if ( row == 0 ) transform = 0; // don't transform to black
				/*
				if(!PLAYER_WHITE){
					if ( row == 0 ) transform = 0; // don't transform to black
				}
				else{
					if(row == 7) transform = 0;
				}*/
				switch( column )
				{
					case 0:
						tmp = WHITE_R - transform;
						break;
					case 1:
						tmp = WHITE_N - transform;
						break;
					case 2:
						tmp = WHITE_B - transform;
						break;
					case 3:
						tmp = WHITE_Q - transform;
						break;
					case 4:
						tmp = WHITE_K - transform;
						break;
					case 5:
						tmp = WHITE_B - transform;
						break;
					case 6:
						tmp = WHITE_N - transform;
						break;
					case 7:
						tmp = WHITE_R - transform;
						break;
				}
			}
			if ( row == 1 || row == 6 ) // pawns rows
			{
				/*if(!PLAYER_WHITE){
					if ( row == 1 ) transform = 0; // don't transform to black
				}
				else{
					if(row == 6) transform = 0;
				}*/
				if ( row == 1 ) transform = 0;
				tmp = WHITE_P - transform;
			}

			board[column][row] = tmp;
		}
	}
}

//						*************** General methods ****************

/** returns a pointer 'l' to a location constructed of 'row' and 'column'.
  * this functions uses malloc (allocates memory dynamically). */
location *create_location(int row, int column)
{
	location *l;
	if ((l = (location *)malloc(1*sizeof(location))) == NULL) quit();
	l->row = row;
	l->column = column;
	return l;
}

/** returns a pointer 'm' to a move constructed of 'from_row', 'from_column', 'to_row' and 'to_column'.
  * this functions uses malloc (allocates memory dynamically). */
move *create_move(int from_row, int from_column, int to_row, int to_column)
{
	move *m;
	if ((m = (move *)malloc(1*sizeof(move))) == NULL) quit();
	m->next = NULL;
	m->promote = EMPTY;
	if (from_row == -1)
	{
		m->from = NULL; 
		m->to = NULL;
	}
	else
	{
		m->from = create_location(from_row, from_column);;
		m->to = create_location(to_row, to_column);;
	}
	return m;
}
/** frees location . */
void free_location(location *l)
{
	if ( l != NULL ) free(l);
}

/** frees move (as a link list). */
void free_move(move *m)
{
	if ( m != NULL )
	{
		free_move(m->next);
		free_location(m->from);
		free_location(m->to);
		free(m);
	}
} 

/**quits the program. */ 
void quit( void )
{
  exit(0);
}

/** checks if l is valid location in the board
  * returns '1' if l is a legal location
  * else, returns '0'. */
int is_legal_location( location l )
{ 
  if ( l.row > BOARD_SIZE - 1  ||l.row < 0 ||\
       l.column > BOARD_SIZE - 1  || l.column < 0) return 0; //location is outside the board
  return 1;	//this location is legal (black and on the board)
}

//						*************** Settings methods ****************

/** checks if 'piece' can be set at location 'l'. 
  * returns '1' if True, else '0'. */
int is_legal_placement( location l, char piece )
{
	int count = 0;
	int is_bishop = ((piece == WHITE_B) || (piece == BLACK_B)) ? 1 : 0;
	int max_allowed; 

	//check for allowed amount this piece
	switch( tolower(piece) )
	{
		case WHITE_P:
			max_allowed = 8;
			break;
		case WHITE_B:
			max_allowed = 2;
			break;
		case WHITE_N:
			max_allowed = 2;
			break;
		case WHITE_R:
			max_allowed = 2;
			break;
		case WHITE_Q:
			max_allowed = 1;
			break;
		case WHITE_K:
			max_allowed = 1;
			break;
	}
	// count and check that the amount of this piece is legal after placement	
	for ( int i = 0; i < BOARD_SIZE; i++ )
	{
		for ( int j = 0; j < BOARD_SIZE; j++ )
		{
			if ( l.row == i && l.column == j ) continue;
			if ( board[j][i] == piece )
			{
				count++;
				if ( count >= max_allowed ) return 0; 
				if ( is_bishop && ((l.row + l.column) % 2 == (i + j) % 2 ) ) return 0;
			}
		}
	}
	return 1;
}

/**  translates a 'word' to it's relevant char 'piece'. */
char to_piece( char *word )
{
	if ( strcmp(word, "king") == 0 ) return WHITE_K;
	if ( strcmp(word, "queen") == 0 ) return WHITE_Q;
	if ( strcmp(word, "rook") == 0 ) return WHITE_R;
	if ( strcmp(word, "bishop") == 0 ) return WHITE_B;
	if ( strcmp(word, "knight") == 0 ) return WHITE_N;
	return WHITE_P;
}

/** returns '1' if the board set-up is legal.
  * '0' otherwise. */
int check_settings( void )
{
	int kings_cnt = 0;
	for ( int i = 0; i < BOARD_SIZE; i++ )
	{
		for ( int j = 0; j < BOARD_SIZE; j++ )
		{
			if (tolower(board[i][j]) == WHITE_K) kings_cnt++;
		}
	}
	if ( kings_cnt != 2 ) return 0;
	return 1;
}
			

//						*************** input methods ****************

/** reads the input from user by placing a it char-by-char in 'block'.
  * assumes that the input is less than BUFF_SIZE. */
void read_input( char block[BUFF_SIZE] )
{
	// get_command(block);
    if ( (strlen(block)>0) && (block[strlen(block) - 1] == '\n') )
        block[strlen (block) - 1] = '\0';
}

int settings_input_manager(){

	// start command
	if ( check_settings() )
	{
		start_game();
		return 0;
	}
	strcpy(error_message,WROND_BOARD_INITIALIZATION);//print_message(WROND_BOARD_INITIALIZATION);
	return -1;
}

/** receives 'input'.
  * if input contains valid, setting phase, instructions;
  * executes them. 
  * otherwise in nessecary, prints ILLEGAL_COMMAND. */
void parse_input_settings( char input[BUFF_SIZE] )
{
	char words[BUFF_SIZE]; // will be a copy of the input.
	char *word;
	char pretty_board[100];

	strcpy(words, input); 
	word = strtok(words, " ");
	
	// check if 'word' matches a legal (settings) command: 
	if ( strcmp(word, "difficulty") == 0 && !TWO_PLAYERS_MODE )
	{
		word = strtok(NULL, " "); //next word 
		if ( strcmp(word, "best") == 0 ) MINIMAX_DEPTH = BEST_DEPTH_VALUE;
		else if ( strcmp(word, "depth") == 0 )
		{
			word = strtok(NULL, " "); //next word 
			int x = atoi(word); // returns '0' if not valid
			set_minmax_depth(x);		
		}
	}
	else if ( strcmp(word, "user_color") == 0 && !TWO_PLAYERS_MODE )
	{
		word = strtok(NULL, " ");
		set_user_color(word);
	}
	else if ( strcmp(word, "clear") == 0 )
	{ 
		clear();
	}
	else if ( strcmp(word, "rm") == 0 )
	{
		word = strtok(NULL, " ");
		location l = str_to_location(word);
		remove_location(l);	
	}
	else if ( strcmp(word, "set") == 0)
	{
		int is_white = 1;		
		word = strtok(NULL, " ");
		location l = str_to_location(word); 
		word = strtok(NULL, " ");
		if ( strcmp(word, "black") == 0 ) is_white = 0;
		word = strtok(NULL, " ");
		set_location( l, is_white, to_piece(word) );	
	}
	else if ( strcmp(word, "game_mode") == 0 )
	{
		word = strtok(NULL, " ");
		if ( atoi(word) != 1 && atoi(word) != 2 )
		{
			print_message(WRONG_GAME_MODE);
			return;
		}
		TWO_PLAYERS_MODE =  (atoi(word) == 2) ? 0 : 1;
		if ( TWO_PLAYERS_MODE )
		{
			print_message(TWO_PLAYERS_GAME_MODE);
		}
		else print_message(PLAYER_VS_AI_GAME_MODE);
	}
	else if ( strcmp(word, "next_player") == 0)
	{
		word = strtok(NULL, " ");
		if ( strcmp(word, "black") == 0 ) WHITE_TURN = 0;
		if ( strcmp(word, "white") == 0 ) WHITE_TURN = 1;
	}
	else if ( strcmp(word, "print") == 0) print_board(board,pretty_board);
	/*else if ( strcmp(word, "load") == 0)
	{
		if ( load_xml(strtok(NULL, " ")) ) print_message(WRONG_FILE_NAME)
		else print_board(board,pretty_board);
	}*/
	else if ( strcmp(word, "start") == 0 )
	{
		if ( check_settings() ) 
		{
			start_game();
		}
		else print_message(WROND_BOARD_INITIALIZATION);
	}
	// 'word' doesn't match any legal (settings) command 
	else print_message(ILLEGAL_COMMAND);
}

/** converts locus into a location. */
location str_to_location( char* locus )
{
	location l;
	int column = locus[1] - 'a'; // convert the row to integer
	int row = 0;
	for ( int i = 3 ; locus[i] != '>' ; i++ )
	{
		row = 10 * row + (locus[i] - '0'); // calculate column value (could be '10')
	}
	l.column = column; 
	l.row = row-1; // normalize to range starting from 0
	return l;
}


//						*************** setters methods ***************

/** set the minimax_depth. */
void set_minmax_depth( int x ) 
{ 
  if ( x > 0 && x < 5 ) MINIMAX_DEPTH = x; 
  else print_message(WRONG_MINIMAX_DEPTH); // x is illegal
}

/** sets user color, default is white. */
void set_user_color( char *s )
{  
  if ( strcmp(s, "black")== 0 ) PLAYER_WHITE = 0;
}

/** clears the board; all board cells are EMPTY. */
void clear(void)
{ 
  for ( int i = 0; i < BOARD_SIZE; i++ )
  {
    for ( int j = 0; j < BOARD_SIZE; j++ )
	{
		board[i][j] = EMPTY;
	}
  }
}

/** changes game status; !SETTINGS and to GAME. */
void start_game(void)
{
	SETTINGS = 0;
	GAME = 1;
}

/** sets EMPTY to location 'l'. */
void remove_location(location l)
{ 
	if ( is_legal_location(l) ) board[l.column][l.row] = EMPTY;
	else print_message(WRONG_POSITION);
}

/** sets a 'piece' on the board at location 'l' if 'l' is legal.
  * 'white' is '1' if the 'piece' should be white (otherwise, black) 
  * if 'l' is an illegal location or the setting createsd an illegal board,
  * prints an appropriate message. */
void set_location( location l, int white, char piece )
{
	if ( !(white) ) piece = piece - ('a' - 'A');
	if ( is_legal_location(l) )
	{
	if ( is_legal_placement(l, piece) ) board[l.column][l.row] = piece;
	else print_message(NO_PIECE);
	}
	else print_message(WRONG_POSITION);
}

//						*************** GAME methods ****************

// Our adapter methods

void perform_move(char a_board[BOARD_SIZE][BOARD_SIZE], move *user_move){
	int empty = a_board[user_move->to->column][user_move->to->row] == EMPTY;
	do_move(a_board,user_move);
	//Mossa sulla scacchiera reale
	if(!empty) axis_manager_move(axis_manager,user_move->to->row,user_move->to->column,OUT_CHESSBOARD,OUT_CHESSBOARD);
	axis_manager_move(axis_manager,user_move->from->row,user_move->from->column,user_move->to->row,user_move->to->column);
}


int player_input_game_manager(){
	// Read the grid and get the move

	move *m = fetch_moves(grid_manager);

	location from;
	location to;

	if(m == NULL){
		from.row = -1;
		from.column = -1;
	}
	else{
		from.row = m->from->row;
		from.column = m->from->column;

		to.row = m->to->row;
		to.column = m->to->column;
	}

	if ( !is_legal_location(from) || !is_legal_location(to))
	{
				strcpy(error_message,"Illegal pos");//print_message(WRONG_POSITION);
				free_move(m);
				//ANTONIO Mossa Errata e ripristino
				return 1; // repeat
	}
	else if ( (WHITE_TURN && !IS_WHITE(board[from.column][from.row])) || (!WHITE_TURN && !IS_BLACK(board[from.column][from.row])))
	{
				strcpy(error_message,"Illegal piece");
				// print_message(NOT_YOUR_PIECE);
				free_move(m);
				return 1; // repeat
	}

	//create_move(from.row, from.column, to.row, to.column);

	// m->promote = piece_name_to_char(m, word); //ANTONIO probabile arrivo alla fine

	DO_DEBUG(print_message("move:created move \n");fflush(stdout);)

	if ( !is_legal_move(m) )
	{
				free_move(m);
				strcpy(error_message,"Illegal move");//print_message(ILLEGAL_MOVE);
				//ANTONIO Mossa Errata e ripristino
				return 1; // repeat
	}
	else
	{
				DO_DEBUG(print_message("move: move is legal checked \n"); fflush(stdout);)
				update_magnetic_grid(grid_manager); //Update status

				int white_piece = IS_WHITE(board[m->from->column][m->from->row]);
				if(((board[m->from->column][m->from->row] == WHITE_P) || (board[m->from->column][m->from->row] == BLACK_P))
					&& ((white_piece && m->to->row == BOARD_SIZE-1) || (!white_piece && m->to->row == 0))){
					ch_i = m->to->column;
					ch_j = m->to->row;
					return 2;
				}

				do_move(board, m);
				free_move(m);
				// print_board(board,pretty_board);
				return 0;
			// check for tie / mate / check !! ???
	}
}


//end out adapters

/** returns '1' if current player is still playing (the turn hasn't passed).
  * else returns'0' (turn will pass to the next player). */
int parse_input_game( char input[BUFF_SIZE] )
{
	char words[BUFF_SIZE]; // will be a copy of the input.
	char *word;
	char pretty_board[100];

	strcpy(words, input); 
	word = strtok(words, " ");
	
	// check if 'word' matches a legal (settings) command: 
	/*if ( strcmp(word, "save") == 0 ) // && !TWO_PLAYERS_MODE )
	{
		if ( save_xml(strtok(NULL, " ")) ) print_message(WRONG_FILE_NAME);
		return 1; // repeat
	}*/
	
	if ( strcmp(word, "get_moves") == 0 ) //
	{
		word = strtok(NULL, " ");
		location from  = str_to_location(word);
		if (!is_legal_location(from))
		{
			print_message(WRONG_POSITION);
			return 1; // repeat
		}
		else if ( (WHITE_TURN && !IS_WHITE(board[from.column][from.row])) || 
				(!WHITE_TURN && !IS_BLACK(board[from.column][from.row])) )
		{
			print_message(NOT_YOUR_PIECE);
			return 1; // repeat
		}
		move *m = get_piece_moves(board, &from);
		print_all_moves(m); 
		free_move(m);
		return 1; // repeat
	}
	else if( strcmp(word, "get_best_moves") == 0 )  
	{
		int real_minimax_depth = MINIMAX_DEPTH; // save the orginal MINIMAX_DEPTH
		word = strtok(NULL, " "); //next word 
		if ( strcmp(word, "best") == 0 ) 
		{
			MINIMAX_DEPTH = get_best_minmax_depth(board); // change temporarily the MINIMAX_DEPTH
			move *best_moves = get_move_minmax(board);
			print_all_moves(best_moves);
			free_move(best_moves);
		}
		else 
		{
			int x = atoi(word); // returns '0' if not valid
			MINIMAX_DEPTH = x; //set_minmax_depth(x); // change temporarily the MINIMAX_DEPTH
			move *best_moves = get_move_minmax(board);
			print_all_moves(best_moves);
			free_move(best_moves);			
		}
		MINIMAX_DEPTH = real_minimax_depth; // return to the orginal MINIMAX_DEPTH
	}
	else if( strcmp(word, "get_score") == 0 )  
	{
		int real_minimax_depth = MINIMAX_DEPTH; // save the orginal MINIMAX_DEPTH
		
		word = strtok(NULL, " "); //next word 
		if ( strcmp(word, "best") == 0 ) 
		{
			MINIMAX_DEPTH = get_best_minmax_depth(board); // change temporarily the MINIMAX_DEPTH
		}
		else 
		{
			int x = atoi(word); // returns '0' if not valid
			set_minmax_depth(x); // change temporarily the MINIMAX_DEPTH		
		}
		word = strtok(NULL, " ");
		word = strtok(NULL, " ");
		location from  = str_to_location(word);
		word = strtok(NULL, " ");
		word = strtok(NULL, " ");
		location to  = str_to_location(word);
		if ( !is_legal_location(from) || !is_legal_location(to))
		{
			print_message(WRONG_POSITION);
			MINIMAX_DEPTH = real_minimax_depth; // return to the orginal MINIMAX_DEPTH
			return 1; // repeat
		}
		else if ( (WHITE_TURN && !IS_WHITE(board[from.column][from.row])) || 
				(!WHITE_TURN && !IS_BLACK(board[from.column][from.row])) )
		{
			print_message(NOT_YOUR_PIECE);
			MINIMAX_DEPTH = real_minimax_depth; // return to the orginal MINIMAX_DEPTH
			return 1; // repeat
		}
		move *m = create_move(from.row, from.column, to.row, to.column);
		word = strtok(NULL, " ");
		m->promote = piece_name_to_char(m, word);
		if ( !is_legal_move(m) ) 
		{
			free_move(m); 
			print_message(ILLEGAL_MOVE);
			MINIMAX_DEPTH = real_minimax_depth; // return to the orginal MINIMAX_DEPTH
			return 1; // repeat
		}
		else
		{
			char board_copy[BOARD_SIZE][BOARD_SIZE];
			// copy the board
			memcpy(board_copy, board, sizeof(board_copy));
			
			//do move 
			do_move(board_copy, m); // now the board copy is updated 

			// get score for the move, using minmax
			board_cnt = 0; // a board counter
			int move_score = minmax(board_copy, 1 ,!WHITE_TURN , 1, -INF_SCORE, INF_SCORE);
			free_move(m);
			if ( move_score == WIN_SCORE ) print_message("WIN score\n")
			else if ( move_score == TIE_SCORE ) print_message("TIE score\n")
			else if ( move_score == LOOSE_SCORE ) print_message("LOOSE score\n")
			else printf("%d\n",move_score);
			MINIMAX_DEPTH = real_minimax_depth; // return to the orginal MINIMAX_DEPTH
		}
		MINIMAX_DEPTH = real_minimax_depth; // return to the orginal MINIMAX_DEPTH
	}
	else if ( strcmp(word, "move") == 0 ) // 
	{
		word = strtok(NULL, " ");
		location from  = str_to_location(word);
		word = strtok(NULL, " ");
		word = strtok(NULL, " ");
		location to  = str_to_location(word);
		if ( !is_legal_location(from) || !is_legal_location(to))
		{
			print_message(WRONG_POSITION);
			//ANTONIO Mossa Errata e ripristino
			return 1; // repeat
		}
		else if ( (WHITE_TURN && !IS_WHITE(board[from.column][from.row])) || 
				(!WHITE_TURN && !IS_BLACK(board[from.column][from.row])) )
		{
			print_message(NOT_YOUR_PIECE);
			return 1; // repeat
		}
		move *m = create_move(from.row, from.column, to.row, to.column);
		word = strtok(NULL, " ");
		m->promote = piece_name_to_char(m, word); 
		DO_DEBUG(print_message("move:created move \n");fflush(stdout);)
		if ( !is_legal_move(m) ) 
		{
			free_move(m); 
			print_message(ILLEGAL_MOVE);
			//ANTONIO Mossa Errata e ripristino
			return 1; // repeat
		}
		else
		{			
			DO_DEBUG(print_message("move: move is legal checked \n"); fflush(stdout);)			
			do_move(board, m);
			free_move(m); 
			print_board(board,pretty_board);
			return 0;
		// check for tie / mate / check !! ???
		}
	}
	return 1;

}

/** returns char 'piece' which is the translation of 'name' to char.
  *	the functions uses 'm' to decide if 'piece' should be black/white. */
char piece_name_to_char(move *m, char *name)
{
	char piece = EMPTY;
	int white_piece = IS_WHITE(board[m->from->column][m->from->row]);
	if ( (board[m->from->column][m->from->row] != WHITE_P) && (board[m->from->column][m->from->row] != BLACK_P) ) return piece;
	if ( name == NULL )
	{
		if ( (white_piece && m->to->row == BOARD_SIZE-1) || (!white_piece && m->to->row == 0) ) 
		{
			piece = white_piece ? WHITE_Q : BLACK_Q;
			return piece;
		}
		else return piece;
	}

	if ( strcmp(name, "queen") == 0 ) piece = white_piece ? WHITE_Q : BLACK_Q;
	else if ( strcmp(name, "rook") == 0 ) piece = white_piece ? WHITE_R : BLACK_R;
	else if ( strcmp(name, "knight") == 0 ) piece = white_piece ? WHITE_N : BLACK_N;
	else if ( strcmp(name, "bishop") == 0 ) piece = white_piece ? WHITE_B : BLACK_B;
	return piece;
}
/** return '1' of m is a legal move,
  * '0' otherwise. */
int is_legal_move(move* m)
{
	location *temp_loc1_from; //  pointer for the pattern moves
	location *temp_loc1_to; // pointer for the user move
	location *temp_loc2_from; //  pointer for the pattern moves
	location *temp_loc2_to; // pointer for the user move
	move *moves = get_moves(board, WHITE_TURN); //get pattern moves (legal)
	move * temp_moves = moves;
	while(temp_moves != NULL){ // check if one of the pattern moves is the same as the user move
		temp_loc1_from = temp_moves->from;
		temp_loc2_from = m->from;
		temp_loc1_to = temp_moves->to;
		temp_loc2_to = m->to;
		if ( temp_loc1_from != NULL  && temp_loc1_to != NULL && temp_loc2_from != NULL && temp_loc2_to != NULL ) 
		{
			if ( (temp_loc1_from->row == temp_loc2_from->row )&& temp_loc1_from->column == temp_loc2_from->column 
				&& temp_loc1_to->column == temp_loc2_to->column && temp_loc1_to->row == temp_loc2_to->row )
			{
				DO_DEBUG
				(
				printf("good move!"); 
				print_move(temp_moves);
				print_move(m);
				)
				free_move(moves); 
				return 1;
			}
		}
		temp_moves = temp_moves->next;
	}
	free_move(moves); 
	return 0;
}

/** returns 'all_moves'; all the available moves for the requested player ('white_turn'),
  *	in the given board ('a_board').
  * the function goes through 'a_board' cell-by-cell, if the color of the current piece
  * matches 'white turn', the function concatenates 'all_moves' and new moves for the current piece.
  * if no moves are available, return NULL.
  * uses method "create_location" & "get_piece_moves" (which use malloc).  */
move *get_moves(char a_board[BOARD_SIZE][BOARD_SIZE], int white_turn)
{
	move *all_moves = NULL;
	location *loc = create_location(-1, -1);
	DO_DEBUG
	(
		print_message("get_moves: emty location create  \n");
		fflush(stdout);
	)
	for ( int row = 0; row < BOARD_SIZE; row++ )
	{
		for ( int column = 0; column < BOARD_SIZE; column++ )
		{
			if ( (IS_WHITE(a_board[column][row]) && white_turn) || (IS_BLACK(a_board[column][row]) && !white_turn) )
			{
				loc->row = row; 
				loc->column = column;
				DO_DEBUG
				(
					printf("get_moves: start get_piece_moves row - %d, column - %d  \n",row,column);
					fflush(stdout);
				)
				all_moves = link_moves(all_moves, get_piece_moves(a_board, loc));
				DO_DEBUG
				(
					printf("get_moves: done get_piece_moves row - %d, column - %d  \n",row,column);
					fflush(stdout);
				)
			}
		}
	}
	free_location(loc); 
	return all_moves;
}

/** returns moves available from location 'from' on board 'a_board'.
  * the function mediates the call to the relevant "get X move" function (X being the type of piece).
  * all the "get X move" allocated memory dynamically. */
move *get_piece_moves(char a_board[BOARD_SIZE][BOARD_SIZE], location *from)
{
	char nrm_piece = (IS_WHITE(a_board[from->column][from->row])) ? 
					 a_board[from->column][from->row] : a_board[from->column][from->row] + ('a' - 'A');  //normalize to white
	switch(nrm_piece)
	{
		case (WHITE_P):
			return get_p_moves(a_board, from);
			break;
		case (WHITE_B):
			return get_b_moves(a_board, from);
			break;
		case (WHITE_N):
			return get_n_moves(a_board, from);
			break;
		case (WHITE_R):
			return get_r_moves(a_board, from);
			break;
		case (WHITE_Q):
			return get_q_moves(a_board, from);
			break;
		case (WHITE_K):
			return get_k_moves(a_board, from);
			break;
	}
	return NULL;
}

/** returns move *'res_moves' which constitute all the available options for the pawn
  * located in location 'from' on board 'a_board'.*/
move *get_p_moves(char a_board[BOARD_SIZE][BOARD_SIZE], location *from)
{
	DO_DEBUG
	(
		printf("get_p_moves: start  \n");
		fflush(stdout);
	)
	move *res_moves = NULL;
	char a_board_copy[BOARD_SIZE][BOARD_SIZE];
	char piece = a_board[from->column][from->row];
	int direction = IS_WHITE(piece) ? 1 : -1;
	location opt_locs[3];
	opt_locs[0].row = from->row + direction;	
	opt_locs[0].column = from->column;
	opt_locs[1].row = from->row + direction;	
	opt_locs[1].column = from->column - 1;
	opt_locs[2].row = from->row + direction;
	opt_locs[2].column = from->column + 1;
	DO_DEBUG
	(
		printf("get_p_moves: creat all opt_locs  - \nopt_locs1 row:%d ,column:%d \nopt_locs2 row:%d ,column:%d \nopt_locs3 row:%d ,column:%d \n",opt_locs[0].row,opt_locs[0].column,opt_locs[1].row,opt_locs[1].column,opt_locs[2].row,opt_locs[2].column);
		fflush(stdout);
	)
	move *opt_moves[12];
	for (int i = 0 ; i<12 ; i++) opt_moves[i] = NULL;
	for (int i = 0 ; i<3 ; i++)	
	{
		if (!i && is_legal_location(opt_locs[i]) &&  a_board[opt_locs[i].column][opt_locs[i].row] != EMPTY) continue;
		if (i && is_legal_location(opt_locs[i]) &&  (IS_SAME_COLOR(piece,a_board[opt_locs[i].column][opt_locs[i].row]) || a_board[opt_locs[i].column][opt_locs[i].row] == EMPTY)) continue;
		
		if (is_legal_location(opt_locs[i]) && !IS_SAME_COLOR(piece,a_board[opt_locs[i].column][opt_locs[i].row])) 
		{
			if ( (IS_BLACK(piece) && opt_locs[i].row == 0 ) || (IS_WHITE(piece) && opt_locs[i].row == BOARD_SIZE-1) ) // check if promotion is needed
			{
				DO_DEBUG
				(
					printf("get_p_moves: need prom?? \n");
					fflush(stdout);
				)
				opt_moves[4*i] = create_move(from->row, from->column, opt_locs[i].row, opt_locs[i].column);
				opt_moves[4*i]->promote = IS_WHITE(piece) ? WHITE_Q : BLACK_Q;
				memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
				do_move(a_board_copy, opt_moves[4*i]);
				if (is_check(a_board_copy, IS_WHITE(piece)))
				{
					free_move(opt_moves[4*i]); 
					opt_moves[4*i] = NULL;
				}
				opt_moves[4*i+1 ] = create_move(from->row, from->column, opt_locs[i].row, opt_locs[i].column);
				opt_moves[4*i+1]->promote = IS_WHITE(piece) ? WHITE_B : BLACK_B;
				memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
				do_move(a_board_copy, opt_moves[4*i+1]);
				if (is_check(a_board_copy, IS_WHITE(piece)))
				{
					free_move(opt_moves[4*i+1]); 
					opt_moves[4*i+1] = NULL;
				}
				opt_moves[4*i+2 ] = create_move(from->row, from->column, opt_locs[i].row, opt_locs[i].column);
				opt_moves[4*i+2]->promote = IS_WHITE(piece) ? WHITE_R : BLACK_R;
				memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
				do_move(a_board_copy, opt_moves[4*i+2]);
				if (is_check(a_board_copy, IS_WHITE(piece)))
				{
					free_move(opt_moves[4*i+2]); 
					opt_moves[4*i+2] = NULL;
				}
				
				opt_moves[4*i+3 ] = create_move(from->row, from->column, opt_locs[i].row, opt_locs[i].column);
				opt_moves[4*i+3]->promote = IS_WHITE(piece) ? WHITE_N : BLACK_N;
				memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
				do_move(a_board_copy, opt_moves[4*i+3]);
				if (is_check(a_board_copy, IS_WHITE(piece)))
				{
					free_move(opt_moves[4*i+3]); 
					opt_moves[4*i+3] = NULL;
				}			
			}				
			else
			{
				DO_DEBUG
				(
					printf("get_p_moves: start create_move to row : %d column: %d \n", opt_locs[i].row, opt_locs[i].column);
					fflush(stdout);
				)
				opt_moves[i] = create_move(from->row, from->column, opt_locs[i].row, opt_locs[i].column);
				DO_DEBUG
				(
					printf("get_p_moves: create_move to row : %d column: %d \n", opt_locs[i].row, opt_locs[i].column);
					fflush(stdout);
				)
				memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
				do_move(a_board_copy, opt_moves[i]);
				if (is_check(a_board_copy, IS_WHITE(piece)))
				{
					free_move(opt_moves[i]); 
					opt_moves[i] = NULL;
				}			
			}				
		}		
	}
	for (int i = 0 ; i<12 ; i++)
	{
		if (opt_moves[i] == NULL) continue;
		if (res_moves==NULL) res_moves = opt_moves[i];
		else
		{
			opt_moves[i]->next = res_moves;
			res_moves = opt_moves[i];
		}

	}
	//add first move of pawn (if P_FIRST_MOVE == 1 in Chess.h) 
	DO_P_FIRST_MOVE
	(
		if ( (IS_BLACK(piece) && from->row == BOARD_SIZE-2 ) || (IS_WHITE(piece) && from->row == 1) )
		{
			if ( a_board[from->column][from->row + (direction*2)] == EMPTY )
			{
				move *p_first_move = create_move(from->row, from->column, from->row + (direction*2), from->column );
				if (res_moves==NULL) res_moves = p_first_move;
				else
				{
					p_first_move->next = res_moves;
					res_moves = p_first_move;
				}
			}
		}
	)
	return res_moves;
}

/** returns move *'res_moves' which constitute all the available options for the rook
  * located in location 'from' on board 'a_board'. */
move *get_r_moves(char a_board[BOARD_SIZE][BOARD_SIZE], location *from)
{
	DO_DEBUG
	(
		printf("get_r_moves: start  \n");
		fflush(stdout);
	)
	move *res_moves = NULL;
	char piece = a_board[from->column][from->row];
	char a_board_copy[BOARD_SIZE][BOARD_SIZE];
	int up_first_seen = 0;
	int down_first_seen = 0;
	int right_first_seen = 0;
	int left_first_seen = 0;
	move *opt_moves[14];
	int cnt_moves = 0;
	location loc_to;
	for (int i = 1 ; i < 14 ; i++) opt_moves[i] = NULL;
	for (int i = 1 ; i < BOARD_SIZE ; i++)
	{
		if( !up_first_seen )
		{
			loc_to.row =  from->row + i;
			loc_to.column = from->column;
			if ( is_legal_location(loc_to) )
			{				
				if( !IS_SAME_COLOR(piece, a_board[loc_to.column][loc_to.row])) 
				{
					opt_moves[cnt_moves] = create_move(from->row, from->column, loc_to.row, loc_to.column);
					memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
					do_move(a_board_copy, opt_moves[cnt_moves]);
					if (is_check(a_board_copy, IS_WHITE(piece)))
					{
						free_move(opt_moves[cnt_moves]); 
						opt_moves[cnt_moves] = NULL;
						cnt_moves--;
					}			
					cnt_moves++;
				}
				if ( a_board[loc_to.column][loc_to.row] != EMPTY ) up_first_seen = 1;
			}
		}
		if(!down_first_seen)
		{
			loc_to.row =  from->row - i;
			loc_to.column = from->column;
			if (is_legal_location(loc_to))
			{
				if( !IS_SAME_COLOR(piece, a_board[loc_to.column][loc_to.row]) ) 
				{
					opt_moves[cnt_moves] = create_move(from->row, from->column, loc_to.row, loc_to.column);
					memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
					do_move(a_board_copy, opt_moves[cnt_moves]);
					if (is_check(a_board_copy, IS_WHITE(piece)))
					{
						free_move(opt_moves[cnt_moves]); 
						opt_moves[cnt_moves] = NULL;
						cnt_moves--;
					}			
					cnt_moves++;
				}				
			
				if ( a_board[loc_to.column][loc_to.row] != EMPTY ) down_first_seen = 1;
			}
		}
		if(!right_first_seen)
		{
			loc_to.row =  from->row;
			loc_to.column = from->column + i;
			if (is_legal_location(loc_to))
			{				
				if( !IS_SAME_COLOR(piece, a_board[loc_to.column][loc_to.row]) ) 
				{
					opt_moves[cnt_moves] = create_move(from->row, from->column, loc_to.row, loc_to.column);
					memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
					do_move(a_board_copy, opt_moves[cnt_moves]);
					if (is_check(a_board_copy, IS_WHITE(piece)))
					{
						free_move(opt_moves[cnt_moves]); 
						opt_moves[cnt_moves] = NULL;
						cnt_moves--;
					}			
					cnt_moves++;
				}			
			
				if ( a_board[loc_to.column][loc_to.row] != EMPTY )right_first_seen = 1;
			}
		}
		if(!left_first_seen)
		{
			loc_to.row =  from->row;
			loc_to.column = from->column - i;
			if ( is_legal_location(loc_to) )
			{
				if( !IS_SAME_COLOR(piece, a_board[loc_to.column][loc_to.row]) ) 
				{
					opt_moves[cnt_moves] = create_move(from->row, from->column, loc_to.row, loc_to.column);
					memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
					do_move(a_board_copy, opt_moves[cnt_moves]);
					if (is_check(a_board_copy, IS_WHITE(piece)))
					{
						free_move(opt_moves[cnt_moves]); 
						opt_moves[cnt_moves] = NULL;
						cnt_moves--;
					}			
					cnt_moves++;
				}		
				
				if ( a_board[loc_to.column][loc_to.row] != EMPTY ) left_first_seen = 1;
			}
		}
		
	}
	for (int i = 0 ; i<cnt_moves ; i++)
	{
		if (res_moves==NULL) res_moves = opt_moves[i];
		else
		{
			opt_moves[i]->next = res_moves;
			res_moves = opt_moves[i];
		}
	}
	return res_moves;
}

/** returns move *'res_moves' which constitute all the available options for the bishop
  * located in location 'from' on board 'a_board'. */
move *get_b_moves(char a_board[BOARD_SIZE][BOARD_SIZE], location *from)
{
	DO_DEBUG
	(
		printf("get_b_moves: start  \n");
		fflush(stdout);
	)
	move *res_moves = NULL;
	char piece = a_board[from->column][from->row];
	char a_board_copy[BOARD_SIZE][BOARD_SIZE];
	int up_right_first_seen = 0;
	int down_right_first_seen = 0;
	int down_left_first_seen = 0;
	int up_left_first_seen = 0;
	move *opt_moves[14];
	int cnt_moves = 0;
	location loc_to;
	for (int i = 1 ; i < 14 ; i++) opt_moves[i] = NULL;
	for (int i = 1 ; i < BOARD_SIZE ; i++)
	{
		if( !up_right_first_seen )
		{
			loc_to.row =  from->row + i;
			loc_to.column = from->column + i;
			if ( is_legal_location(loc_to) )
			{				
				if( !IS_SAME_COLOR(piece, a_board[loc_to.column][loc_to.row]) ) 
				{
					opt_moves[cnt_moves] = create_move(from->row, from->column, loc_to.row, loc_to.column);
					memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
					do_move(a_board_copy, opt_moves[cnt_moves]);
					if (is_check(a_board_copy, IS_WHITE(piece)))
					{
						free_move(opt_moves[cnt_moves]); 
						opt_moves[cnt_moves] = NULL;
						cnt_moves--;
					}			
					cnt_moves++;
				}
				if ( a_board[loc_to.column][loc_to.row] != EMPTY ) up_right_first_seen = 1;
			}
		}
		if(!down_right_first_seen)
		{
			loc_to.row =  from->row - i;
			loc_to.column = from->column + i;
			if (is_legal_location(loc_to))
			{
				if( !IS_SAME_COLOR(piece, a_board[loc_to.column][loc_to.row]) ) 
				{
					opt_moves[cnt_moves] = create_move(from->row, from->column, loc_to.row, loc_to.column);
					memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
					do_move(a_board_copy, opt_moves[cnt_moves]);
					if (is_check(a_board_copy, IS_WHITE(piece)))
					{
						free_move(opt_moves[cnt_moves]);
						opt_moves[cnt_moves] = NULL;
						cnt_moves--;
					}			
					cnt_moves++;
				}				
			
				if ( a_board[loc_to.column][loc_to.row] != EMPTY ) down_right_first_seen = 1;
			}
		}
		if(!down_left_first_seen)
		{
			loc_to.row =  from->row - i ;
			loc_to.column = from->column - i;
			if (is_legal_location(loc_to))
			{				
				if( !IS_SAME_COLOR(piece, a_board[loc_to.column][loc_to.row]) ) 
				{
					opt_moves[cnt_moves] = create_move(from->row, from->column, loc_to.row, loc_to.column);
					memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
					do_move(a_board_copy, opt_moves[cnt_moves]);
					if (is_check(a_board_copy, IS_WHITE(piece)))
					{
						free_move(opt_moves[cnt_moves]);
						opt_moves[cnt_moves] = NULL;
						cnt_moves--;
					}			
					cnt_moves++;
				}			
			
				if ( a_board[loc_to.column][loc_to.row] != EMPTY )down_left_first_seen = 1;
			}
		}
		if(!up_left_first_seen)
		{
			loc_to.row =  from->row + i;
			loc_to.column = from->column - i;
			if ( is_legal_location(loc_to) )
			{
				if( !IS_SAME_COLOR(piece, a_board[loc_to.column][loc_to.row]) )
				{
					opt_moves[cnt_moves] = create_move(from->row, from->column, loc_to.row, loc_to.column);
					memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
					do_move(a_board_copy, opt_moves[cnt_moves]);
					if (is_check(a_board_copy, IS_WHITE(piece)))
					{
						free_move(opt_moves[cnt_moves]);
						opt_moves[cnt_moves] = NULL;
						cnt_moves--;
					}			
					cnt_moves++;
				}		
				
				if ( a_board[loc_to.column][loc_to.row] != EMPTY ) up_left_first_seen = 1;
			}
		}
		
	}
	for (int i = 0 ; i<cnt_moves ; i++)
	{
		if (res_moves==NULL) res_moves = opt_moves[i];
		else
		{
			opt_moves[i]->next = res_moves;
			res_moves = opt_moves[i];
		}
	}
	return res_moves;
}


/** returns move *'res_moves' which constitute all the available options for the queen
  * located in location 'from' on board 'a_board'. */
move *get_q_moves(char a_board[BOARD_SIZE][BOARD_SIZE], location *from)
{
	PRINTD("get_q_moves: start  \n")
	return link_moves(get_b_moves(a_board, from), get_r_moves(a_board, from));
}

/** returns move *'res_moves' which constitute all the available options for the knight
  * located in location 'from' on board 'a_board'. */
move *get_n_moves(char a_board[BOARD_SIZE][BOARD_SIZE], location *from)
{
	PRINTD("get_n_moves: start  \n")
	move *res_moves = NULL;
	char a_board_copy[BOARD_SIZE][BOARD_SIZE];
	char piece = a_board[from->column][from->row];
	location opt_locs[8];
	move *opt_moves[8];
	for (int i = 0; i < 8; i++) opt_moves[i] = NULL; // initialize array to NULL
	opt_locs[0].row = from->row + 1;		//| | |*|
	opt_locs[0].column = from->column + 2;  //|n|
	opt_locs[1].row = from->row + 1;			//|*| | |
	opt_locs[1].column = from->column - 2;	    //    |n|
	opt_locs[2].row = from->row + 2;		 
	opt_locs[2].column = from->column + 1;
	opt_locs[3].row = from->row + 2;	
	opt_locs[3].column = from->column - 1;
	opt_locs[4].row = from->row - 1;	
	opt_locs[4].column = from->column + 2;
	opt_locs[5].row = from->row - 1;
	opt_locs[5].column = from->column + 2;
	opt_locs[6].row = from->row - 2;	
	opt_locs[6].column = from->column + 1;
	opt_locs[7].row = from->row - 2;	
	opt_locs[7].column = from->column - 1;
	DO_DEBUG
	(
		printf("get_n_moves: creat all opt_locs  - \nopt_locs1 row:%d ,column:%d \nopt_locs2 row:%d ,column:%d \nopt_locs3 row:%d ,column:%d \n",opt_locs[0].row,opt_locs[0].column,opt_locs[1].row,opt_locs[1].column,opt_locs[2].row,opt_locs[2].column);
		fflush(stdout);
	)
	
	for (int i = 0; i < 8; i++ )
	{
		if ( is_legal_location(opt_locs[i]) && !IS_SAME_COLOR(piece,a_board[opt_locs[i].column][opt_locs[i].row]) ) 
		{		
			opt_moves[i] = create_move(from->row, from->column, opt_locs[i].row, opt_locs[i].column);
			memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
			do_move(a_board_copy, opt_moves[i]);
			if (is_check(a_board_copy, IS_WHITE(piece)))
			{
				free_move(opt_moves[i]);
				opt_moves[i] = NULL;
			}
		}
	}
	PRINTD("get_n_moves: moves created\n")
	for (int i = 0 ; i < 8 ; i++)
	{
		if (opt_moves[i] == NULL) continue;
		if (res_moves==NULL) res_moves = opt_moves[i];
		else
		{
			opt_moves[i]->next = res_moves;
			res_moves = opt_moves[i];
		}
	}
	PRINTD("get_n_moves: done\n")
	return res_moves;
}

/** returns move *'res_moves' which constitute all the available options for the king
  * located in location 'from' on board 'a_board'. */
move *get_k_moves(char a_board[BOARD_SIZE][BOARD_SIZE], location *from)
{
	move *res_moves = NULL;
	char a_board_copy[BOARD_SIZE][BOARD_SIZE];
	char piece = a_board[from->column][from->row];
	location opt_locs[8];
	move *opt_moves[8];
	for (int i = 0; i < 8; i++) opt_moves[i] = NULL; // initialize array to NULL
	opt_locs[0].row = from->row + 1;		
	opt_locs[0].column = from->column;  
	opt_locs[1].row = from->row + 1;			
	opt_locs[1].column = from->column + 1;	   
	opt_locs[2].row = from->row + 1;		 
	opt_locs[2].column = from->column - 1;
	opt_locs[3].row = from->row;	
	opt_locs[3].column = from->column + 1;
	opt_locs[4].row = from->row;	
	opt_locs[4].column = from->column - 1;
	opt_locs[5].row = from->row - 1;
	opt_locs[5].column = from->column;
	opt_locs[6].row = from->row - 1;	
	opt_locs[6].column = from->column + 1;
	opt_locs[7].row = from->row - 1;	
	opt_locs[7].column = from->column - 1;
	
	for (int i = 0; i < 8; i++ )
	{
		if ( is_legal_location(opt_locs[i]) && !IS_SAME_COLOR(piece,a_board[opt_locs[i].column][opt_locs[i].row]) ) 
		{		
			opt_moves[i] = create_move(from->row, from->column, opt_locs[i].row, opt_locs[i].column);
			memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
			do_move(a_board_copy, opt_moves[i]);
			if (is_check(a_board_copy, IS_WHITE(piece)))
			{
				free_move(opt_moves[i]);
				opt_moves[i] = NULL;
			}
		}
	}
	for (int i = 0 ; i < 8 ; i++)
	{
		if (opt_moves[i] == NULL) continue;
		if (res_moves==NULL) res_moves = opt_moves[i];
		else
		{
			opt_moves[i]->next = res_moves;
			res_moves = opt_moves[i];
		}
	}
	return res_moves;
	
}

/** return a move pointer which constitute the concatenated move of m1 & m2.
  * if both m1 and m2 are NULL returns NULL. */
move *link_moves(move *m1, move *m2)
{
	move *tmp;
	if (m1 == NULL ) return m2;
	if (m2 == NULL ) return m1;
	else
	{
		tmp = m1;
		while(tmp->next != NULL) tmp = tmp->next;
		tmp->next = m2;
	}
	return m1;
}

/** performs 'user_move' on 'a_board'.
  * if promotion occurred updates 'a_board' accordingly. */
void do_move(char a_board[BOARD_SIZE][BOARD_SIZE], move *user_move)
{
	int empty = a_board[user_move->to->column][user_move->to->row] == EMPTY;

	char piece = a_board[user_move->from->column][user_move->from->row];
	a_board[user_move->from->column][user_move->from->row] = EMPTY;
	a_board[user_move->to->column][user_move->to->row] = ((piece == BLACK_P) || (piece == WHITE_P)) && (user_move->promote != EMPTY) ?
														user_move->promote : piece; //promote if needed (promote != EMPTY)

}

/** prints move 'm' in the requested format.
  * if promotion prints the relevant promoted piece name. */
void print_move(move *m)
{
	printf("<%c,%d> to <%c,%d>",(m->from->column+'a'),(m->from->row+1),(m->to->column+'a'),(m->to->row+1));
	int is_promotion = m->promote == EMPTY ? 0:1;
	if (is_promotion) 
	{
		char norm_piecem = IS_WHITE(m->promote) ? m->promote : m->promote - ('a'-'A');
		switch(norm_piecem)
		{
			case(WHITE_Q):
				printf(" queen");
				
				break;
			case(WHITE_B):
				printf(" bishop");
				break;
			case(WHITE_R):
				printf(" rook");
				break;
			case(WHITE_N):
				printf(" knight");
				break;
		}
	}
	printf("\n");
}

/** prints all moves in 'm' by calling 
  * "print_move" for each "->next" of 'm'. */
void print_all_moves(move *m)
{
	while(m != NULL)
	{
		print_move(m);
		m = m->next;
	}
}

/** returns the 'a_board' score according to the player ('white_player').
  * (if necessary) returns WIN/LOOSE/TIE scores accordingly. */
int score_board(char a_board[BOARD_SIZE][BOARD_SIZE], int white_player)
{
	int black_score = 0;
	int white_score = 0;
	int white_can_move = 0;
	int black_can_move = 0;
	char current = EMPTY;
	location loc;
	move *tmp = NULL;
	
	for( int row = 0 ; row < BOARD_SIZE ; row++ )
	{
		for( int column = 0 ; column < BOARD_SIZE ; column++ )
		{
			//update current location and piece
			current = a_board[column][row];
			loc.row = row;
			loc.column = column;
			//update movability
			DO_DEBUG
			(
				printf("loc in score is; \trow:%d\tcolumn:%d\n", loc.row, loc.column);
				fflush(stdout);
			)
			if ( !black_can_move && IS_BLACK(current) ) 
			{
					black_can_move = ((tmp = get_piece_moves(a_board, &loc)) != NULL) ? 1 : 0;
					free_move(tmp);
					tmp = NULL;
			}
				
			if ( !white_can_move && IS_WHITE(current) )
			{
					white_can_move = ((tmp = get_piece_moves(a_board, &loc)) != NULL) ? 1 : 0;
					free_move(tmp);
					tmp = NULL;
			}
			//update score
			switch(current)
			{
				case (EMPTY):
					break;
				case (BLACK_P):
					black_score++;
					break;
				case (BLACK_N):
					black_score += 3; 
					break;
				case (BLACK_B):
					black_score += 3; 
					break;
				case (BLACK_R):
					black_score += 5; 
					break;
				case (BLACK_Q):
					black_score += 9; 
					break;
				case (BLACK_K):
					black_score += 400; 
					break;
				case (WHITE_P):
					white_score++;
					break;
				case (WHITE_N):
					white_score += 3; 
					break;
				case (WHITE_B):
					white_score += 3; 
					break;
				case (WHITE_R):
					white_score += 5; 
					break;
				case (WHITE_Q):
					white_score += 9; 
					break;
				case (WHITE_K):
					white_score += 400; 
					break;
			}
		}
	}
	//check for tie
	if ( white_player && !is_check(a_board, white_player) && !white_can_move ) return TIE_SCORE; 
	if ( !white_player && !is_check(a_board, white_player) && !black_can_move ) return TIE_SCORE; 
	//check for mate
	if ( white_player && is_check(a_board, white_player) && !white_can_move ) return LOOSE_SCORE; 
	if ( !white_player && is_check(a_board, white_player) && !black_can_move ) return LOOSE_SCORE; 
	if ( white_player && is_check(a_board, !white_player) && !black_can_move ) return WIN_SCORE; 
	if ( !white_player && is_check(a_board, !white_player) && !white_can_move ) return WIN_SCORE; 

	return white_player ? white_score - black_score : black_score - white_score;
}

/** returns 1 if 'color' (1: white, 0: black) 
  * king is threatened, else returns 0. */
int is_check(char a_board[BOARD_SIZE][BOARD_SIZE], int color)
{
	PRINTD("is_check: start  \n")
	int kings_row;
	int kings_column;
	int direction = color ? 1:-1;
	location enemy_loc;
	for( int row = 0 ; row < BOARD_SIZE ; row++ )
	{
		for( int column = 0 ; column < BOARD_SIZE ; column++ )
		{
			if(color && a_board[column][row] == WHITE_K)
			{
				kings_row = row;
				kings_column = column;
			}
			else if(!color && a_board[column][row] == BLACK_K)
			{
				kings_row = row;
				kings_column = column;
			}
		}
	}
	// checks if king is threatened by pawn
	enemy_loc.row =  kings_row + direction;
	enemy_loc.column = kings_column-1;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_P ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_P ))) 
		return 1;
	enemy_loc.row =  kings_row + direction;
	enemy_loc.column = kings_column + 1;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_P ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_P ))) 
		return 1;
	
	// checks if king is threatened by king 
	enemy_loc.row =  kings_row + 1;
	enemy_loc.column = kings_column;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_K ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_K ))) 
		return 1;
	enemy_loc.row =  kings_row + 1;
	enemy_loc.column = kings_column + 1;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_K ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_K ))) 
		return 1;
	enemy_loc.row =  kings_row + 1;
	enemy_loc.column = kings_column - 1;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_K ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_K ))) 
		return 1;
	enemy_loc.row =  kings_row - 1;
	enemy_loc.column = kings_column + 1;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_K ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_K ))) 
		return 1;
	enemy_loc.row =  kings_row - 1;
	enemy_loc.column = kings_column;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_K ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_K ))) 
		return 1;
	enemy_loc.row =  kings_row - 1;
	enemy_loc.column = kings_column - 1;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_K ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_K ))) 
		return 1;
	enemy_loc.row =  kings_row;
	enemy_loc.column = kings_column - 1;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_K ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_K ))) 
		return 1;
	enemy_loc.row =  kings_row;
	enemy_loc.column = kings_column + 1;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_K ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_K ))) 
		return 1;
	
	
	// checks if king is threatened by Knight
	enemy_loc.row =  kings_row + 2;
	enemy_loc.column = kings_column - 1;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_N ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_N ))) 
		return 1;
	enemy_loc.row =  kings_row + 2;
	enemy_loc.column = kings_column + 1;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_N ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_N ))) 
		return 1;
	enemy_loc.row =  kings_row - 2;
	enemy_loc.column = kings_column - 1;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_N ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_N ))) 
		return 1;
	enemy_loc.row =  kings_row - 2;
	enemy_loc.column = kings_column + 1;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_N ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_N ))) 
		return 1;
	enemy_loc.row =  kings_row - 1;
	enemy_loc.column = kings_column + 2;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_N ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_N ))) 
		return 1;
	enemy_loc.row =  kings_row + 1;
	enemy_loc.column = kings_column + 2;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_N ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_N ))) 
		return 1;
	enemy_loc.row =  kings_row - 1;
	enemy_loc.column = kings_column - 2;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_N ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_N ))) 
		return 1;
	enemy_loc.row =  kings_row + 1;
	enemy_loc.column = kings_column - 2;
	if (is_legal_location(enemy_loc) && 
		((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_N ) || 
		(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_N ))) 
		return 1;
		
	// checks if king is threatened by Rook, Queen, Bishop
	int left_up_first_seen = 0;
	int left_down_first_seen = 0;
	int right_up_first_seen = 0;
	int right_down_first_seen = 0;
	int up_first_seen = 0;
	int down_first_seen = 0;
	int right_first_seen = 0;
	int left_first_seen = 0;
	
	for (int i = 1 ; i < BOARD_SIZE ; i++)
	{
		if( !up_first_seen )
		{
			enemy_loc.row =  kings_row + i;
			enemy_loc.column = kings_column;
			if ( is_legal_location(enemy_loc) )
			{				
				if ( (color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_R ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_R )||
				(color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_Q ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_Q )) 
				return 1;
				
				if ( a_board[enemy_loc.column][enemy_loc.row] != EMPTY ) up_first_seen = 1;
			}
		}
		if(!down_first_seen)
		{
			enemy_loc.row =  kings_row - i;
			enemy_loc.column = kings_column;
			if (is_legal_location(enemy_loc))
			{
				if ((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_R ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_R )||
				(color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_Q ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_Q ))
				return 1;
			
				if ( a_board[enemy_loc.column][enemy_loc.row] != EMPTY ) down_first_seen = 1;
			}
		}
		if(!right_first_seen)
		{
			enemy_loc.row =  kings_row;
			enemy_loc.column = kings_column + i;
			if (is_legal_location(enemy_loc))
			{				
				if ((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_R ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_R )||
				(color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_Q ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_Q ))
				return 1;
			
				if ( a_board[enemy_loc.column][enemy_loc.row] != EMPTY )right_first_seen = 1;
			}
		}
		if(!left_first_seen)
		{
			enemy_loc.row =  kings_row;
			enemy_loc.column = kings_column - i;
			if ( is_legal_location(enemy_loc) )
			{
				if ((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_R ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_R )||
				(color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_Q ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_Q ))
				return 1;
				
				if ( a_board[enemy_loc.column][enemy_loc.row] != EMPTY ) left_first_seen = 1;
			}
		}
		
		if(!left_up_first_seen)
		{
			enemy_loc.row =  kings_row - i;
			enemy_loc.column = kings_column - i;
			if ( is_legal_location(enemy_loc) ) 
			{
				if ((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_B ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_B )||
				(color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_Q ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_Q ))
				return 1;
			
				if ( a_board[enemy_loc.column][enemy_loc.row] != EMPTY ) left_up_first_seen = 1;
			}
		}
		if(!left_down_first_seen)
		{
			enemy_loc.row =  kings_row + i;
			enemy_loc.column = kings_column + i;
			if ( is_legal_location(enemy_loc) )
			{
				if ((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_B ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_B )||
				(color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_Q ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_Q ))
				return 1;
			
				if ( a_board[enemy_loc.column][enemy_loc.row] != EMPTY ) left_down_first_seen = 1;
			}
		}
		if(!right_up_first_seen)
		{
			enemy_loc.row =  kings_row - i;
			enemy_loc.column = kings_column + i;
			if (is_legal_location(enemy_loc) )
			{
				if ((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_B ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_B )||
				(color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_Q ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_Q ))
				return 1;
			
				if ( a_board[enemy_loc.column][enemy_loc.row] != EMPTY ) right_up_first_seen = 1;
			}
		}
		if(!right_down_first_seen)
		{
			enemy_loc.row =  kings_row + i;
			enemy_loc.column = kings_column - i;
			if (is_legal_location(enemy_loc) )
			{
				if ((color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_B ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_B )||
				(color && a_board[enemy_loc.column][enemy_loc.row] == BLACK_Q ) || 
				(!color && a_board[enemy_loc.column][enemy_loc.row] == WHITE_Q ))
				return 1;
				
				if ( a_board[enemy_loc.column][enemy_loc.row] != EMPTY ) right_down_first_seen = 1;
			}
		}
	}
	PRINTD("is_check: done  \n")
	return 0; //all is good no check
}

/** returns an int which represent the game status 
  * according to the board (uses "score_board()"). */
int game_over(char board[BOARD_SIZE][BOARD_SIZE])
{
	int current_score = score_board(board, WHITE_TURN);
	if ( current_score == TIE_SCORE ) return 1;
	if ( current_score == WIN_SCORE ) return 2;
	if ( current_score == LOOSE_SCORE ) return 3;
	return 0;
}

/** declares the winner (or tie) according to GAME_STATUS. */
void declare_winner(void)
{
	if (GAME_STATUS == 1) print_message(DECLARE_TIE);
	if (GAME_STATUS == 2) print_message(DECLARE_WINNER(WHITE_TURN));
	if (GAME_STATUS == 3) print_message(DECLARE_WINNER(!WHITE_TURN));
}

/** plays the computer's turn. 
  * uses "get_rand_move_minmax()" (dynamically allocates memory) 
  * and "do_move()". */
void play_computer_turn(char board[BOARD_SIZE][BOARD_SIZE])
{
	move *comuter_moves = get_rand_move_minmax(board);
	sprintf(info_message,"%c%d => %c%d",('H'-comuter_moves->from->column),(8-comuter_moves->from->row),('H'-comuter_moves->to->column),(8-comuter_moves->to->row));
	lcd_send_string (info_message, 2);
	HAL_Delay(10);

	perform_move(board,comuter_moves);
	//printf("Computer: move ");
	//print_move(comuter_moves);
	// print_board(board,pretty_board);
	free_move(comuter_moves);
}  
