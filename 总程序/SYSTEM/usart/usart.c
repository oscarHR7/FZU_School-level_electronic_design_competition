#include "sys.h"
#include "usart.h"	
#include "led.h"
#include "oled.h"
#include "delay.h"
#include <stdlib.h>
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
static u8 n1=0,n2=0,n3=0,n=0;
static u8 data=0;
u8  string2 [32] = {0};
//u8  temp = 0;
u8 data_test = 0;
u8 com_data; 

u8 personal = 99;
u8 personal_last = 99;
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//??????????                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//??_sys_exit()??????????    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//???fputc?? 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//????,??????   
    USART2->DR = (u8) ch;      
	return ch;
}
#endif 
/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void uart1_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//?????3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//????3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ????
	NVIC_Init(&NVIC_InitStructure);	//??????????VIC???
  
   //USART ?????

	USART_InitStructure.USART_BaudRate = bound;//?????
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//???8?????
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//?????
	USART_InitStructure.USART_Parity = USART_Parity_No;//??????
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//????????
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//????

  USART_Init(USART2, &USART_InitStructure); //?????1
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//????????
  USART_Cmd(USART2, ENABLE);                    //????1 



}

void USART1_IRQHandler(void)                	//串口1中断服务程序
	{


#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	
				 

		u8 i;
		static u16 RxBuffer1[10]={0};
		static u8 RxCounter1=0;
		static u8 RxState = 0;	
		static u8 RxFlag1 = 0;

		if( USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)  	   //????  
		{
				USART_ClearITPendingBit(USART1,USART_IT_RXNE);   //??????
				com_data = USART_ReceiveData(USART1);
			
				if(RxState==0&&com_data==0x2C)  //0x2c??
				{
					RxState=1;
					RxBuffer1[RxCounter1++]=com_data;
				}
		
				else if(RxState==1&&com_data==0x12)  //0x12??
				{
					RxState=2;
					RxBuffer1[RxCounter1++]=com_data;
					
				}
		
				else if(RxState==2)
				{
					RxBuffer1[RxCounter1++]=com_data;

					if(RxCounter1>=10||com_data == 0x5B)       //RxBuffer1????,??????
					{
						RxState=3;
						RxFlag1=1;
						personal=RxBuffer1[RxCounter1-2];
					}
				}
		
				else if(RxState==3)		//???????????
				{
						if(RxBuffer1[RxCounter1-1] == 0x5B)
						{
									USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);//??DTSABLE??		
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
								RxBuffer1[i]=0x00;      //?????????
						}
				}

		}
	}


		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
//		if( USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)  	   //????  
//		{
//		  	OLED_Clear();
//				USART_ClearITPendingBit(USART1,USART_IT_RXNE);   //??????
//				com_data = USART_ReceiveData(USART1);
////				 sprintf(string2,"" com_data + '0');
//				OLED_ShowChar(20,20 ,com_data + '0',12);
//				
//				
//		}

#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
void SHOW(u8 personal)
{
				switch (personal)
				{
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
        	case 3:		OLED_Clear();
										OLED_ShowCHinese(48,10,0);
										OLED_ShowCHinese(64,10,4);
										OLED_ShowCHinese(80,10,5);
										printf("张程，请进入"); 
										delay_ms(3000);
										break;
					case 4:
										OLED_Clear();
										OLED_ShowCHinese(48,10,3);
										OLED_ShowCHinese(64,10,0);
										OLED_ShowCHinese(80,10,5);
										printf("张志龙，请进入");
										delay_ms(3000);
										break;
        	case 5:		OLED_Clear();
										OLED_ShowCHinese(48,10,3);
										OLED_ShowCHinese(64,10,4);
										OLED_ShowCHinese(80,10,0);
										printf("涂淦英，请进入");
										delay_ms(3000);
										break;
        	case 6://printf("Saturday\n");break;
        	case 7://printf("Sunday\n");break;
        	default:printf("未检测到人脸");
									break;

										
				}
				
			
}

#endif	

