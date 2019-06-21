/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "chess_engine/Chess.h"
#include "axis_manager_stm32f4/axis_manager.h"
#include "magnetic_grid_interface_stm32/API_magneticGrid.h"
#include "choice_stm32f4/scelta.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int MINIMAX_DEPTH = 1;

enum status{
	init=0,
	player_control=1,
	elaboration = 2,
	choose_piece  = 3,
	error = 4,
	end_game = 5
} current_status,prev_stat,origin_error;

int GAME = 0;
int SETTINGS = 1;
/** holds the current game state:
	  *	0 - game isn't over
	  * 1 - game is over with TIE
	  * 2 - game is over with WIN
	  * 3 - game is over with LOOSE */
int GAME_STATUS = 0;
/** 1 - two players mode
	  * 2(0) - player vs. AI mode */
	int TWO_PLAYERS_MODE = 0;
/** 0 - black
	  * 1 - white */
	int WHITE_TURN = 1;
/** 1 - player is white
	  * 0 - player is black */
	int PLAYER_WHITE = 0;
/** the board */

//----Game settings----//
/** 0 - no check
 *  1 - check on X player */
int CHECK_ON_WHITE = 0;
int CHECK_ON_BLACK = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char board[BOARD_SIZE][BOARD_SIZE];
char info_message[16] = "";
char error_message[16];
char end_message[16];
char solver_message[16];
char p_mex[6];
char value_ch;

Axis_manager* axis_manager;
magnetic_grid_manager* grid_manager;
menu_manager* menu_man;
int menu_ch=0;
int counter = 30;
int single_variation = 1;
int ch_i,ch_j;

