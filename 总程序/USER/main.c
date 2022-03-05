#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "oled.h" 

 int main(void)
 {		
 	u16 t;  
	u16 len;	
	u16 times=0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart1_init(115200);	 //串口初始化为115200
 	uart2_init(9600);
	LED_Init();			     //LED端口初始化
	KEY_Init(); 
	OLED_Init();
	OLED_Clear();	 
	 //初始化与按键连接的硬件接口
 	while(1)
	{	OLED_ShowChar(10,10,'a',12);
		if(personal != personal_last)
			{
			
				personal_last = personal;
					SHOW(personal);
					personal_last = personal = 99;
			}

	}


 }
