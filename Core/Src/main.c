/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "st7789.h"
#include "math.h"
#include "stdlib.h" // rand() utiliza esta lib


//#include "string.h"
#include "stdio.h" // Utilizado pelo snprinf

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

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef handle_GPDMA1_Channel0;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
volatile uint8_t spi_tx_done = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_GPDMA1_Init(void);
static void MX_SPI1_Init(void);
static void MX_ICACHE_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_GPDMA1_Init();
  MX_SPI1_Init();
  MX_ICACHE_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(BLK_GPIO_Port, BLK_Pin, 1);
  ST7789_Init();
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint32_t inicio;
  uint32_t tempo_gasto;
  uint32_t periodo_ms = 10;   // aproximação de 40 Hz

  HAL_GPIO_WritePin(TEMPO_GPIO_Port, TEMPO_Pin, 0);

  ST7789_Fill_Color(BLACK);
  ST7789_WriteString(55, 5, "Snake game", Font_11x18, YELLOW, BLACK);
  ST7789_DrawLine(0, 25, 239, 25, WHITE);
  //ST7789_DrawImage(0, 0, 128, 128, (uint16_t *)saber); // knky

  uint8_t q_x = 10;
  uint8_t q_y = 120;
  uint8_t q_dir = 0; // 0 -> frente, 1 <- traz, 2 ^ cima e 3 v baixo
  char buf[15];
  uint16_t enc_anterior, enc_atual;
  uint8_t tamanho= 1;
  uint8_t corpo[2][100];
  uint8_t loops = 0;
  uint8_t posx_comida, posy_comida;
  uint8_t comeu = 1;
  uint8_t score = 0;
  uint8_t col_corpo = 0;

  for ( uint8_t i = 0; i < 10; i++) {
 	  corpo[0][i] = i;
      corpo[1][i] = 128;
   }
  __HAL_TIM_SET_COUNTER(&htim2, 32768);
  enc_atual = (uint16_t)(TIM2->CNT)>>2;
  enc_anterior = enc_atual;

  while (1)
  {
	  enc_atual = (uint16_t)(TIM2->CNT)>>2;
	  snprintf(buf, sizeof(buf), "Score: %d", score);
	  ST7789_WriteString(0, 230, buf, Font_7x10, YELLOW, BLACK);
	  HAL_GPIO_TogglePin(TEMPO_GPIO_Port, TEMPO_Pin);
	  inicio = HAL_GetTick();
	  if (loops > 50 && comeu) {
		  posx_comida = (uint8_t)rand()%(200) + 15;
		  posy_comida = (uint8_t)rand()%(180) + 40;

		  loops = 0;
		  comeu = 0;
		  ST7789_DrawFilledCircle(posx_comida, posy_comida, 8, WHITE);
	  }

	  if (enc_anterior != enc_atual ) { // Muda de direção
		  switch(q_dir) {
			  case 0: // Frente
				  if (enc_atual > enc_anterior) {
					  q_dir = 3; // Baixo
				  }
				  if (enc_atual < enc_anterior) {
					  q_dir = 2; // Cima
				  }
				  break;
			  case 1: // Traz
				  if (enc_atual > enc_anterior) {
					  q_dir = 2; // Baixo
				  }
				  if (enc_atual < enc_anterior) {
					  q_dir = 3; // Cima
				  }
				  break;
			  case 2: // Cima
				  if (enc_atual > enc_anterior) {
					  q_dir = 0; // Frente
				  }
				  if (enc_atual < enc_anterior) {
					  q_dir = 1; // Traz
				  }
				  break;
			  case 3: // Baixo
				  if (enc_atual > enc_anterior) {
					  q_dir = 1; // Traz
				  }
				  if (enc_atual < enc_anterior) {
					  q_dir = 0; // Frente
				  }
				  break;
	      }
		  enc_anterior = enc_atual;
	  }
	  if ((q_dir == 0) & (q_x < 239)){ // Frente
		  q_x++;
	  }
	  if ((q_dir == 1) & (q_x > 0)){   // Traz
		  q_x--;
	  }
	  if ((q_dir == 2) & (q_y > 26)){  // Cima
		  q_y--;
	  }
	  if ((q_dir == 3) & (q_y < 239)){ // Baixo
		  q_y++;
	  }

	  // Procula colisão do corpo
	  for ( uint8_t i = 0; i < 10 * tamanho; i++) {
	      if ((corpo[0][i] == q_x ) && (corpo[1][i] == q_y)) {
			  col_corpo = 1;
		  }
	  }

	  if ((q_y >= 239) || (q_y <= 26) || (q_x >= 239) || (q_x <= 0) || (col_corpo == 1)) { // Colisão nos limites da tela
		  for ( uint8_t i = 0; i < 30; i += 3) {
              ST7789_DrawCircle(q_x, q_y, i, RED);
		  }
          HAL_Delay(1000);
          ST7789_WriteString(20, 112, "Game Over!!!", Font_16x26, RED, BLACK);
          HAL_Delay(3000);
          q_x = 0;
          q_y = 120;
          ST7789_Fill_Color(BLACK);
          ST7789_WriteString(55, 5, "Snake game", Font_11x18, YELLOW, BLACK);
          ST7789_DrawLine(0, 25, 239, 25, WHITE);
          __HAL_TIM_SET_COUNTER(&htim2, 32768);
          enc_atual = (uint16_t)(TIM2->CNT)>>2;
          enc_anterior = enc_atual;
          q_dir = 0;
          score = 0;
          tamanho = 1;
          col_corpo = 0;
          comeu  = 1;
          for ( uint8_t i = 0; i < 10; i++) {
         	  corpo[0][i] = i;
              corpo[1][i] = 128;
          }
	  }
	  if ((q_y >= posy_comida-8) && (q_y <= posy_comida +8) && // Comeu
	  (q_x >= posx_comida-8) && (q_x <= posx_comida +8) && (comeu == 0)) {
		  tamanho++;
		  score += 10;
		  comeu = 1;
		  snprintf(buf, sizeof(buf), "Score: %d", score);
		  ST7789_WriteString(0, 230, buf, Font_7x10, YELLOW, BLACK);
		  if (tamanho < 11) {
		      for ( uint8_t i = 0; i < 10; i++) {
		          corpo[0][i + ((tamanho -1) * 10)] = q_x;
		          corpo[1][i + ((tamanho -1) * 10)] = q_y;
		      }
		      ST7789_DrawFilledCircle(posx_comida, posy_comida, 8, BLACK);
		  } else {
	          ST7789_WriteString(40, 112, "You Won!!!", Font_16x26, RED, BLACK);

	          HAL_Delay(3000);
	          q_x = 0;
	          q_y = 120;
	          ST7789_Fill_Color(BLACK);
	          ST7789_WriteString(55, 5, "Snake game", Font_11x18, YELLOW, BLACK);
	          ST7789_DrawLine(0, 25, 239, 25, WHITE);
	          __HAL_TIM_SET_COUNTER(&htim2, 32768);
	          enc_atual = (uint16_t)(TIM2->CNT)>>2;
	          enc_anterior = enc_atual;
	          q_dir = 0;
	          score = 0;
	          tamanho = 1;
	          col_corpo = 0;
	          comeu  = 1;
	          for ( uint8_t i = 0; i < 10; i++) {
	         	  corpo[0][i] = i;
	              corpo[1][i] = 128;
	          }

		  }
      }

	  // Desenha a cobra

	  //for ( uint8_t i = 0; i < 10 * tamanho; i++) {
      //    ST7789_DrawPixel(corpo[0][i], corpo[1][i], WHITE);
	  //}
	  ST7789_DrawPixel(corpo[0][10 * tamanho - 1], corpo[1][10 * tamanho - 1], WHITE);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      tempo_gasto = HAL_GetTick() - inicio;
      if (tempo_gasto < periodo_ms) {
          HAL_Delay(periodo_ms - tempo_gasto);
      }

      loops++;

      //for ( uint8_t i = 0; i < 10 * tamanho; i++) {
      //    ST7789_DrawPixel(corpo[0][i], corpo[1][i], BLACK);
	  //}
      ST7789_DrawPixel(corpo[0][0], corpo[1][0], BLACK);
	  for ( uint8_t i = 0; i < 10 * tamanho; i++) {
		  corpo[0][i] = corpo[0][i+1];
		  corpo[1][i] = corpo[1][i+1];
	  }
	  corpo[0][10 * tamanho - 1] = q_x;
	  corpo[1][10 * tamanho - 1] = q_y;

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 62;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 4096;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the programming delay
  */
  __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CKPER;
  PeriphClkInitStruct.CkperClockSelection = RCC_CLKPSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPDMA1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPDMA1_Init(void)
{

  /* USER CODE BEGIN GPDMA1_Init 0 */

  /* USER CODE END GPDMA1_Init 0 */

  /* Peripheral clock enable */
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  /* GPDMA1 interrupt Init */
    HAL_NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */

  /* USER CODE END GPDMA1_Init 1 */
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* USER CODE END GPDMA1_Init 2 */

}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x7;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi1.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi1.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 10;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 10;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, BLK_Pin|CS_Pin|DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, RST_Pin|TEMPO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PSH_Pin TRB_Pin TRA_Pin */
  GPIO_InitStruct.Pin = PSH_Pin|TRB_Pin|TRA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : BLK_Pin */
  GPIO_InitStruct.Pin = BLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BLK_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_Pin DC_Pin */
  GPIO_InitStruct.Pin = CS_Pin|DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : RST_Pin TEMPO_Pin */
  GPIO_InitStruct.Pin = RST_Pin|TEMPO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : KO_Pin */
  GPIO_InitStruct.Pin = KO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KO_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &ST7789_SPI_PORT)
    {
        spi_tx_done = 1;
    }
}
/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};
  MPU_Attributes_InitTypeDef MPU_AttributesInit = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region 0 and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x08FFF000;
  MPU_InitStruct.LimitAddress = 0x08FFFFFF;
  MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
  MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RO;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Attribute 0 and the memory to be protected
  */
  MPU_AttributesInit.Number = MPU_ATTRIBUTES_NUMBER0;
  MPU_AttributesInit.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);

  HAL_MPU_ConfigMemoryAttributes(&MPU_AttributesInit);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