int check_coherence(char board[BOARD_SIZE][BOARD_SIZE],magnetic_grid_manager* grid_manager){
	int i,j,tmp;
	for(i = 0;i < 8;i++){
		if(!PLAYER_WHITE){
			tmp = i;
			i =7-i;
		}
		for(j =0;j<8;j++){
			if((board[j][i] == EMPTY && (grid_manager->magnetic_grid)[i][j] == 1) ||
					(board[j][i] != EMPTY && (grid_manager->magnetic_grid)[i][j] == 0)){
				ch_i = i;
				ch_j = j;
				if((grid_manager->magnetic_grid)[i][j] == 0) strcpy(p_mex,"Miss");
				else if((grid_manager->magnetic_grid)[i][j] == 1) strcpy(p_mex,"Added");

				return 0;
			}
		}
		if(!PLAYER_WHITE)i =tmp;
	}
	return 1;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  	axis_manager = axis_manager_init(&htim2,100,GPIOB,GPIO_PIN_1,GPIOB,GPIO_PIN_2,3,GPIOC,GPIO_PIN_8,GPIOC,GPIO_PIN_5,3,GPIOB,GPIOB,GPIO_PIN_13,GPIO_PIN_14);
	grid_manager = init_magnetic_grid();
	menu_man = menu_manager_init(hadc1);

	HAL_ADC_Start_DMA(&hadc1,menu_man->raw_values,2);

	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);

	char tmp[MAXCHAR];
	char color_choice[2][MAXCHAR];
	strcpy(color_choice[0],"BLACK\0");
	strcpy(color_choice[1],"WHITE\0");
	char start_choice[1][MAXCHAR];
	strcpy(start_choice[0],"Press to Start\0");
	char piece_choice[4][MAXCHAR];
	strcpy(piece_choice[0],"QUEEN\0");
	strcpy(piece_choice[1],"KNIGHT\0");
	strcpy(piece_choice[2],"BISHOP\0");
	strcpy(piece_choice[3],"ROOK\0");
	int piece_ch = 0;

	createMenu(menu_man,"Choose color\0",color_choice,2,&PLAYER_WHITE,1);
	createMenu(menu_man,"Are you ready?\0",start_choice,1,NULL,1);
	createMenu(menu_man,"Piece Choose\0",piece_choice,4,&piece_ch,0);

	prev_stat = init;
	init_board(board);


	lcd_send_string("Reset Motors'",1);
	HAL_Delay(10);
	lcd_send_string("Position",2);
	HAL_Delay(10);

	axis_manager_reset_position(axis_manager);

    /* EXTI interrupt init*/
      HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(EXTI1_IRQn);

	read_magnetic_grid(grid_manager,0);
	if(!check_restoring(grid_manager)){
		current_status = error;
		htim3.Init.Prescaler = 15999;
		htim3.Init.Period = 999;
		HAL_TIM_Base_Start_IT(&htim3);
		origin_error = init;
		sprintf(error_message,"Illegal init");
		sprintf(solver_message,"Repos %c%d",'H'-ch_j,8-ch_i);
	}
	else {
		current_status = init;
	    show_menu(menu_man,0,0);
	}



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if(prev_stat != current_status){
	  		  lcd_clear();
	  		  HAL_Delay(10);
	  		  prev_stat = current_status;
	  		  if(current_status == init){
	  			if(prev_stat == end_game && !check_restoring(grid_manager)){
	  					current_status = error;
	  					htim3.Init.Prescaler = 15999;
	  					htim3.Init.Period = 999;
	  					HAL_TIM_Base_Start_IT(&htim3);
	  					origin_error = init;
	  					init_board(board);
	  					reset_magnetic_grid(grid_manager);
	  					axis_manager_reset_position(axis_manager);

	  					sprintf(error_message,"Illegal init");
	  					sprintf(solver_message,"Repos %c%d",'H'-ch_j,8-ch_i);
	  				}
	  			  show_menu(menu_man,0,0);
	  			  reset_magnetic_grid(grid_manager);
	  		  }
	  		  else if(current_status == choose_piece) show_menu(menu_man,2,0);
	  }

	if ( current_status == init )
	{
		DO_DEBUG
		(
			printf("user color is: %d\n", PLAYER_WHITE);
			printf("minmax depth is: %d\n", MINIMAX_DEPTH);
			printf("next player is: %d\n", WHITE_TURN);
			printf("game mode is: %d\n", TWO_PLAYERS_MODE);
		)
		if(menu_ch){
			next_menu(menu_man);
			menu_ch = 0;
		}
		else change_menu(menu_man);
		change_choice(menu_man);
	}
	else if ( current_status == player_control )
	{
		lcd_send_string ("Player turn\0", 1);
		lcd_send_string (info_message, 2);

		HAL_Delay(10);
	}
	else if(current_status == elaboration){
		lcd_send_string ("Elaboration\0", 1);
		play_computer_turn(board);

		read_magnetic_grid(grid_manager,0);
		if(check_coherence(board,grid_manager)){
			update_magnetic_grid(grid_manager);
			single_variation = 1;
			current_status = player_control;
			htim3.Init.Prescaler = 15999;
			htim3.Init.Period = 99;
			HAL_TIM_Base_Start_IT(&htim3);
		}
		else{
			current_status = error;
			origin_error = elaboration;
			strcpy(solver_message,info_message);
			sprintf(error_message,"%s piece",p_mex);
			htim3.Init.Prescaler = 15999;
			htim3.Init.Period = 9999;
			HAL_TIM_Base_Start_IT(&htim3);
		}


		if ( GAME && (GAME_STATUS = game_over(board)) ) {
			GAME = 0;
		}
		if( !GAME && !SETTINGS ) // end the game
		{
			// declare_winner();
			if(GAME_STATUS == 1) strcpy(end_message,"Tie match");
			else if(GAME_STATUS == 2) strcpy(end_message,"White wins");
			else if(GAME_STATUS == 3) strcpy(end_message,"Black wins");
			current_status = end_game;
		}
		if( GAME && is_check(board, WHITE_TURN)) strcpy(info_message,"CHECK");
		else strcpy(info_message,"");


		WHITE_TURN = (WHITE_TURN + 1)%2;

	}
	else if(current_status == choose_piece){
		change_choice(menu_man);
		switch(piece_ch){
			case 0:
				value_ch = (PLAYER_WHITE)? WHITE_Q:BLACK_Q;
				break;
			case 1:
				value_ch = (PLAYER_WHITE)? WHITE_N:BLACK_N;
				break;
			case 2:
				value_ch = (PLAYER_WHITE)? WHITE_B:BLACK_B;
				break;
			case 3:
				value_ch = (PLAYER_WHITE)? WHITE_R:BLACK_R;
				break;
			default:
				value_ch = (PLAYER_WHITE)? WHITE_Q:BLACK_Q;
				break;
		}
	}
	else if(current_status == error){
		sprintf(tmp,"%2d %s",counter,solver_message);
		lcd_send_string (error_message, 1);
		lcd_send_string (tmp, 2);
		HAL_Delay(10);

		if(counter <= 0){
			current_status = end_game;
			strcpy(end_message,"You lose\0");
		    HAL_TIM_Base_Stop_IT(&htim3);
		}
	}
	else if(current_status == end_game){
		lcd_send_string (end_message, 1);
		lcd_send_string ("Press to restart", 2);
		HAL_Delay(10);
	}

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// Button interrupt management, switch state
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	if(GPIO_Pin == GPIO_PIN_1 && current_status != elaboration){
		enum status next_state = current_status;
		int player_stat;
		switch(current_status){
			case init:
				if(menu_man->current_menu == 1){
					if(settings_input_manager() == -1){
						next_state = init;
						PLAYER_WHITE = 0;
						WHITE_TURN = 1;
					}
					else{
						next_state = (PLAYER_WHITE == 1)? player_control:elaboration;
						set_invalid_menu(menu_man,0);
						set_invalid_menu(menu_man,1);

						single_variation = 1;
						htim3.Init.Prescaler = 15999;
						htim3.Init.Period = 99; //Check 100ms
						HAL_TIM_Base_Start_IT(&htim3);
					}
				}
				else menu_ch = 1;
				break;
			case player_control:
				if(strcmp(info_message,"CHECK") == 0){
					strcpy(end_message,"You win!");
					next_state = end_game;
				}
				player_stat = player_input_game_manager();
				if(player_stat == 0){
					next_state = elaboration;
					WHITE_TURN = (WHITE_TURN + 1)%2;
					HAL_TIM_Base_Stop_IT(&htim3);
				}
				else if(player_stat == 2){
					next_state = choose_piece;
					WHITE_TURN = (WHITE_TURN + 1)%2;
					HAL_TIM_Base_Stop_IT(&htim3);
				}
				else{
					HAL_TIM_Base_Stop_IT(&htim3);
					next_state = error;
					origin_error = player_control;
					htim3.Init.Prescaler = 15999;
					htim3.Init.Period = 999;
					HAL_TIM_Base_Start_IT(&htim3);
				}
				break;
			case choose_piece:
				set_invalid_menu(menu_man,2);
				board[ch_i][ch_j] = value_ch;
				next_state = elaboration;
				// make promote player_control
				break;
			case error:
				HAL_TIM_Base_Stop_IT(&htim3);
				counter = 30;
				single_variation = 1;

				read_magnetic_grid(grid_manager, 0);

				int coherence =	check_coherence(board,grid_manager);
				int restoring = check_restoring(grid_manager);

				if((origin_error == elaboration || origin_error == init) && coherence){
					update_magnetic_grid(grid_manager);
					if(origin_error == init) next_state = init;
					else next_state = player_control;
				}
				else if(origin_error == player_control && restoring){
					next_state = player_control;
				}
				else if((origin_error == elaboration || origin_error == init) && !coherence){
					sprintf(solver_message,"%s %c%d",p_mex,'H'-ch_j,8-ch_i);
				}
				else if(origin_error == player_control && !restoring){
					sprintf(solver_message,"%s %c%d",p_mex,'H'-ch_j,8-ch_i);
				}

				if(next_state == player_control){
					htim3.Init.Prescaler = 15999;
					htim3.Init.Period = 99; //Check 100ms
					free(grid_manager->move_list);
					grid_manager->move_list = NULL;
				}

				HAL_TIM_Base_Start_IT(&htim3);

				break;
			case end_game:
				next_state = init;

				set_valid_menu(menu_man,0);
				set_valid_menu(menu_man,1);

				PLAYER_WHITE = 0;
				WHITE_TURN = 1;
				SETTINGS = 1;
				counter = 30;

				break;
		}
		current_status = next_state;
	}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
		if(htim == &htim3 && current_status == error) counter--;
		else if(htim == &htim3 && current_status == player_control){
			// READ STATUS
			HAL_TIM_Base_Stop_IT(htim);

			if(read_magnetic_grid(grid_manager,1)>1){
				single_variation = 0;
			}

			HAL_TIM_Base_Start_IT(htim);
		}
		else {
			HAL_NVIC_EnableIRQ(EXTI1_IRQn);
			HAL_TIM_Base_Stop_IT(htim);
		}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
