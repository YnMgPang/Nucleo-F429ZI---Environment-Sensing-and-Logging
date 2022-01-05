/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */


/*	Program flow
 *	1 - initialize -> ~15s
 *  2 - enter infinite loop
 *  3 - go into sleep
 *  4 - wake up after 1s and do task
 *  5 - back to 3
 */


/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "lwip.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


#include "SVM30.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define i2cDevice hi2c1
#define remote_port 61111
#define payload_size 30
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern struct netif gnetif;
char buffer_UDP[40] = {0};							//UDP packet buffer
struct pbuf *pbufPtr = NULL;						//packet buffer pointer
struct udp_pcb *pcbPtr = NULL;						//pcb pointer
struct ip4_addr ipaddr_remote_struct;				//message destination

float RH = 99;										//variable for each type of reading
float Temp = 99;									//
uint16_t TVOC = 999;								//
uint16_t CO2eq = 999;								//
uint8_t buffer_Sensor[8];							//buffer for I2C comm. w/ sensor


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void doTasks(struct pbuf *pbufPtr);					//per timer interrupt
													//send calibration command
													//get measurements
													//send data through UDP
void sendDataUDP(struct udp_pcb *pcbPtr, struct pbuf *pbufPtr, char *buffer, uint8_t size);		//load buffer content onto pbuf
																								//send data
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
  MX_I2C1_Init();
  MX_TIM14_Init();
  MX_LWIP_Init();
  /* USER CODE BEGIN 2 */

  if (netif_is_link_up(&gnetif))						//check ethernet cable connection
  {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);	//connected -> red
  }

  IP4_ADDR(&ipaddr_remote_struct, 10, 183, 93, 17);		//set up remote ip

  pcbPtr = udp_new();
  pbufPtr = pbuf_alloc(PBUF_TRANSPORT, payload_size, PBUF_RAM);

  udp_connect(pcbPtr, &ipaddr_remote_struct, remote_port);	//connect to remote ip
  	  	  	  	  	  	  	  	  	  	  	  	  	  	//random port number
  initAQ(buffer_Sensor, i2cDevice);						//initialize air quality measurement
  HAL_Delay(15000);										//wait until sensor ready

  HAL_TIM_Base_Start_IT(&htim14);						//start timer in interrupt mode

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  MX_LWIP_Process();								//------------------------

	  HAL_SuspendTick();								//prevent SysTick-triggered wake-up
	  __WFI();											//enter sleep mode
	  	  	  	  	  	  	  	  	  	  	  	  	  	//timer interrupt triggers wake-up
	  HAL_ResumeTick();
	  HAL_GPIO_TogglePin (GPIOB, GPIO_PIN_14);			//LD3

	  doTasks(pbufPtr);
    /* USER CODE END WHILE */

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void sendDataUDP(struct udp_pcb *pcbPtr, struct pbuf *pbufPtr, char *buffer, uint8_t size){
	pbuf_take(pbufPtr, buffer, size);					//copy data to pbuf ---> change also:pbuf_alloc()
	udp_send(pcbPtr, pbufPtr);
}

void doTasks(struct pbuf *pbufPtr){

	sendMeasureAirQuality(buffer_Sensor, i2cDevice);			//sensor req. to maintain accurate measurements

	Temp = getTemp(buffer_Sensor, i2cDevice);					//get measurements
	RH = getRH(buffer_Sensor, i2cDevice);						//
	CO2eq = getCO2eq(buffer_Sensor, i2cDevice);					//
	TVOC = getTVOC(buffer_Sensor, i2cDevice);					//

	sprintf(buffer_UDP, "%.2fT %.2fRH %uC %uVO", Temp, RH, CO2eq, TVOC);	//separators -> T, RH, C, VO
	sendDataUDP(pcbPtr, pbufPtr, buffer_UDP, payload_size);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htim14 )										//check timer version
  {
	  HAL_GPIO_TogglePin (GPIOB, GPIO_PIN_7);				//LD2
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
