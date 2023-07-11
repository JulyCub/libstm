/**
 * @file bsp_ov2640.c
 * @author July (Email: JulyCub@163.com)
 * @brief OV2640 Driver
 * @version 0.1
 * @date 2023.05.24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "dcmi.h"
#include "usart.h"
#include "string.h"
#include "bsp_ov2640.h"
#include "bsp_ov2640cfg.h"
#include "bsp_sccb.h"
#include "bsp_common.h"
#include "bsp_delay.h"
#include "bsp_usart.h"
#include "bsp_dcmi.h"
#include "bsp_lcd.h"

/**
 * @brief ��ʼ�� OV2640
 * @attention �������Ժ�,Ĭ�������1600*1200�ߴ��ͼƬ
 * @attention ��ʹ��CubeMX����DCMI���Զ����ɵ�������Ҫ����
 * @return 0-�ɹ� | ����-�������
 */
u8 BSP_OV2640_Init(void)
{
	u16 i = 0;
	u16 reg;

	GPIO_InitTypeDef GPIO_InitStructure;

	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	// GPIOG10,12��ʼ������
	GPIO_InitStructure.Pin = OV2640_PWDN_PIN | OV2640_RST_PIN; // PG10,12�������
	GPIO_InitStructure.Mode = GPIO_Mode_OUT;				   // �������
	GPIO_InitStructure.Speed = GPIO_Speed_100MHz;			   // 100MHz
	GPIO_InitStructure.Pull = GPIO_PuPd_UP;					   // ����
	GPIO_Init(GPIOG, &GPIO_InitStructure);					   // ��ʼ��

	// PF8��ʼ�����ã�PF8���ڿ�������ͷ�������LED�ƣ��ߵ�ƽ�������͵�ƽ��
	GPIO_InitStructure.Pin = OV2640_LED_PIN;	  // PF8�������
	GPIO_InitStructure.Mode = GPIO_Mode_OUT;	  // �������
	GPIO_InitStructure.Speed = GPIO_Speed_100MHz; // 100MHz
	GPIO_InitStructure.Pull = GPIO_PuPd_UP;		  // ����
	GPIO_Init(GPIOF, &GPIO_InitStructure);		  // ��ʼ��

#if OV2640_FILL_LIGHT
	OV2640_LED_light = 0; // �رղ���LED
	OV2640_LED_light = 1; // ��������LED
	OV2640_LED_light = 0; // �رղ���LED
	OV2640_LED_light = 1; // ��������LED
#endif

	OV2640_PWDN(0); // POWER ON
	delay_ms(10);
	OV2640_RST(0);									  // ��λOV2640
	OV2640_RST(1);									  // ������λ
	BSP_SCCB_Init();								  // ��ʼ��SCCB ��IO��
	BSP_SCCB_WriteRegister(OV2640_DSP_RA_DLMT, 0x01); // ����sensor�Ĵ���
	BSP_SCCB_WriteRegister(OV2640_SENSOR_COM7, 0x80); // ��λOV2640
	delay_ms(50);
	reg = BSP_SCCB_ReadRegister(OV2640_SENSOR_MIDH); // ��ȡ����ID �߰�λ
	reg <<= 8;
	reg |= BSP_SCCB_ReadRegister(OV2640_SENSOR_MIDL); // ��ȡ����ID �Ͱ�λ
	if (reg != OV2640_MID)
	{
		printf("MID:%d\r\n", reg);
		return 1;
	}
	reg = BSP_SCCB_ReadRegister(OV2640_SENSOR_PIDH); // ��ȡ����ID �߰�λ
	reg <<= 8;
	reg |= BSP_SCCB_ReadRegister(OV2640_SENSOR_PIDL); // ��ȡ����ID �Ͱ�λ
	if ((reg != OV2640_PID1) & (reg != OV2640_PID2))
	{
		printf("HID:%d\r\n", reg);
		return 2;
	}
	// ��ʼ�� OV2640,����SXGA�ֱ���(1600*1200)
	for (i = 0; i < sizeof(ov2640_sxga_init_reg_tbl) / 2; i++)
	{
		BSP_SCCB_WriteRegister(ov2640_sxga_init_reg_tbl[i][0], ov2640_sxga_init_reg_tbl[i][1]);
		if (i < 10)
			delay_ms(5); // д���ʼ�������ݵ�ʱ�򣬼���ʱ���������ò�����
	}
	return 0x00; // ok
}

/**
 * @brief OV2640�л�ΪJPEGģʽ
 */
