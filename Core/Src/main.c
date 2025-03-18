/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
int read_encoder (uint8_t  ,uint8_t);
float get_grados(uint16_t);
//void send_grados_to_host(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define max_grados 2400
#define min_grados -2400
#define initial_grados  400.0
#define initial_value  0U

int a = 0;
int b = 0;
int c = 0;
int suma = initial_value;
int last_c = 0;
int16_t xd = initial_value;
float xd_grados = 0.0f;
float grados_wheel = 0.0f;
int16_t last_xd = initial_value;
uint16_t xd_grados_decimal = 0;
extern USBD_HandleTypeDef hUsbDeviceFS;

typedef struct {
   uint8_t buttons;
   int8_t HID_Report[2];
   //uint16_t HID_Report;
   //uint8_t HID_Report[4];
}joystickReport ;

joystickReport joystickReportCONtainer;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	uint8_t K1 = 0;
	//uint8_t K0 = 0;
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
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      /* USER CODE END WHILE */
       a = HAL_GPIO_ReadPin(GPIOE, A_Read_Pin);
       b = HAL_GPIO_ReadPin(GPIOE, B_Read_Pin);

       //K0 = !HAL_GPIO_ReadPin(GPIOE, K0_Button_Pin);
       K1 = !HAL_GPIO_ReadPin(GPIOE, K1_Button_Pin);
       joystickReportCONtainer.buttons =  K1;

       xd = read_encoder(a,b);
       xd_grados_decimal = (int16_t)get_grados(xd);
       int8_t MSB = (int8_t)((xd_grados_decimal >> 8) & 0xFF);
       int8_t LSB = (int8_t)(xd_grados_decimal & 0xFF);

       // Descomponer xd_grados_decimal en dos bytes (uint8_t)
       joystickReportCONtainer.HID_Report[0] = LSB;//MSB;  // MSB
       joystickReportCONtainer.HID_Report[1] = MSB;//LSB;  // LSB

       //joystickReportCONtainer.HID_Report = LSB;  // LSB

       // Enviar el reporte HID al host
       USBD_HID_SendReport(&hUsbDeviceFS,(int8_t*)& joystickReportCONtainer, sizeof( joystickReport ));
       /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}


//void send_grados_to_host(void ) {
//    // Empaquetar el valor de grados como float
//      // 4 bytes para el valor flotante
//
//    // Convertir el valor de grados (float) en su representación de 4 bytes (float -> uint8_t[])
//    union {
//        float grados;
//        uint8_t bytes[4];
//    } float_to_bytes;
//
//    float_to_bytes.grados = xd_grados;
//
//    // Copiar los 4 bytes en el reporte HID
//    for (int i = 0; i < 4; i++) {
//    	joystickReportCONtainer.HID_Report[i] = float_to_bytes.bytes[i];
//    }
//
//
//}

float get_grados(uint16_t encoder_inc)
{
	float gain_grado = 0.0f;
	int8_t coe = 0;
	coe = encoder_inc - last_xd;
	if(coe != 0)
	{
		if(coe>1)
		{

		}
		gain_grado =  (360.0f/2400.0f);

	}
	else
	{
		/*nothing*/
		gain_grado = 0;
	}

	grados_wheel = grados_wheel + (gain_grado*coe);

	grados_wheel = (grados_wheel <= min_grados) ? min_grados : grados_wheel;
	grados_wheel = (grados_wheel >= max_grados) ? max_grados : grados_wheel;


	last_xd = encoder_inc;
	return grados_wheel;
}


int read_encoder (uint8_t a_val , uint8_t b_val)
{
	int increment_ret_val = 0;
	uint8_t c = (a_val << 1) | b_val;

    if((last_c == 0 && c == 1)||(last_c == 1 && c == 3)||(last_c == 2 && c == 0)||(last_c == 3 && c == 2))
    {
    	if(suma<15000)
    	{
    		suma++;
    	}
    }
    else if(last_c == c)
    {
    	/*do nothing*/
    }
    else
    {
    	if(suma>-15000)
    	{
    		suma--;
    	}
    }

    last_c = c;
	a = 0;
	b = 0;
	c = 0;
    increment_ret_val = suma;

	return increment_ret_val;
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pins : K1_Button_Pin A_Read_Pin B_Read_Pin */
  GPIO_InitStruct.Pin = K1_Button_Pin|A_Read_Pin|B_Read_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
