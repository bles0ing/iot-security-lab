/**
  ******************************************************************************
  * @file       bsp_key.c
  * @author     embedfire
  * @version     V1.0
  * @date        2025
  * @brief      按键函数接口
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

#include "key/bsp_key.h"

/**
  * @brief  按键 KEY 初始化函数
  * @note   完成 KEY1/KEY2 按键输入引脚的配置
  * @param  None
  * @retval None
  */
void KEY_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};   // 定义 GPIO 初始化结构体，并清零

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();   // 使能 GPIOC 时钟
  __HAL_RCC_GPIOD_CLK_ENABLE();   // 使能 GPIOD 时钟
  __HAL_RCC_GPIOA_CLK_ENABLE();   // 使能 GPIOA 时钟
  __HAL_RCC_GPIOB_CLK_ENABLE();   // 使能 GPIOB 时钟

  /* 配置 KEY2 引脚 */
  GPIO_InitStruct.Pin = KEY2_Pin;           // 选择 KEY2 引脚
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;   // 配置为输入模式
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;     // 配置下拉电阻，默认电平为低
  HAL_GPIO_Init(KEY2_GPIO_Port, &GPIO_InitStruct); // 初始化 KEY2 对应端口

  /* 配置 KEY1 引脚 */
  GPIO_InitStruct.Pin = KEY1_Pin;           // 选择 KEY1 引脚
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;   // 配置为输入模式
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;     // 配置下拉电阻，默认电平为低
  HAL_GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStruct); // 初始化 KEY1 对应端口
}

/*****************************END OF FILE***************************************/
