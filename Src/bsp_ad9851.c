/**
 ******************************************************************************
 * @file			: bsp_ad9851.c
 * @brief			: AD9851 Driver.
 * @version		    : 0.1.0
 * @author		    : ��������
 * @date			: 2022.07.27
 ******************************************************************************
 * @pinset
 *              PA4     --> CLK
 *              PA5     --> FQ
 *              PA6     --> RST   
 *              PC0     --> D0
 *              PC1     --> D1
 *              PC2     --> D2
 *              PC3     --> D3
 *              PC4     --> D4
 *              PC5     --> D5
 *              PC6     --> D6
 *              PC7     --> D7
 *
 ******************************************************************************
 */

#include "bsp_ad9851.h" 

#ifdef USE_AD9851_SER

uint8_t AD9851_FD=0x01; //AD9851����6��Ƶ,AD9850��֧�ֱ�Ƶ��AD9851֧�ֱ�Ƶ

//***************************************************//
//              AD9851��λ(����ģʽ)                 //
//---------------------------------------------------//
void AD9851_reset_serial()
{
	AD9851_w_clk=0;
	AD9851_fq_up=0;
	//rest�ź�
	AD9851_rest=0;
	AD9851_rest=1;
	AD9851_rest=0;
	//w_clk�ź�
	AD9851_w_clk=0;
	AD9851_w_clk=1;
	AD9851_w_clk=0;
	//fq_up�ź�
	AD9851_fq_up=0;
	AD9851_fq_up=1;
	AD9851_fq_up=0;
}

//***************************************************//
//          ��AD9851��д����������(����)             //
//---------------------------------------------------//
void AD9851_wr_serial(uint8_t w0,double frequence)
{
	unsigned char i,w;
	long int y;
	double x;
	//����Ƶ�ʵ�HEXֵ
	x=4294967295/180;//
	
	frequence=frequence/1000000;
	frequence=frequence*x;
	y=frequence;
	//дw4����
	w=(y>>=0);
	for(i=0;i<8;i++)
	{
		AD9851_bit_data=(w>>i)&0x01;
		AD9851_w_clk=1;
		AD9851_w_clk=0;
	}
	//дw3����
	w=(y>>8);
	for(i=0;i<8;i++)
	{
		AD9851_bit_data=(w>>i)&0x01;
		AD9851_w_clk=1;
		AD9851_w_clk=0;
	}
	//дw2����
	w=(y>>16);
	for(i=0;i<8;i++)
	{
		AD9851_bit_data=(w>>i)&0x01;
		AD9851_w_clk=1;
		AD9851_w_clk=0;
	}
	//дw1����
	w=(y>>24);
	for(i=0;i<8;i++)
	{
		AD9851_bit_data=(w>>i)&0x01;
		AD9851_w_clk=1;
		AD9851_w_clk=0;
	}
	//дw0����
	w=w0;   
	for(i=0;i<8;i++)
	{
		AD9851_bit_data=(w>>i)&0x01;
		AD9851_w_clk=1;
		AD9851_w_clk=0;
	}
	//����ʼ��
	AD9851_fq_up=1;
	AD9851_fq_up=0;
}
void AD9851_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure ; 

	__HAL_RCC_GPIOA_CLK_ENABLE();	 //ʹ��PA�˿�ʱ��
	__HAL_RCC_GPIOC_CLK_ENABLE();	 //ʹ��PA�˿�ʱ��

	GPIO_InitStructure.Pin = GPIO_Pin_0| GPIO_Pin_1| GPIO_Pin_2| GPIO_Pin_3| GPIO_Pin_4| GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 ; 
	GPIO_InitStructure.Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.Mode = GPIO_Mode_Out_PP;	
	GPIO_Init(GPIOC ,&GPIO_InitStructure) ;

	GPIO_InitStructure.Pin =  GPIO_Pin_5|GPIO_Pin_4| GPIO_Pin_6; 
	GPIO_Init(GPIOA ,&GPIO_InitStructure) ;
}
//Ƶ�����
void AD9851_Setfq(double fq)
{
	AD9851_wr_serial(AD9851_FD,fq);
}


void AD9851_Init(void)
{
	AD9851_IO_Init();
	D2=0;D1=1;D0=1; //���ô���ģʽD2��D0��̶���ƽ011�����оƬ�����ֲ�
	AD9851_reset_serial();
}
#endif
#ifdef USE_AD9851_PAR

uint8_t AD9851_FD=0x01; //AD9851����6��Ƶ,AD9850��֧�ֱ�Ƶ��AD9851֧�ֱ�Ƶ

//***************************************************//
//              AD9851��λ(����ģʽ)                 //
//---------------------------------------------------//
void AD9851_reset()
{
	AD9851_w_clk=0;
	AD9851_fq_up=0;
	//rest�ź�
	AD9851_rest=0;
	AD9851_rest=1;
	AD9851_rest=0;
}

//***************************************************//
//          ��AD9851��д����������(����)             //
//---------------------------------------------------//
void AD9851_wr_parrel(uint8_t w0,double frequence)
{
	uint32_t w;
	long int y;
	double x;
	//����Ƶ�ʵ�HEXֵ
	x=4294967295/180;//�ʺ�125M����
	//���ʱ��Ƶ�ʲ�Ϊ125MHZ���޸ĸô���Ƶ��ֵ����λMHz ������
	frequence=frequence/1000000;
	frequence=frequence*x;
	y=frequence;
	//дw0����
	w=w0;   
	AD9851_DataBus=w|(w^0xff)<<16;     
	AD9851_w_clk=1;
	AD9851_w_clk=0;
	//дw1����
	w=(y>>24);
	AD9851_DataBus=w|(w^0xff)<<16;     
	AD9851_w_clk=1;
	AD9851_w_clk=0;
	//дw2����
	w=(y>>16);
	AD9851_DataBus=w|(w^0xff)<<16;     
	AD9851_w_clk=1;
	AD9851_w_clk=0;
	//дw3����
	w=(y>>8);
	AD9851_DataBus=w|(w^0xff)<<16;     
	AD9851_w_clk=1;
	AD9851_w_clk=0;
	//дw4����
	w=(y>>=0);
	AD9851_DataBus=w|(w^0xff)<<16;     
	AD9851_w_clk=1;
	AD9851_w_clk=0;
	//����ʼ��
	AD9851_fq_up=1;
	AD9851_fq_up=0;
}
void AD9851_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure ; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB,PE�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1| GPIO_Pin_2| GPIO_Pin_3| GPIO_Pin_4| GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 ; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_Init(GPIOC ,&GPIO_InitStructure) ;

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3|GPIO_Pin_4| GPIO_Pin_6; 
	GPIO_Init(GPIOA ,&GPIO_InitStructure) ;
}
//Ƶ�����
void AD9851_Setfq(double fq)
{
	AD9851_wr_parrel(AD9851_FD,fq);
}


void AD9851_Init(void)
{
	AD9851_IO_Init();
	AD9851_reset();
}
#endif






