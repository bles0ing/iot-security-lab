#ifndef __BSP_RS485_H
#define __BSP_RS485_H

#include "main.h"

/* 外部声明串口句柄定义 */
extern UART_HandleTypeDef huart3;
extern uint8_t Rx_flag;

/* ------------------ RS485 引脚定义 ------------------ */
/* TX 引脚 */
#define RS485_TX_Pin        GPIO_PIN_10
#define RS485_TX_GPIO_Port  GPIOB

/* RX 引脚 */
#define RS485_RX_Pin        GPIO_PIN_11
#define RS485_RX_GPIO_Port  GPIOB

/* DE/RE 引脚（方向控制） */
#define RS485_RE_Pin       GPIO_PIN_8
#define RS485_DE_Pin       GPIO_PIN_9
#define RS485_DIR_GPIO_Port GPIOB

/* ------------------ RS485 方向控制宏 ------------------ */
/* 切换到发送模式 */
#define RS485_DIR_TX()      HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DE_Pin|RS485_RE_Pin , GPIO_PIN_SET)

/* 切换到接收模式 */
#define RS485_DIR_RX()      HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DE_Pin|RS485_RE_Pin , GPIO_PIN_RESET)

/* ------------------ RS485 接收缓冲区大小 ------------------ */
#define RS485_RX_BUF_SIZE    128
extern uint8_t RS485_RxBuf[RS485_RX_BUF_SIZE];

/* ------------------ 函数声明 ------------------ */
void RS485_UART_Init(void);   // RS485 串口初始化
void RS485_DIR_Init(void);		// RS485 DIR 初始化
void RS485_Init(void);				// RS485 初始化
void RS485_Send(uint8_t *pData, uint16_t size);   // RS485 发送数据

#endif /* __BSP_RS485_H */
