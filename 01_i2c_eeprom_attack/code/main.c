#include "i2c/bsp_i2c.h"
#include "main.h"
#include "oled/bsp_oled.h"
#include "AT24Cxx/at24cxx.h"
#include <stdio.h>

void SystemClock_Config(void);
void ByteToHexStr(uint8_t val, uint8_t*buf){
	const char hex[] = "0123456789ABCDEF";
	buf[0]='0';
	buf[1]='x';
	buf[2]=hex[val>>4];
	buf[3]=hex[val& 0x0F];
	buf[4]='\0';
	
}




int main(void)
{


  HAL_Init();

  SystemClock_Config();
	
  MX_I2C1_Init();
	
	OLED_Init();
	
	OLED_ShowChinese_F16X16(0,2,2);
	OLED_ShowChinese_F16X16(0,4,3);
	
	uint8_t str_buf[]="SECURE_2026";
	uint8_t atread[64];
	//uint8_t buffer[10];
	uint8_t mod;

	HAL_I2C_Mem_Read(&hi2c1,AT24Cxx_Read_ADDR,0x00,I2C_MEMADD_SIZE_8BIT,&mod,1,100);
	if(mod==0xFF){
		HAL_I2C_Mem_Write(&hi2c1,AT24Cxx_Write_ADDR,0x00,I2C_MEMADD_SIZE_8BIT,str_buf,sizeof(str_buf)/sizeof(str_buf[0]),100);
		HAL_Delay(500);
		HAL_I2C_Mem_Read(&hi2c1,AT24Cxx_Read_ADDR,0x00,I2C_MEMADD_SIZE_8BIT,atread,sizeof(str_buf)/sizeof(str_buf[0]),100);
		OLED_ShowString_F8X16(1,1,atread);
	}
	else{

		HAL_I2C_Mem_Read(&hi2c1,AT24Cxx_Read_ADDR,0x00,I2C_MEMADD_SIZE_8BIT,atread,12,100);		
		//ByteToHexStr(atread,buffer);
		OLED_ShowString_F8X16(1,1,atread);
	}

  while (1)
  {
		
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
