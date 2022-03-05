#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "uart.h"

 int main(void)
 {	
	
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 //串口初始化为115200
	 
	 //初始化与按键连接的硬件接口
 	while(1)
	{
		//  Usart_SendStr(USART1, "欢迎使用秉火STM32F103开发板 \n");
		//  USART_SendData(USART1, 'a');

			printf("%",name[0][0]); 
		  delay_ms(50000);
			delay_ms(50000);
		
		
	}
		
		
	}


