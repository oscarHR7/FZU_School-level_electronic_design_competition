#include "sys.h"
#include "usart.h"	
#include "oled.h"
#include "delay.h"
#include <stdlib.h>

#if SYSTEM_SUPPORT_OS
#include "includes.h"					  
#endif

static u8 data=0;


u8 data_test = 0;
u8 com_data; 

u8 personal = 99;
u8 personal_last = 99;
 
#if 1
#pragma import(__use_no_semihosting)             
               
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
  
void _sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0); 
    USART2->DR = (u8) ch;      
	return ch;
}
#endif 

 
#if EN_USART1_RX   

 	
u8 USART_RX_BUF[USART_REC_LEN];    

u16 USART_RX_STA=0;     	  
  
void uart1_init(u32 bound){
 
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	
  
	//USART1_TX
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  //USART1_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

  USART_Init(USART1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1, ENABLE);                    
	USART_ClearFlag(USART1, USART_FLAG_TC);

}

void uart2_init(u32 bound){
  
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	 //??????
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	

  
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	
  

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

  USART_Init(USART2, &USART_InitStructure); 
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART2, ENABLE);                   



}

void USART1_IRQHandler(void)                	
	{


#if SYSTEM_SUPPORT_OS 		
	OSIntEnter();    
#endif
	
		u8 i;
		static u16 RxBuffer1[10]={0};
		static u8 RxCounter1=0;
		static u8 RxState = 0;	
		static u8 RxFlag1 = 0;

				if( USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)  	  
						{
								USART_ClearITPendingBit(USART1,USART_IT_RXNE);   
								com_data = USART_ReceiveData(USART1);
			
					if(RxState==0&&com_data==0x2C)  
						{
							RxState=1;
							RxBuffer1[RxCounter1++]=com_data;
						}
		
					else if(RxState==1&&com_data==0x12)  
						{
							RxState=2;
							RxBuffer1[RxCounter1++]=com_data;		
						}
		
					else if(RxState==2)
						{
							RxBuffer1[RxCounter1++]=com_data;

							if(RxCounter1>=10||com_data == 0x5B)       
							{
								RxState=3;
								RxFlag1=1;
								personal=RxBuffer1[RxCounter1-2];
							}
						}
		
					else if(RxState==3)		
						{
								if(RxBuffer1[RxCounter1-1] == 0x5B)
								{
											USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);		
											RxFlag1 = 0;
											RxCounter1 = 0;
											RxState = 0;
											USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
								}
						else   
								{
											RxState = 0;
											RxCounter1=0;
											for(i=0;i<10;i++)
											{
													RxBuffer1[i]=0x00;     
											}
								}
						} 
	
						else   
						{
								RxState = 0;
								RxCounter1=0;
								for(i=0;i<10;i++)
								{
										RxBuffer1[i]=0x00;      
								}
						}

								}
			}



#if SYSTEM_SUPPORT_OS 	
	OSIntExit();  											 
