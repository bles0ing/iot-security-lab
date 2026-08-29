#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "main.h"

/* ------------------ 按键引脚定义 ------------------ */
/* KEY1 引脚定义 */
#define KEY1_Pin        GPIO_PIN_0         // KEY1 引脚：PA0
#define KEY1_GPIO_Port  GPIOA

/* KEY2 引脚定义 */
#define KEY2_Pin        GPIO_PIN_13        // KEY2 引脚：PC13
#define KEY2_GPIO_Port  GPIOC

/* ------------------ 按键功能宏 ------------------ */
/* 读取 KEY1 当前状态：按下为 1，未按下为 0 */
#define KEY1_READ()     (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_SET ? 1 : 0)

/* 读取 KEY2 当前状态：按下为 1，未按下为 0 */
#define KEY2_READ()     (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_SET ? 1 : 0)


/* ------------------ 按键初始化函数 ------------------ */
void KEY_GPIO_Init(void);   // 按键 GPIO 初始化

#endif /* __BSP_KEY_H */
