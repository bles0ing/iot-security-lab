#include "main.h"
#include "led/bsp_led.h"
#include "key/bsp_key.h"
#include "dwt/bsp_dwt.h"
#include "usart/bsp_usart.h"
#include "can/app_can.h"
#include "can/bsp_can.h"
#include "oled/bsp_oled.h"
#include "i2c/bsp_i2c.h"
#include "fonts/bsp_fonts.h"
#include <stdio.h>
#include "tim/bsp_tim.h"

void SystemClock_Config(void);

volatile uint8_t count=0;
volatile uint8_t mode=0;
volatile uint8_t status=0;
int main(void)
{

	HAL_Init();  						// 初始化 HAL 库 

	SystemClock_Config();   // 配置系统时钟，设置为 72MHz
	
  LED_GPIO_Config();			// 配置 LED 相关 GPIO 引脚，用于状态指示
	
  KEY_GPIO_Init();		// 配置 KEY1/KEY2 等输入引脚
	
	MX_TIM2_Init();
	
	Debug_UART_Init();			// 初始化调试串口，用于打印调试信息
	
	DWT_Init();							// 启动 DWT 计数器，用于精确测量程序运行时间
	CAN_Init();  					 	// 配置 CAN 波特率、模式及滤波器，开启接收中断
  MX_I2C1_Init();
	
	OLED_Init();
	HAL_TIM_Base_Start_IT(&htim2);
	
	HAL_Delay(20);
	OLED_ShowString_F8X16(0,6,(uint8_t*)"DATA:");
	

	while (1)
	{
		//CAN_Task(); // 执行 CAN 任务函数，扫描按键按下并发送数据
    

	}
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2){
		if(count>=60){
			mode=1;
			count=0;
		}
		else {
			mode=0;
			count=0;
		}
		status=1;
	}
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
