#include "ax_basis.h"
#include <stdio.h>


void AX_Init(uint32_t baud) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
                       	   RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE); //����GPIOʱ�� 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD,  GPIO_Pin_10);	


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD,  GPIO_Pin_11);


	GPIO_SetBits(GPIOE,  GPIO_Pin_6);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOE, &GPIO_InitStructure);


	GPIO_SetBits(GPIOE,  GPIO_Pin_15);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);


	GPIO_SetBits(GPIOE,  GPIO_Pin_10);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	/* ��ʱ���� SysTick ���� */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 	//ʱ�ӹ̶�ΪAHBʱ�ӵ�1/8

	/* ���� USART1���� */
	//��GPIO��USART������ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	//USART1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 

	//USART1 �˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOA,&GPIO_InitStructure); 

	//USART1��������
	USART_InitStructure.USART_BaudRate = baud;    //������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	//ʹ�� USART�� �������
	USART_Cmd(USART1, ENABLE); 
	
	//��ܵ�һ���ַ����������BUG
	USART_ClearFlag(USART1, USART_FLAG_TC);
}

/**
  * @��  ��  SW���ѡ��״̬
  * @˵  ��	 ���ز�����ON��Ϊ1���Բ�Ϊ0��
  * @��  ��  ��	  
  * @����ֵ  ����״̬
  *			SW1(PA8)  SW2(PE10)    ����ֵ
  *			 0         0             0  
  *          0         1             1  
  *          1         0             2
  *          1         1             3   
  */
uint8_t AX_SW_GetSW12Status(void)
{
    uint8_t sw = 0;

	if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_15)) 	
	{sw <<= 1;}
	else
	{sw=sw+1; sw<<=1;}

	if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_10))
	{;}
	else
	{sw=sw+1;}

	return sw;
}

/**
  * @��  ��  SW1���ѡ��״̬
  * @��  ��  ��	  
  * @����ֵ  ���뿪��״̬
  *			 1 - ���ز�����ON��
  *			 0 - ���ز�����ON�Բ�
  */
uint8_t AX_SW_GetSW1Status(void)
{
   	if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_15))
	return 0;
	else
	return 1;
}

/**
  * @��  ��  SW2���ѡ��״̬
  * @��  ��  ��	  
  * @����ֵ  ���뿪��״̬
  *			 1 - ���ز�����ON��
  *			 0 - ���ز�����ON�Բ�
  */
uint8_t AX_SW_GetSW2Status(void)
{
   	if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_10))
	return 0;
	else
	return 1;
}

/**
  * @��  ��  ���΢����ʱ
  * @��  ��  us����ʱ���ȣ���λus	  
  * @����ֵ  ��
  */
void AX_Delayus(uint16_t us)
{
	uint32_t temp;
	
	SysTick->LOAD=21*us; 				 		 
	SysTick->VAL=0x00;        				
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;  	 
	
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));	 
	
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; 
	SysTick->VAL =0X00;       				
}

/**
  * @��  ��  ��������ʱ������
  * @��  ��  ms����ʱ���ȣ���λms	  	  
  * @����ֵ  ��
  * @˵  ��  ע��ms�ķ�Χ��SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:nTime<=0xffffff*8*1000/SYSCLK
  *          ��168M������,ms<=798ms 
  */
static void Delay_ms(uint16_t ms)
{	 		  	  
	uint32_t temp;	
	
	SysTick->LOAD=(uint32_t)21000*ms;			
	SysTick->VAL =0x00;           		
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; 
	
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	
	
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;      
	SysTick->VAL =0X00;     		  		 	    
}

/**
  * @��  ��  ���������ʱ
  * @��  ��  ms����ʱ���ȣ���λms	  	 	  
  * @����ֵ  ��
  */
void AX_Delayms(uint16_t ms)
{
	uint8_t repeat=ms/500;																
	uint16_t remain=ms%500;
	
	while(repeat)
	{
		Delay_ms(500);
		repeat--;
	}
	
	if(remain)
	{
		Delay_ms(remain);
	}
		
}

/*** ���ڴ�ӡ��غ����ض��� ***********/
/**
  * @��  ��  �ض���putc������USART1��	
  */
int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1, (uint8_t) ch);

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}

	return ch;
}

/**
  * @��  ��  �ض���getc������USART1��	
  */
int fgetc(FILE *f)
{
	/* �ȴ�����1�������� */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
	{}

	return (int)USART_ReceiveData(USART1);
}

/******************* (C) ��Ȩ 2018 XTARK **************************************/
