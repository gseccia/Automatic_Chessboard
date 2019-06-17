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
} current_status;

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
char pretty_board[1000];
char info_message[16] = "";
char error_message[16];
char end_message[16];
Axis_manager* axis_manager;
magnetic_grid_manager* grid_manager;
menu_manager* menu_man;
int menu_ch=0;
int counter = 10;
int single_variation = 1;
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
  /*MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();*/
  /* USER CODE BEGIN 2 */


	axis_manager = axis_manager_init(&htim2,100,GPIOB,GPIO_PIN_1,GPIOC,GPIO_PIN_8,1,GPIOB,GPIO_PIN_2,GPIOC,GPIO_PIN_5,1,GPIOB,GPIO_PIN_13,GPIOB,GPIO_PIN_14);
	/*grid_manager = init_magnetic_grid();
	menu_man = menu_manager_init(hadc1);

	HAL_ADC_Start_DMA(&hadc1,menu_man->raw_values,2);

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

	current_status = init;
	enum status prev_stat = init;
	init_board(board);

    show_menu(menu_man,0,0);

    axis_manager_reset_position(axis_manager);

    move_n_steps(&(axis_manager->x_stepper),500,BACKWARD);
    move_n_steps(&(axis_manager->y_stepper),500,BACKWARD);*/
    //SERVO_HOOK_on(&(axis_manager->hook));
  //print_board(board,pretty_board); //prints only if in console mode
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  move_n_steps(&axis_manager->x_stepper,500,BACKWARD);
	  HAL_Delay(10000);
	  move_n_steps(&axis_manager->y_stepper,500,BACKWARD);
	  HAL_Delay(10000);


	  move_n_steps(&axis_manager->x_stepper,500,FORWARD);
	  HAL_Delay(10000);

	  move_n_steps(&axis_manager->y_stepper,500,FORWARD);
	  HAL_Delay(10000);


    /* USER CODE BEGIN 3 */
	 /* if(prev_stat != current_status){
	  		  lcd_clear();
	  		  HAL_Delay(10);
	  		  prev_stat = current_status;
	  		  if(current_status == init){
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
		HAL_Delay(10);
		play_computer_turn(board);
		lcd_send_string (info_message, 2);
		HAL_Delay(10000);

		read_magnetic_grid(grid_manager,0);
		update_magnetic_grid(grid_manager);

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

		single_variation = 1;
		current_status = player_control;
		htim3.Init.Prescaler = 15999;
		htim3.Init.Period = 999;
		HAL_TIM_Base_Start_IT(&htim3);

		WHITE_TURN = (WHITE_TURN + 1)%2;

	}
	else if(current_status == choose_piece){
		change_choice(menu_man);
	}
	else if(current_status == error){
		sprintf(tmp,"%2d",counter);
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
	}*/

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
		switch(current_status){
			case init:
				if(menu_man->current_menu == 1){
					if(settings_input_manager() == -1){
						next_state = init;
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
				if(player_input_game_manager() == 0){
					next_state = elaboration;
					WHITE_TURN = (WHITE_TURN + 1)%2;
					HAL_TIM_Base_Stop_IT(&htim3);
					 // if(current_status == choose_piece)
				}
				else {
					HAL_TIM_Base_Stop_IT(&htim3);
					next_state = error;
					htim3.Init.Prescaler = 15999;
					htim3.Init.Period = 999;
					HAL_TIM_Base_Start_IT(&htim3);
				}
				break;
			case choose_piece:
				set_invalid_menu(menu_man,2);
				// make promote, go error or player_control
				break;
			case error:
				HAL_TIM_Base_Stop_IT(&htim3);
				counter = 10;
				single_variation = 1;

				read_magnetic_grid(grid_manager, 0);
				if(check_restoring(grid_manager)){
					next_state = player_control;
					htim3.Init.Prescaler = 15999;
					htim3.Init.Period = 99; //Check 100ms
				}
				else{
					next_state = error;
				}
				HAL_TIM_Base_Start_IT(&htim3);


				break;
			case end_game:
				next_state = init;

				set_valid_menu(menu_man,0);
				set_valid_menu(menu_man,1);

				init_board(board);
			    counter = 10;

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