void OV2640_JPEG_Mode(void)
{
	u16 i = 0;
	// ����:YUV422��ʽ
	for (i = 0; i < (sizeof(ov2640_yuv422_reg_tbl) / 2); i++)
	{
		BSP_SCCB_WriteRegister(ov2640_yuv422_reg_tbl[i][0], ov2640_yuv422_reg_tbl[i][1]);
	}

	// ����:���JPEG����
	for (i = 0; i < (sizeof(ov2640_jpeg_reg_tbl) / 2); i++)
	{
		BSP_SCCB_WriteRegister(ov2640_jpeg_reg_tbl[i][0], ov2640_jpeg_reg_tbl[i][1]);
	}
}

/**
 * @brief OV2640�л�ΪRGB565ģʽ
 */
void OV2640_RGB565_Mode(void)
{
	u16 i = 0;
	// ����:RGB565���
	for (i = 0; i < (sizeof(ov2640_rgb565_reg_tbl) / 2); i++)
	{
		BSP_SCCB_WriteRegister(ov2640_rgb565_reg_tbl[i][0], ov2640_rgb565_reg_tbl[i][1]);
	}
}
/* �Զ��ع����ò�����,֧��5���ȼ� */
const static u8 OV2640_AUTOEXPOSURE_LEVEL[5][8] = {
	{
		0xFF,
		0x01,
		0x24,
		0x20,
		0x25,
		0x18,
		0x26,
		0x60,
	},
	{
		0xFF,
		0x01,
		0x24,
		0x34,
		0x25,
		0x1c,
		0x26,
		0x00,
	},
	{
		0xFF,
		0x01,
		0x24,
		0x3e,
		0x25,
		0x38,
		0x26,
		0x81,
	},
	{
		0xFF,
		0x01,
		0x24,
		0x48,
		0x25,
		0x40,
		0x26,
		0x81,
	},
	{
		0xFF,
		0x01,
		0x24,
		0x58,
		0x25,
		0x50,
		0x26,
		0x92,
	},
};

/**
 * @brief OV2640�Զ��ع�ȼ�����
 * @param level 0~4
 */
void OV2640_Auto_Exposure(u8 level)
{
	u8 i;
	u8 *p = (u8 *)OV2640_AUTOEXPOSURE_LEVEL[level];
	for (i = 0; i < 4; i++)
	{
		BSP_SCCB_WriteRegister(p[i * 2], p[i * 2 + 1]);
	}
}

/**
 * @brief ��ƽ������
 * @param mode	| 0-�Զ�
 * 				| 1-̫��sunny
 * 				| 2-����cloudy
 * 				| 3-�칫��office
 * 			 	| 4-����home
 */
void OV2640_Light_Mode(u8 mode)
{
	u8 regccval = 0X5E; // Sunny
	u8 regcdval = 0X41;
	u8 regceval = 0X54;
	switch (mode)
	{
	case 0: // auto
		BSP_SCCB_WriteRegister(0XFF, 0X00);
		BSP_SCCB_WriteRegister(0XC7, 0X00); // AWB ON
		return;
	case 2: // cloudy
		regccval = 0X65;
		regcdval = 0X41;
		regceval = 0X4F;
		break;
	case 3: // office
		regccval = 0X52;
		regcdval = 0X41;
		regceval = 0X66;
		break;
	case 4: // home
		regccval = 0X42;
		regcdval = 0X3F;
		regceval = 0X71;
		break;
	}
	BSP_SCCB_WriteRegister(0XFF, 0X00);
	BSP_SCCB_WriteRegister(0XC7, 0X40); // AWB OFF
	BSP_SCCB_WriteRegister(0XCC, regccval);
	BSP_SCCB_WriteRegister(0XCD, regcdval);
	BSP_SCCB_WriteRegister(0XCE, regceval);
}

/**
 * @brief ɫ������
 * @param sat	| 0:-2
 * 				| 1:-1
 * 				| 2:0
 * 				| 3:+1
 * 			 	| 4:+2
 */
void OV2640_Color_Saturation(u8 sat)
{
	u8 reg7dval = ((sat + 2) << 4) | 0X08;
	BSP_SCCB_WriteRegister(0XFF, 0X00);
	BSP_SCCB_WriteRegister(0X7C, 0X00);
	BSP_SCCB_WriteRegister(0X7D, 0X02);
	BSP_SCCB_WriteRegister(0X7C, 0X03);
	BSP_SCCB_WriteRegister(0X7D, reg7dval);
	BSP_SCCB_WriteRegister(0X7D, reg7dval);
}

