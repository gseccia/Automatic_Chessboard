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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "chess_engine/Chess.h"
#include "axis_manager_stm32f4/axis_manager.h"
#include "magnetic_grid_interface_stm32/API_magneticGrid.h"
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
int GAME = 1;
int SETTINGS = 0;
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
	int PLAYER_WHITE = 1;
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
Axis_manager* axis_manager;
magnetic_grid_manager* grid_manager;
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
  /* USER CODE BEGIN 2 */

	axis_manager = axis_manager_init(&htim2,100,GPIOB,GPIO_PIN_1,GPIOC,GPIO_PIN_8,1,GPIOB,GPIO_PIN_2,GPIOC,GPIO_PIN_5,1,GPIOB,GPIO_PIN_13,GPIOB,GPIO_PIN_14);
	grid_manager = init_magnetic_grid();


  init_board(board);
  print_board(board,pretty_board); //prints only if in console mode

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  if ( (SETTINGS) || (TWO_PLAYERS_MODE) || (WHITE_TURN&&PLAYER_WHITE) || ((!WHITE_TURN)&&(!PLAYER_WHITE)) )
	    	  	    		{
	    	  	    			if ( GAME && (GAME_STATUS = game_over(board)) ) GAME = 0;
	    	  	    			if( !GAME && !SETTINGS ) // end the game
	    	  	    			{
	    	  	    				declare_winner();
	    	  	    				quit();
	    	  	    			}
	    	  	    			/*if( GAME && !repeat && is_check(board, WHITE_TURN)) print_message(CHECK)
	    	  	    			if ( GAME )	print_message(ENTER_MOVE(WHITE_TURN));
	    	  	    			if ( SETTINGS ) print_message(ENTER_SETTINGS);
	    	  	    			read_input(input);
	    	  	    			if( strcmp(input,"") == 0 ) continue; // verify input isn't empty.
	    	  	    			if ( strcmp(input, "quit") == 0 ) quit();*/
	    	  	    		}
	    	  	    		if ( SETTINGS )
	    	  	    		{
	    	  	    			//parse_input_settings(input);
	    	  	    			DO_DEBUG
	    	  	    			(
	    	  	    				printf("user color is: %d\n", PLAYER_WHITE);
	    	  	    				printf("minmax depth is: %d\n", MINIMAX_DEPTH);
	    	  	    				printf("next player is: %d\n", WHITE_TURN);
	    	  	    				printf("game mode is: %d\n", TWO_PLAYERS_MODE);
	    	  	    			)
	    	  	    		}
	    	  	    		else if ( GAME )
	    	  	    		{

	    	  	    			if ( (TWO_PLAYERS_MODE) || (WHITE_TURN&&PLAYER_WHITE) || ((!WHITE_TURN)&&(!PLAYER_WHITE)) ) //user's turn
	    	  	    			{
	    	  	    				if(player_input_game_manager() == 0 ) //'1' if user's input was wrong in some way or need another input
	    	  	    					WHITE_TURN = (WHITE_TURN + 1)%2;
	    	  	    				//ANTONIO Mossa fatta dall'utente
	    	  	    			}
	    	  	    			else play_computer_turn(board); //computer's turn

	    	  	    			print_board(board,pretty_board);
	    	  	    			// WHITE_TURN = (WHITE_TURN + 1)%2;
	    	  	    		}
    /* USER CODE BEGIN 3 */
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