#endif
void SHOW(u8 personal)
{
				switch (personal)
				{
					case 45: 
										OLED_Clear();
										OLED_ShowCHinese(48,10,70);
										OLED_ShowCHinese(64,10,71);
										OLED_ShowCHinese(80,10,72);
										delay_ms(3000);
										break;
					case 46: 
										OLED_Clear();
										OLED_ShowCHinese(48,10,73);
										OLED_ShowCHinese(64,10,74);
										delay_ms(3000);
										break;
					case 47: 
										OLED_Clear();
										OLED_ShowCHinese(48,10,75);
										OLED_ShowCHinese(64,10,76);
										OLED_ShowCHinese(80,10,72);
										delay_ms(3000);
										break;
				
        	case 0: 
										OLED_Clear();
										OLED_ShowCHinese(48,10,0);
										OLED_ShowCHinese(64,10,1);
										OLED_ShowCHinese(80,10,2);
										printf("陌生人，禁止进入");
										delay_ms(3000);
										break;
			 
					case 1:
										OLED_Clear();
										OLED_ShowCHinese(48,10,3);
										OLED_ShowCHinese(64,10,4);
										OLED_ShowCHinese(80,10,5);
										printf("杨浩然，请进入"); 
										delay_ms(3000);
										break;
					case 2:
										OLED_Clear();
										OLED_ShowCHinese(48,10,6);
										OLED_ShowCHinese(64,10,7);
										OLED_ShowCHinese(80,10,8);
										printf("卓嘉伟，请进入");
										delay_ms(3000);
										break;
        	case 3:
										OLED_Clear();
										OLED_ShowCHinese(48,10,9);
										OLED_ShowCHinese(64,10,10);
										printf("张程，请进入"); 
										delay_ms(3000);
										break;
					case 4:
										OLED_Clear();
										OLED_ShowCHinese(48,10,9);
										OLED_ShowCHinese(64,10,11);
										OLED_ShowCHinese(80,10,12);
										printf("张志龙，请进入");
										delay_ms(3000);
										break;
        	case 5:		OLED_Clear();
										OLED_ShowCHinese(48,10,13);
										OLED_ShowCHinese(64,10,14);
										OLED_ShowCHinese(80,10,15);
										printf("涂淦英，请进入");
										delay_ms(3000);
										break;
        	case 6:
										OLED_Clear();
										OLED_ShowCHinese(48,10,34);
										OLED_ShowCHinese(64,10,35);
										OLED_ShowCHinese(80,10,36);
										printf("董晓龙，请进入");
										delay_ms(3000);
										break;
        	case 7:
										OLED_Clear();
										OLED_ShowCHinese(48,10,39);
										OLED_ShowCHinese(64,10,40);
										OLED_ShowCHinese(80,10,41);
										printf("丁玉萧，请进入");
										delay_ms(3000);
										break;
					case 8:
										OLED_Clear();
										OLED_ShowCHinese(48,10,37);
										OLED_ShowCHinese(64,10,38);
										OLED_ShowCHinese(80,10,6);
										printf("赖晨卓，请进入");
										delay_ms(3000);
										break;
					case 9:
										OLED_Clear();
										OLED_ShowCHinese(48,10,23);
										OLED_ShowCHinese(64,10,45);
										OLED_ShowCHinese(80,10,46);
										printf("林登炎，请进入");
										delay_ms(3000);
										break;
					case 10:
										OLED_Clear();
										OLED_ShowCHinese(48,10,47);
										OLED_ShowCHinese(64,10,48);
										OLED_ShowCHinese(80,10,49);
										printf("罗玉春，请进入");
										delay_ms(3000);
										break;
					case 11:
										OLED_Clear();
										OLED_ShowCHinese(48,10,50);
										OLED_ShowCHinese(64,10,51);
										OLED_ShowCHinese(80,10,19);
										printf("刘其斌，请进入");
										delay_ms(3000);
										break;
					case 12:
										OLED_Clear();
										OLED_ShowCHinese(48,10,20);
										OLED_ShowCHinese(64,10,21);
										OLED_ShowCHinese(80,10,22);
										printf("许太荣，请进入");
										delay_ms(3000);
										break;
										
					case 13:
										OLED_Clear();
										OLED_ShowCHinese(48,10,42);
										OLED_ShowCHinese(64,10,8);
										OLED_ShowCHinese(80,10,43);
										printf("吴伟晗，请进入");
										delay_ms(3000);
										break;
					case 14:
										OLED_Clear();
										OLED_ShowCHinese(48,10,23);
										OLED_ShowCHinese(64,10,24);
										OLED_ShowCHinese(80,10,25);
										printf("林炳一，请进入");
										delay_ms(3000);
										break;
					case 15:
										OLED_Clear();
										OLED_ShowCHinese(48,10,3);
										OLED_ShowCHinese(64,10,16);
										OLED_ShowCHinese(80,10,17);
										printf("杨彬辉，请进入");
										delay_ms(3000);
										break;
					case 16:
										OLED_Clear();
										OLED_ShowCHinese(48,10,26);
										OLED_ShowCHinese(64,10,27);
										OLED_ShowCHinese(80,10,28);
										printf("梁家琪，请进入");
										delay_ms(3000);
										break;
					case 17:
										OLED_Clear();
										OLED_ShowCHinese(48,10,26);
										OLED_ShowCHinese(64,10,27);
										OLED_ShowCHinese(80,10,28);
										printf("吴育懋，请进入");
										delay_ms(3000);
										break;
					case 18:
										OLED_Clear();
										OLED_ShowCHinese(48,10,3);
										OLED_ShowCHinese(64,10,18);
										OLED_ShowCHinese(80,10,19);
										printf("杨良斌，请进入");
										delay_ms(3000);
										break;
					case 19:
										OLED_Clear();
										OLED_ShowCHinese(48,10,52);
										OLED_ShowCHinese(64,10,53);
										OLED_ShowCHinese(80,10,54);
										printf("徐哲远，请进入");
										delay_ms(3000);
										break;
					case 20:
										OLED_Clear();
										OLED_ShowCHinese(48,10,55);
										OLED_ShowCHinese(64,10,56);
										OLED_ShowCHinese(80,10,57);
										printf("赖松涛，请进入");
										delay_ms(3000);
										break;
					case 21:
										OLED_Clear();
										OLED_ShowCHinese(48,10,58);
										OLED_ShowCHinese(64,10,59);
										OLED_ShowCHinese(80,10,60);
										printf("唐安培，请进入");
										delay_ms(3000);
										break;
					case 22:
										OLED_Clear();
										OLED_ShowCHinese(48,10,61);
										OLED_ShowCHinese(64,10,65);
										OLED_ShowCHinese(80,10,66);
										printf("方立栋，请进入");
										delay_ms(3000);
										break;
					case 23:
										OLED_Clear();
										OLED_ShowCHinese(48,10,67);
										OLED_ShowCHinese(64,10,68);
										OLED_ShowCHinese(80,10,69);
										printf("郑皓文，请进入");
										delay_ms(3000);
										break;
					case 24:
										OLED_Clear();
										OLED_ShowCHinese(48,10,26);
										OLED_ShowCHinese(64,10,25);
										OLED_ShowCHinese(80,10,23);
										printf("翟玉林，请进入");
										delay_ms(3000);
										break;
					case 25:
										OLED_Clear();
										OLED_ShowCHinese(48,10,34);
										OLED_ShowCHinese(64,10,43);
										OLED_ShowCHinese(80,10,44);
										printf("王耀鸿，请进入");
										delay_ms(3000);
										break;
					case 26:
										OLED_Clear();
										OLED_ShowCHinese(48,10,42);
										OLED_ShowCHinese(64,10,46);
										OLED_ShowCHinese(80,10,49);
										printf("原晨景，请进入");
										delay_ms(3000);
										break;
					case 27:
										OLED_Clear();
										OLED_ShowCHinese(48,10,47);
										OLED_ShowCHinese(64,10,51);
										OLED_ShowCHinese(80,10,56);
										printf("张悦斌，请进入");
										delay_ms(3000);
										break;
					case 28:
										OLED_Clear();
										OLED_ShowCHinese(48,10,9);
										OLED_ShowCHinese(64,10,12);
										OLED_ShowCHinese(80,10,14);
										printf("唐普颉，请进入");
										delay_ms(3000);
										break;
					case 29:
										OLED_Clear();
										OLED_ShowCHinese(48,10,13);
										OLED_ShowCHinese(64,10,12);
										OLED_ShowCHinese(80,10,19);
										printf("郭伟铭，请进入");
										delay_ms(3000);
										break;
					case 30:
										OLED_Clear();
										OLED_ShowCHinese(48,10,39);
										OLED_ShowCHinese(64,10,57);
										OLED_ShowCHinese(80,10,58);
										printf("李岩铎，请进入");
										delay_ms(3000);
										break;
					case 31:
										OLED_Clear();
										OLED_ShowCHinese(48,10,62);
										OLED_ShowCHinese(64,10,63);
										OLED_ShowCHinese(80,10,64);
										printf("请进入");
										delay_ms(3000);
										break;
					case 32:
										OLED_Clear();
										OLED_ShowCHinese(48,10,62);
										OLED_ShowCHinese(64,10,63);
										OLED_ShowCHinese(80,10,64);
										printf("请进入");
										delay_ms(3000);
										break;
					case 33:
										OLED_Clear();
										OLED_ShowCHinese(48,10,62);
										OLED_ShowCHinese(64,10,63);
										OLED_ShowCHinese(80,10,64);
										printf("请进入");
										delay_ms(3000);
										break;
					case 34:
										OLED_Clear();
										OLED_ShowCHinese(48,10,62);
										OLED_ShowCHinese(64,10,63);
										OLED_ShowCHinese(80,10,64);
										printf("请进入");
										delay_ms(3000);
										break;
					case 35:
										OLED_Clear();
										OLED_ShowCHinese(48,10,62);
										OLED_ShowCHinese(64,10,63);
										OLED_ShowCHinese(80,10,64);
										printf("请进入");
										delay_ms(3000);
										break;
        	default:	
										OLED_Clear();
										OLED_ShowCHinese(16,10,29);
										OLED_ShowCHinese(32,10,30);
										OLED_ShowCHinese(48,10,31);
										OLED_ShowCHinese(64,10,32);
										OLED_ShowCHinese(80,10,2);
										OLED_ShowCHinese(96,10,33);
										delay_ms(3000);
										break;
										
				}
				
			
}

#endif	

