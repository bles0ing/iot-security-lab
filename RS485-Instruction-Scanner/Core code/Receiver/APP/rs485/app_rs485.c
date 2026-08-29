

#include "rs485/app_rs485.h"
#include "rs485/bsp_rs485.h"
#include "usart/bsp_usart.h"
#include "key/bsp_key.h"
#include "led/bsp_led.h"
#include "oled/bsp_oled.h"
#include <string.h>

/**
 * @brief  RS485 任务函数
 * @param  无
 * @retval 无
 */
void RS485_Task(void)
{
    static uint8_t key1_last_state = 0;

    /* 检测 KEY1 按键下降沿（按下事件） */
    if(KEY1_READ() && !key1_last_state)  
    {
        /* 按键按下 */
        key1_last_state = 1;

        /* 切换到发送模式 */
        RS485_DIR_TX();
			  HAL_Delay(10);
        /* 蓝灯亮 */
        B_LED_ON_ONLY();

        /* 发送数据（示例数据） */
        uint8_t txData[] = "Hello RS485!!";
        OLED_ShowString_F8X16(2,0,txData);
			  RS485_Send(txData,sizeof(txData)-1);

        /* 切换到接收模式 */
        RS485_DIR_RX();
			  HAL_Delay(10);//神来之笔的延时

        /* 绿灯亮 */ 
        G_LED_ON_ONLY();
    }
    else if(!KEY1_READ() && key1_last_state)
    {
        /* 按键松开 */
        key1_last_state = 0;
    }
}
void RS485_Receive(){
	if(RS485_RxBuf[0]!=0){
		OLED_CLS();

		if(RS485_RxBuf[0]==0x03 || RS485_RxBuf[0]==0x06 ){                                
			RS485_DIR_TX();
			HAL_Delay(10);
			RS485_Send((uint8_t*)"ACK",3);
			
			RS485_DIR_RX();
			OLED_ShowString_F8X16(0,0,(uint8_t*)"NORAML");
			G_LED_ON_ONLY();
			
		}
		else{
			
			RS485_DIR_TX();
      HAL_Delay(10);
			
			RS485_Send((uint8_t*)"NAK",3);
			RS485_DIR_RX();
			OLED_ShowString_F8X16(0,0,(uint8_t*)"ABNORAML");
			B_LED_ON_ONLY();
			
		}
		memset(RS485_RxBuf,0,RS485_RX_BUF_SIZE);
  }
}

/*****************************END OF FILE***************************************/