/**
 * @brief ��������
 * @param bright | 0:(0X00)-2
 *				 | 1:(0X10)-1
 *				 | 2:(0X20) 0
 *				 | 3:(0X30)+1
 *				 | 4:(0X40)+2
 */
void OV2640_Brightness(u8 bright)
{
	BSP_SCCB_WriteRegister(0xff, 0x00);
	BSP_SCCB_WriteRegister(0x7c, 0x00);
	BSP_SCCB_WriteRegister(0x7d, 0x04);
	BSP_SCCB_WriteRegister(0x7c, 0x09);
	BSP_SCCB_WriteRegister(0x7d, bright << 4);
	BSP_SCCB_WriteRegister(0x7d, 0x00);
}

// �Աȶ�����
// 0:-2
// 1:-1
// 2,0
// 3,+1
// 4,+2
void OV2640_Contrast(u8 contrast)
{
	u8 reg7d0val = 0X20; // Ĭ��Ϊ��ͨģʽ
	u8 reg7d1val = 0X20;
	switch (contrast)
	{
	case 0: //-2
		reg7d0val = 0X18;
		reg7d1val = 0X34;
		break;
	case 1: //-1
		reg7d0val = 0X1C;
		reg7d1val = 0X2A;
		break;
	case 3: // 1
		reg7d0val = 0X24;
		reg7d1val = 0X16;
		break;
	case 4: // 2
		reg7d0val = 0X28;
		reg7d1val = 0X0C;
		break;
	}
	BSP_SCCB_WriteRegister(0xff, 0x00);
	BSP_SCCB_WriteRegister(0x7c, 0x00);
	BSP_SCCB_WriteRegister(0x7d, 0x04);
	BSP_SCCB_WriteRegister(0x7c, 0x07);
	BSP_SCCB_WriteRegister(0x7d, 0x20);
	BSP_SCCB_WriteRegister(0x7d, reg7d0val);
	BSP_SCCB_WriteRegister(0x7d, reg7d1val);
	BSP_SCCB_WriteRegister(0x7d, 0x06);
}

// ��Ч����
// 0:��ͨģʽ
// 1,��Ƭ
// 2,�ڰ�
// 3,ƫ��ɫ
// 4,ƫ��ɫ
// 5,ƫ��ɫ
// 6,����
void OV2640_Special_Effects(u8 eft)
{
	u8 reg7d0val = 0X00; // Ĭ��Ϊ��ͨģʽ
	u8 reg7d1val = 0X80;
	u8 reg7d2val = 0X80;
	switch (eft)
	{
	case 1: // ��Ƭ
		reg7d0val = 0X40;
		break;
	case 2: // �ڰ�
		reg7d0val = 0X18;
		break;
	case 3: // ƫ��ɫ
		reg7d0val = 0X18;
		reg7d1val = 0X40;
		reg7d2val = 0XC0;
		break;
	case 4: // ƫ��ɫ
		reg7d0val = 0X18;
		reg7d1val = 0X40;
		reg7d2val = 0X40;
		break;
	case 5: // ƫ��ɫ
		reg7d0val = 0X18;
		reg7d1val = 0XA0;
		reg7d2val = 0X40;
		break;
	case 6: // ����
		reg7d0val = 0X18;
		reg7d1val = 0X40;
		reg7d2val = 0XA6;
		break;
	}
	BSP_SCCB_WriteRegister(0xff, 0x00);
	BSP_SCCB_WriteRegister(0x7c, 0x00);
	BSP_SCCB_WriteRegister(0x7d, reg7d0val);
	BSP_SCCB_WriteRegister(0x7c, 0x05);
	BSP_SCCB_WriteRegister(0x7d, reg7d1val);
	BSP_SCCB_WriteRegister(0x7d, reg7d2val);
}

// ��������
// sw:0,�رղ���
//    1,��������(ע��OV2640�Ĳ����ǵ�����ͼ�������)
void OV2640_Color_Bar(u8 sw)
{
	u8 reg;
	BSP_SCCB_WriteRegister(0XFF, 0X01);
	reg = BSP_SCCB_ReadRegister(0X12);
	reg &= ~(1 << 1);
	if (sw)
		reg |= 1 << 1;
	BSP_SCCB_WriteRegister(0X12, reg);
}

