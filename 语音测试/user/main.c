#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "uart.h"

 int main(void)
 {	
	
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 //���ڳ�ʼ��Ϊ115200
	 
	 //��ʼ���밴�����ӵ�Ӳ���ӿ�
 	while(1)
	{
		//  Usart_SendStr(USART1, "��ӭʹ�ñ���STM32F103������ \n");
		//  USART_SendData(USART1, 'a');

			printf("%",name[0][0]); 
		  delay_ms(50000);
			delay_ms(50000);
		
		
	}
		
		
	}


