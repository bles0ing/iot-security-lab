#include "uart_backroot/bsp_backroot.h"
#include "main.h"
#include "led/bsp_led.h"
#include "key/bsp_key.h"
#include "dwt/bsp_dwt.h"
#include "usart/bsp_usart.h"
#include "oled/bsp_oled.h"
#include "i2c/bsp_i2c.h"

void SystemClock_Config(void);


int main(void)
{

	HAL_Init();  						// 初始化 HAL 库 

	SystemClock_Config();   // 配置系统时钟，设置为 72MHz
	
	LED_GPIO_Config();
	
	KEY_GPIO_Init();
	
	MX_I2C1_Init();
	
	Debug_UART_Init();			// 初始化调试串口，用于打印调试信息
	
	DWT_Init();							// 启动 DWT 计数器，用于精确测量程序运行时间
	
	OLED_Init();
	
	
	uint8_t rxbuf[64];
	char byte;
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, RxBuf, 128);
	OLED_ShowString_F8X16(0,0,(uint8_t*)"System OK");
	while (1)
	{//利用状态机防止CPU被困在后门植入的模块化功能选择中
		if (bk_status==1){
			Back_Root(RxBuf);
		}
		else{
			UART_BKROOT();
		}
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