// ����ͼ���������
// sx,sy,��ʼ��ַ
// width,height:���(��Ӧ:horizontal)�͸߶�(��Ӧ:vertical)
void OV2640_Window_Set(u16 sx, u16 sy, u16 width, u16 height)
{
	u16 endx;
	u16 endy;
	u8 temp;
	endx = sx + width / 2; // V*2
	endy = sy + height / 2;

	BSP_SCCB_WriteRegister(0XFF, 0X01);
	temp = BSP_SCCB_ReadRegister(0X03); // ��ȡVref֮ǰ��ֵ
	temp &= 0XF0;
	temp |= ((endy & 0X03) << 2) | (sy & 0X03);
	BSP_SCCB_WriteRegister(0X03, temp);		 // ����Vref��start��end�����2λ
	BSP_SCCB_WriteRegister(0X19, sy >> 2);	 // ����Vref��start��8λ
	BSP_SCCB_WriteRegister(0X1A, endy >> 2); // ����Vref��end�ĸ�8λ

	temp = BSP_SCCB_ReadRegister(0X32); // ��ȡHref֮ǰ��ֵ
	temp &= 0XC0;
	temp |= ((endx & 0X07) << 3) | (sx & 0X07);
	BSP_SCCB_WriteRegister(0X32, temp);		 // ����Href��start��end�����3λ
	BSP_SCCB_WriteRegister(0X17, sx >> 3);	 // ����Href��start��8λ
	BSP_SCCB_WriteRegister(0X18, endx >> 3); // ����Href��end�ĸ�8λ
}

// ����ͼ�������С
// OV2640���ͼ��Ĵ�С(�ֱ���),��ȫ�ɸĺ���ȷ��
// width,height:���(��Ӧ:horizontal)�͸߶�(��Ӧ:vertical),width��height������4�ı���
// ����ֵ:0,���óɹ�
//     ����,����ʧ��
u8 OV2640_OutSize_Set(u16 width, u16 height)
{
	u16 outh;
	u16 outw;
	u8 temp;

	if (width % 4)
		return 1;
	if (height % 4)
		return 2;

	outw = width / 4;
	outh = height / 4;

	BSP_SCCB_WriteRegister(0XFF, 0X00);
	BSP_SCCB_WriteRegister(0XE0, 0X04);
	BSP_SCCB_WriteRegister(0X5A, outw & 0XFF); // ����OUTW�ĵͰ�λ
	BSP_SCCB_WriteRegister(0X5B, outh & 0XFF); // ����OUTH�ĵͰ�λ
	temp = (outw >> 8) & 0X03;
	temp |= (outh >> 6) & 0X04;
	BSP_SCCB_WriteRegister(0X5C, temp); // ����OUTH/OUTW�ĸ�λ
	BSP_SCCB_WriteRegister(0XE0, 0X00);
	return 0;
}

// ����ͼ�񿪴���С
// ��:OV2640_ImageSize_Setȷ������������ֱ��ʴӴ�С.
// �ú������������Χ������п���,����OV2640_OutSize_Set�����
// ע��:�������Ŀ�Ⱥ͸߶�,������ڵ���OV2640_OutSize_Set�����Ŀ�Ⱥ͸߶�
//      OV2640_OutSize_Set���õĿ�Ⱥ͸߶�,���ݱ��������õĿ�Ⱥ͸߶�,��DSP
//      �Զ��������ű���,������ⲿ�豸.
// width,height:���(��Ӧ:horizontal)�͸߶�(��Ӧ:vertical),width��height������4�ı���
// ����ֵ:0,���óɹ�
//     ����,����ʧ��
u8 OV2640_ImageWin_Set(u16 offx, u16 offy, u16 width, u16 height)
{
	u16 hsize;
	u16 vsize;
	u8 temp;
	if (width % 4)
		return 1;
	if (height % 4)
		return 2;
	hsize = width / 4;
	vsize = height / 4;
	BSP_SCCB_WriteRegister(0XFF, 0X00);
	BSP_SCCB_WriteRegister(0XE0, 0X04);
	BSP_SCCB_WriteRegister(0X51, hsize & 0XFF); // ����H_SIZE�ĵͰ�λ
	BSP_SCCB_WriteRegister(0X52, vsize & 0XFF); // ����V_SIZE�ĵͰ�λ
	BSP_SCCB_WriteRegister(0X53, offx & 0XFF);	// ����offx�ĵͰ�λ
	BSP_SCCB_WriteRegister(0X54, offy & 0XFF);	// ����offy�ĵͰ�λ
	temp = (vsize >> 1) & 0X80;
	temp |= (offy >> 4) & 0X70;
	temp |= (hsize >> 5) & 0X08;
	temp |= (offx >> 8) & 0X07;
	BSP_SCCB_WriteRegister(0X55, temp);				   // ����H_SIZE/V_SIZE/OFFX,OFFY�ĸ�λ
	BSP_SCCB_WriteRegister(0X57, (hsize >> 2) & 0X80); // ����H_SIZE/V_SIZE/OFFX,OFFY�ĸ�λ
	BSP_SCCB_WriteRegister(0XE0, 0X00);
	return 0;
}

