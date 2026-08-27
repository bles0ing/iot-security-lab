/**
  ******************************************************************************
  * @file       bsp_rs485.c
  * @author     embedfire
  * @version     V1.0
  * @date        2025
  * @brief      RS485 函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台  ：野火 STM32F103C8T6-STM32开发板 
  * 论坛      ：http://www.firebbs.cn
  * 官网      ：https://embedfire.com/
  * 淘宝      ：https://yehuosm.tmall.com/
  *
  ******************************************************************************
  */
  
#include "rs485/bsp_rs485.h"
#include "usart/bsp_usart.h"
#include "led/bsp_led.h"
#include "oled/bsp_oled.h"


/* 串口句柄定义 */
UART_HandleTypeDef huart3;

/* 接收缓冲区 */
uint8_t RS485_RxBuf[RS485_RX_BUF_SIZE];

/**
  * @brief  RS485 串口初始化函数（USART3）
  * @param  无
  * @retval 无
  */
void RS485_UART_Init(void)
{
  /* 配置 USART3 外设 */
  huart3.Instance = USART3;                       // 选择 USART3 外设
  huart3.Init.BaudRate = 115200;                  // 设置波特率为 115200
  huart3.Init.WordLength = UART_WORDLENGTH_8B;    // 设置数据位长度为 8 位
  huart3.Init.StopBits = UART_STOPBITS_1;         // 设置停止位为 1 位
  huart3.Init.Parity = UART_PARITY_NONE;          // 设置无奇偶校验
  huart3.Init.Mode = UART_MODE_TX_RX;             // 设置工作模式：收发均开启
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;    // 设置无硬件流控
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;// 设置过采样率为 16

  /* 调用 HAL 库初始化函数 */
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();  // 若初始化失败，调用错误处理函数
  }
}

/**
  * @brief   RS485 DIR 初始化函数
  * @param   无
  * @retval  无
  */
void RS485_DIR_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};   // 定义 GPIO 初始化结构体

  /* 使能所需 GPIO 端口时钟 */
  __HAL_RCC_GPIOB_CLK_ENABLE();             // 使能 GPIOB 时钟

  /* 上电设置 RS485_DIR 引脚初始输出电平为低电平 */
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_RE_Pin, GPIO_PIN_RESET);

  /* 配置 RS485_DIR 引脚为推挽输出模式 */
  GPIO_InitStruct.Pin = RS485_RE_Pin;          // 配置 RS485_DIR 引脚
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   // 设置为推挽输出模式
  GPIO_InitStruct.Pull = GPIO_NOPULL;           // 不使用上下拉电阻
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // 设置输出速度为低速
	HAL_GPIO_Init(RS485_DIR_GPIO_Port, &GPIO_InitStruct);
	
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);

  /* 配置 RS485_DIR 引脚为推挽输出模式 */
  GPIO_InitStruct.Pin = RS485_DE_Pin;          // 配置 RS485_DIR 引脚
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   // 设置为推挽输出模式
  GPIO_InitStruct.Pull = GPIO_NOPULL;           // 不使用上下拉电阻
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // 设置输出速度为低速
  HAL_GPIO_Init(RS485_DIR_GPIO_Port, &GPIO_InitStruct);  // 初始化 GPIO
}

/**
 * @brief  RS485 初始化
 * @param  无
 * @retval 无
 */
void RS485_Init(void)
{    
    /* 初始化RS485串口引脚和外设 */
    RS485_UART_Init();
	
    /* 初始化RS485其它引脚（DIR） */
    RS485_DIR_Init();
	
		/* 开启空闲中断接收 */
		HAL_UARTEx_ReceiveToIdle_IT(&huart3, RS485_RxBuf, RS485_RX_BUF_SIZE);
	
		/* 接收模式下绿灯亮 */
		G_LED_ON_ONLY();
}

/**
 * @brief  RS485 发送函数
 * @param  pData：待发送数据指针
 * @param  size：数据长度
 * @retval 无
 */
void RS485_Send(uint8_t *pData, uint16_t size)
{
    HAL_UART_Transmit(&huart3, pData, size, HAL_MAX_DELAY);
	
    /* 等待发送完成，确保最后一个字节发出 */
    while (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) == RESET);
}


/**
  * @brief  串口接收空闲中断回调函数
  * @param  huart: 串口句柄
  * @param  Size : 本次接收到的数据长度
  */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART3){   // 判断是不是 RS485 串口
				//OLED_CLS();
	      OLED_ShowString_F8X16(0,6,(uint8_t*)"DATA:");
        OLED_ShowString_F8X16(1,1,RS485_RxBuf);
		
				// 开启接收到空闲中断
				HAL_UARTEx_ReceiveToIdle_IT(&huart3, RS485_RxBuf, RS485_RX_BUF_SIZE);
		}
}

/*****************************END OF FILE***************************************/

