#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "oled.h" 

 int main(void)
 {		
					delay_init();	    	 
					NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
					uart1_init(115200);	 
					uart2_init(9600);
					OLED_Init();
					OLED_Clear();	 
			while(1)
				{
										
						if(personal != personal_last)
								{	
										personal_last = personal;
										SHOW(personal);
										personal_last = personal = 99;
								}

				}


 }