// �ú�������ͼ��ߴ��С,Ҳ������ѡ��ʽ������ֱ���
// UXGA:1600*1200,SVGA:800*600,CIF:352*288
// width,height:ͼ���Ⱥ�ͼ��߶�
// ����ֵ:0,���óɹ�
//     ����,����ʧ��
u8 OV2640_ImageSize_Set(u16 width, u16 height)
{
	u8 temp;
	BSP_SCCB_WriteRegister(0XFF, 0X00);
	BSP_SCCB_WriteRegister(0XE0, 0X04);
	BSP_SCCB_WriteRegister(0XC0, (width) >> 3 & 0XFF);	// ����HSIZE��10:3λ
	BSP_SCCB_WriteRegister(0XC1, (height) >> 3 & 0XFF); // ����VSIZE��10:3λ
	temp = (width & 0X07) << 3;
	temp |= height & 0X07;
	temp |= (width >> 4) & 0X80;
	BSP_SCCB_WriteRegister(0X8C, temp);
	BSP_SCCB_WriteRegister(0XE0, 0X00);
	return 0;
}

/**
 * @brief OV2640 JPEG���ڴ���
 * @attention ������ 921600
 */
uint8_t BSP_OV2640_JPEGToUART(void)
{
#ifndef __RTOS_RTTHREAD_ENABLED
	if (huart3.Init.BaudRate != 921600)
	{
		printf("\nError:\n");
		printf("- The baud rate is %d now.\n", huart3.Init.BaudRate);
		printf("- The baud rate has to be 921600!!!\n");
		return ERROR;
	}
#endif /* !__RTOS_RTTHREAD_ENABLED */

	OV2640_JPEG_Mode();
	OV2640_OutSize_Set(jpeg_img_size_tbl[5][0], jpeg_img_size_tbl[5][1]);
	BSP_DCMI_Start(DCMI_MODE_SNAPSHOT, (uint32_t)&jpeg_buf, JPEG_BUFFER_SIZE);
	return SUCCESS;
}

/**
 * @brief OV2640 RGB565 LCD��Ļ��ʾ
 * @attention ������ 115200
 */
uint8_t BSP_OV2640_RGB565ToLCD(void)
{
#if DCMI_USE_LCD
	OV2640_RGB565_Mode();
	OV2640_OutSize_Set(lcddev.width, lcddev.height);
	BSP_DCMI_Start(DCMI_MODE_CONTINUOUS, (uint32_t)&LCD->LCD_RAM, 1);
#endif /* !DCMI_USE_LCD */
	return SUCCESS;
}

__IO uint32_t jpeg_head = 0;
__IO uint32_t headok = 0;
__IO uint32_t jpeg_len = 0;
uint8_t *jpeg_file = (uint8_t *)jpeg_buf;

/**
 * @brief OV2640 ���ݴ������
 */
void BSP_OV2640_Controller(void)
{
	headok = 0;
	jpeg_head = 0;
	/* JPEG ���ڷ��� */
	if (jpeg_data_state == 1)
	{
		for (uint32_t x = 0; x < jpeg_data_len * 4; x++)
		{
			if ((jpeg_file[x] == 0XFF) && (jpeg_file[x + 1] == 0XD8))
			{
				jpeg_head = x;
				headok = 1;
			}
			if ((jpeg_file[x] == 0XFF) && (jpeg_file[x + 1] == 0XD9) && headok)
			{
				jpeg_len = x - jpeg_head + 2;
				headok = 0;
				break;
			}
		}
		if (jpeg_len)
		{
			jpeg_file += jpeg_head;
			for (uint32_t x = 0; x < jpeg_len; x++)
			{
				while ((USART3->ISR & USART_ISR_TC) == 0);
				USART3->TDR = jpeg_file[x];
			}
		}
		jpeg_data_state = 2;
		BSP_DCMI_Start(DCMI_MODE_SNAPSHOT, (u32)&jpeg_buf, JPEG_BUFFER_SIZE);
		/* ! ��Ҫ��ʱ MIN: 50ms ! */
		delay_ms(100);
		__BSP_LED1_Ficker(100);
	}
}