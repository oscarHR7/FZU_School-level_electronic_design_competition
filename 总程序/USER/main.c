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
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart1_init(115200);	 //���ڳ�ʼ��Ϊ115200
 	uart2_init(9600);
	LED_Init();			     //LED�˿ڳ�ʼ��
	KEY_Init(); 
	OLED_Init();
	OLED_Clear();	 
	 //��ʼ���밴�����ӵ�Ӳ���ӿ�
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
