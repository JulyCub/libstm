/**
 * @file ov2640.c
 * @author July (Email: JulyCub@163.com)
 * @brief OV2640 Driver
 * @version 0.1
 * @date 2023.07.16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "main.h"
#include "dcmi.h"
#include "usart.h"
#include "bsp_ov2640.h"
#include "bsp_ov2640cfg.h"
#include "bsp_delay.h"
#include "bsp_sccb.h"

extern DMA_HandleTypeDef hdma_dcmi;
extern DMA_HandleTypeDef hdma_usart1_tx;

/* ����ͷ ID */
uint16_t Camera_ID;
/* ����ͷ�������-��ǰ�к� */
uint16_t curline = 0;
/* ������Ƭ��״̬ */
uint8_t snape_one = 0;
/* JPEG���ֽڳ��� */
uint32_t JPEG_CHARSIZE;
/* JPEG buffer size */
uint32_t JPEG_FILE_LENGTH;
/* ����һ�� */
uint32_t RGB_Line_DATA[RGB_WIDTH / 2] __attribute__((aligned(4))) __attribute__((section("0x24040000"))) = {0};
/* ����һ��Ļ���� */
uint16_t RGB_DATA[RGB_HEIGHT][RGB_WIDTH] __attribute__((aligned(4))) __attribute__((section("0x24040000"))) = {0};
/* JPEG���ݻ���buf */
uint32_t JPEG_DATA[JPEG_BUF_SIZE] __attribute__((aligned(4))) __attribute__((section("0x24040000"))) = {0};
/* 7����Ч */
const uint8_t *EFFECTS_TBL[7] = {
	(uint8_t *)"Normal", (uint8_t *)"Cool", (uint8_t *)"Warm",
	(uint8_t *)"B&W", (uint8_t *)"Yellowish ", (uint8_t *)"Inverse",
	(uint8_t *)"Greenish"};
/* JPEGͼƬ12�ֳߴ� */
const uint8_t *JPEG_SIZE_TYPE[12] = {
	(uint8_t *)"QQVGA", (uint8_t *)"QVGA", (uint8_t *)"VGA", (uint8_t *)"SVGA",
	(uint8_t *)"XGA", (uint8_t *)"WXGA", (uint8_t *)"WXGA+", (uint8_t *)"SXGA",
	(uint8_t *)"UXGA", (uint8_t *)"1080P", (uint8_t *)"QXGA", (uint8_t *)"500W"};
/* JPEG�ߴ�֧���б� */
const uint16_t JPEG_SIZE_TBL[][2] =
	{
		160, 120,	// QQVGA
		240, 240,	// QVGA
		320, 240,	// QVGA
		640, 480,	// VGA
		800, 600,	// SVGA
		1024, 768,	// XGA
		1280, 800,	// WXGA
		1440, 900,	// WXGA+
		1280, 1024, // SXGA
		1600, 1200, // UXGA
		1920, 1080, // 1080P
		2048, 1536, // QXGA
		2592, 1944, // 500W
};

/**
 * @brief ��ʼ�� OV2640
 * @attention �������Ժ�,Ĭ�������1600*1200�ߴ��ͼƬ
 * @attention ��ʹ��CubeMX����DCMI���Զ����ɵ�������Ҫ����
 * @return 0-�ɹ� | ����-�������
 */
u8 OV2640_Init(void)
{
	u16 i = 0;

	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOA_CLK_ENABLE(); // ����GPIOAʱ��
	__HAL_RCC_GPIOC_CLK_ENABLE(); // ����GPIOCʱ��

	GPIO_Initure.Pin = GPIO_PIN_5 | GPIO_PIN_7;		// PA5--CH1    PA7--PWDN
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;		// �������
	GPIO_Initure.Pull = GPIO_PULLUP;				// ����
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // ����
	HAL_GPIO_Init(GPIOA, &GPIO_Initure);			// ��ʼ��

	GPIO_Initure.Pin = GPIO_PIN_4;					// PC4--RST ��λ����
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;		// �������
	GPIO_Initure.Pull = GPIO_PULLUP;				// ����
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // ����
	HAL_GPIO_Init(GPIOC, &GPIO_Initure);			// ��ʼ��

	OV2640_Flash_Ctrl(0); // �رղ���LED
	OV2640_Flash_Ctrl(1); // ��������LED
	delay_ms(500);
	OV2640_Flash_Ctrl(0); // �رղ���LED
	delay_ms(200);

	OV2640_PWDN(0); // POWER ON
	delay_ms(10);
	OV2640_RST(0); // ��λOV2640
	delay_ms(200);
	OV2640_RST(1); // ������λ
	delay_ms(200);

	SCCB_Init(); // ��ʼ��SCCB ��IO��

	SCCB_WR_Reg(OV2640_DSP_RA_DLMT, 0x01); // ����sensor�Ĵ���
	SCCB_WR_Reg(OV2640_SENSOR_COM7, 0x80); // ��λOV2640
	delay_ms(50);

	Camera_ID = SCCB_RD_Reg(OV2640_SENSOR_MIDH); // ��ȡ����ID �߰�λ
	Camera_ID <<= 8;
	Camera_ID |= SCCB_RD_Reg(OV2640_SENSOR_MIDL); // ��ȡ����ID �Ͱ�λ

	if (Camera_ID != OV2640_MID)
	{
		// printf("MID:%d\r\n",reg);
		return 1;
	}
	Camera_ID = SCCB_RD_Reg(OV2640_SENSOR_PIDH); // ��ȡ����ID �߰�λ
	Camera_ID <<= 8;
	Camera_ID |= SCCB_RD_Reg(OV2640_SENSOR_PIDL); // ��ȡ����ID �Ͱ�λ

	if ((Camera_ID != OV2640_PID1) & (Camera_ID != OV2640_PID2))
	{
		// printf("HID:%d\r\n",reg);
		return 2;
	}

	// ��ʼ�� OV2640,����SXGA�ֱ���(1600*1200)
	for (i = 0; i < sizeof(ov2640_sxga_init_reg_tbl) / 2; i++)
	{
		SCCB_WR_Reg(ov2640_sxga_init_reg_tbl[i][0], ov2640_sxga_init_reg_tbl[i][1]);

		if (i < 10)
			delay_ms(5); // д���ʼ�������ݵ�ʱ�򣬼���ʱ���������ò�����
	}

	return 0x00; // ok
}

/**
 * @brief ����ƿ���
 * @param sw  0-close 1-open
 */
void OV2640_Flash_Ctrl(u8 sw)
{
	if (sw == 0)
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // PA5--CH1 ��0
	else
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // PA5--CH1 ��1
}

/**
 * @brief �л�ΪJPEGģʽ
 */
void OV2640_JPEG_Mode(void)
{
	u16 i = 0;
	// ����:YUV422��ʽ
	for (i = 0; i < (sizeof(ov2640_yuv422_reg_tbl) / 2); i++)
	{
		SCCB_WR_Reg(ov2640_yuv422_reg_tbl[i][0], ov2640_yuv422_reg_tbl[i][1]);
	}

	// ����:���JPEG����
	for (i = 0; i < (sizeof(ov2640_jpeg_reg_tbl) / 2); i++)
	{
		SCCB_WR_Reg(ov2640_jpeg_reg_tbl[i][0], ov2640_jpeg_reg_tbl[i][1]);
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
		SCCB_WR_Reg(ov2640_rgb565_reg_tbl[i][0], ov2640_rgb565_reg_tbl[i][1]);
	}
}

/**
 * @brief �Զ��ع����ò�����,֧��5���ȼ�
 */
const static u8 OV2640_AUTOEXPOSURE_LEVEL[5][8] =
	{
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
		SCCB_WR_Reg(p[i * 2], p[i * 2 + 1]);
	}
}

/**
 * @brief ��ƽ������
 * @param mode 0:�Զ�
 * 1:̫��sunny
 * 2,����cloudy
 * 3,�칫��office
 * 4,����home
 */
void OV2640_Light_Mode(u8 mode)
{
	u8 regccval = 0X5E; // Sunny
	u8 regcdval = 0X41;
	u8 regceval = 0X54;
	switch (mode)
	{
	case 0: // auto
		SCCB_WR_Reg(0XFF, 0X00);
		SCCB_WR_Reg(0XC7, 0X00); // AWB ON
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
	SCCB_WR_Reg(0XFF, 0X00);
	SCCB_WR_Reg(0XC7, 0X40); // AWB OFF
	SCCB_WR_Reg(0XCC, regccval);
	SCCB_WR_Reg(0XCD, regcdval);
	SCCB_WR_Reg(0XCE, regceval);
}

/**
 * @brief ɫ������
 * @param sat
 * 		0:-2
 * 		1:-1
 * 		2,0
 * 		3,+1
 * 		4,+2
 */
void OV2640_Color_Saturation(u8 sat)
{
	u8 reg7dval = ((sat + 2) << 4) | 0X08;
	SCCB_WR_Reg(0XFF, 0X00);
	SCCB_WR_Reg(0X7C, 0X00);
	SCCB_WR_Reg(0X7D, 0X02);
	SCCB_WR_Reg(0X7C, 0X03);
	SCCB_WR_Reg(0X7D, reg7dval);
	SCCB_WR_Reg(0X7D, reg7dval);
}

/**
 * @brief ��������
 * @param bright
 * 	0:(0X00)-2
 *	1:(0X10)-1
 *	2,(0X20) 0
 *	3,(0X30)+1
 *	4,(0X40)+2
 */
void OV2640_Brightness(u8 bright)
{
	SCCB_WR_Reg(0xff, 0x00);
	SCCB_WR_Reg(0x7c, 0x00);
	SCCB_WR_Reg(0x7d, 0x04);
	SCCB_WR_Reg(0x7c, 0x09);
	SCCB_WR_Reg(0x7d, bright << 4);
	SCCB_WR_Reg(0x7d, 0x00);
}

/**
 * @brief �Աȶ�����
 * @param contrast
 *	0:-2
 *	1:-1
 *	2,0
 *	3,+1
 *	4,+2
 */
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
	SCCB_WR_Reg(0xff, 0x00);
	SCCB_WR_Reg(0x7c, 0x00);
	SCCB_WR_Reg(0x7d, 0x04);
	SCCB_WR_Reg(0x7c, 0x07);
	SCCB_WR_Reg(0x7d, 0x20);
	SCCB_WR_Reg(0x7d, reg7d0val);
	SCCB_WR_Reg(0x7d, reg7d1val);
	SCCB_WR_Reg(0x7d, 0x06);
}

/**
 * @brief ��Ч����
 * @param eft
 * 0:��ͨģʽ
 * 1,��Ƭ
 * 2,�ڰ�
 * 3,ƫ��ɫ
 * 4,ƫ��ɫ
 * 5,ƫ��ɫ
 * 6,����
 */
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
	SCCB_WR_Reg(0xff, 0x00);
	SCCB_WR_Reg(0x7c, 0x00);
	SCCB_WR_Reg(0x7d, reg7d0val);
	SCCB_WR_Reg(0x7c, 0x05);
	SCCB_WR_Reg(0x7d, reg7d1val);
	SCCB_WR_Reg(0x7d, reg7d2val);
}

/**
 * @brief ��������
 * @param sw 0-�رղ��� 1-��������
 */
void OV2640_Color_Bar(u8 sw)
{
	u8 reg;
	SCCB_WR_Reg(0XFF, 0X01);
	reg = SCCB_RD_Reg(0X12);
	reg &= ~(1 << 1);
	if (sw)
		reg |= 1 << 1;
	SCCB_WR_Reg(0X12, reg);
}

/**
 * @brief ����ͼ���������
 *
 * @param sx ��ʼ��ַx
 * @param sy ��ʼ��ַy
 * @param width ���-horizontal
 * @param height �߶�-vertical
 */
void OV2640_Window_Set(u16 sx, u16 sy, u16 width, u16 height)
{
	u16 endx;
	u16 endy;
	u8 temp;
	endx = sx + width / 2; // V*2
	endy = sy + height / 2;

	SCCB_WR_Reg(0XFF, 0X01);
	temp = SCCB_RD_Reg(0X03); // ��ȡVref֮ǰ��ֵ
	temp &= 0XF0;
	temp |= ((endy & 0X03) << 2) | (sy & 0X03);
	SCCB_WR_Reg(0X03, temp);	  // ����Vref��start��end�����2λ
	SCCB_WR_Reg(0X19, sy >> 2);	  // ����Vref��start��8λ
	SCCB_WR_Reg(0X1A, endy >> 2); // ����Vref��end�ĸ�8λ

	temp = SCCB_RD_Reg(0X32); // ��ȡHref֮ǰ��ֵ
	temp &= 0XC0;
	temp |= ((endx & 0X07) << 3) | (sx & 0X07);
	SCCB_WR_Reg(0X32, temp);	  // ����Href��start��end�����3λ
	SCCB_WR_Reg(0X17, sx >> 3);	  // ����Href��start��8λ
	SCCB_WR_Reg(0X18, endx >> 3); // ����Href��end�ĸ�8λ
}

/**
 * @brief ����ͼ�������С
 *
 * @param width ���-horizontal
 * @param height �߶�-vertical
 * @return u8 0-�ɹ� other-ʧ��
 */
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

	SCCB_WR_Reg(0XFF, 0X00);
	SCCB_WR_Reg(0XE0, 0X04);
	SCCB_WR_Reg(0X5A, outw & 0XFF); // ����OUTW�ĵͰ�λ
	SCCB_WR_Reg(0X5B, outh & 0XFF); // ����OUTH�ĵͰ�λ
	temp = (outw >> 8) & 0X03;
	temp |= (outh >> 6) & 0X04;
	SCCB_WR_Reg(0X5C, temp); // ����OUTH/OUTW�ĸ�λ
	SCCB_WR_Reg(0XE0, 0X00);
	return 0;
}

/**
 * @brief ����ͼ�񿪴���С
 * @details OV2640_ImageSize_Setȷ������������ֱ��ʴӴ�С,�ú���������
 * 			����Χ������п���,����OV2640_OutSize_Set�����
 * @attention �������Ŀ�Ⱥ͸߶�,������ڵ���OV2640_OutSize_Set�����Ŀ�Ⱥ͸߶�
 * 			  OV2640_OutSize_Set���õĿ�Ⱥ͸߶�,���ݱ��������õĿ�Ⱥ͸߶�,��DSP
 *			  �Զ��������ű���,������ⲿ�豸.
 * @param offx
 * @param offy
 * @param width ��� ����Ϊ4�ı���
 * @param height �߶� ����Ϊ4�ı���
 * @return u8 0-�ɹ� other-ʧ��
 */
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
	SCCB_WR_Reg(0XFF, 0X00);
	SCCB_WR_Reg(0XE0, 0X04);
	SCCB_WR_Reg(0X51, hsize & 0XFF); // ����H_SIZE�ĵͰ�λ
	SCCB_WR_Reg(0X52, vsize & 0XFF); // ����V_SIZE�ĵͰ�λ
	SCCB_WR_Reg(0X53, offx & 0XFF);	 // ����offx�ĵͰ�λ
	SCCB_WR_Reg(0X54, offy & 0XFF);	 // ����offy�ĵͰ�λ
	temp = (vsize >> 1) & 0X80;
	temp |= (offy >> 4) & 0X70;
	temp |= (hsize >> 5) & 0X08;
	temp |= (offx >> 8) & 0X07;
	SCCB_WR_Reg(0X55, temp);				// ����H_SIZE/V_SIZE/OFFX,OFFY�ĸ�λ
	SCCB_WR_Reg(0X57, (hsize >> 2) & 0X80); // ����H_SIZE/V_SIZE/OFFX,OFFY�ĸ�λ
	SCCB_WR_Reg(0XE0, 0X00);
	return 0;
}

/**
 * @brief �ú�������ͼ��ߴ��С,Ҳ������ѡ��ʽ������ֱ���
 * 		  UXGA:1600*1200,SVGA:800*600,CIF:352*288
 *
 * @param width ���
 * @param height �߶�
 * @return u8 0-�ɹ� other-ʧ��
 */
u8 OV2640_ImageSize_Set(u16 width, u16 height)
{
	u8 temp;
	SCCB_WR_Reg(0XFF, 0X00);
	SCCB_WR_Reg(0XE0, 0X04);
	SCCB_WR_Reg(0XC0, (width) >> 3 & 0XFF);	 // ����HSIZE��10:3λ
	SCCB_WR_Reg(0XC1, (height) >> 3 & 0XFF); // ����VSIZE��10:3λ
	temp = (width & 0X07) << 3;
	temp |= height & 0X07;
	temp |= (width >> 4) & 0X80;
	SCCB_WR_Reg(0X8C, temp);
	SCCB_WR_Reg(0XE0, 0X00);
	return 0;
}

/**
 * @brief OV2640 JPEG���ڴ���
 * @attention ������ 921600
 */
uint8_t BSP_OV2640_JPEGToUART(void)
{
#if !__RTOS_RTTHREAD_ENABLED
	if (huart1.Init.BaudRate != 921600)
	{
		printf("\nError:\n");
		printf("- The baud rate is %d now.\n", huart1.Init.BaudRate);
		printf("- The baud rate has to be 921600!!!\n");
		return ERROR;
	}
#endif /* !__RTOS_RTTHREAD_ENABLED */

	OV2640_JPEG_Mode();
	delay_ms(300);
	/* ��������ߴ� */
	OV2640_OutSize_Set(JPEG_SIZE_TBL[5][0], JPEG_SIZE_TBL[5][1]);
	delay_ms(300);
	/* ����֡�ж� */
	__HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IER_FRAME_IE);
	/* ����JPEG���մ��� */
	HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)JPEG_DATA, JPEG_BUF_SIZE);
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

/**
 * @brief DCMI֡�жϻص�����-֡ͼ����
 * @param hdcmi dcmi���
 */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	HAL_DCMI_Suspend(hdcmi); // �������,���� DCMI
	HAL_DCMI_Stop(hdcmi);	 // �������,ֹͣDMA����
	snape_one = 1;
}

/**
 * @brief OV2640 JPEGģʽѭ�����ƺ���
 */
void BSP_OV2640_JPEGCONTR(void)
{
	/* �ɼ���һ֡ͼ�� */
	if (1 == snape_one)
	{
		file = (uint8_t *)JPEG_DATA;
		JPEG_FILE_LENGTH = JPEG_BUF_SIZE;
		/* ��ȡJPEG��С */
		while (JPEG_FILE_LENGTH > 0)
		{
			if (JPEG_DATA[JPEG_FILE_LENGTH - 1] != 0x00000000)
				break;
			JPEG_FILE_LENGTH--;
		}
		JPEG_CHARSIZE = JPEG_FILE_LENGTH * 4;
		SCB_CleanDCache_by_Addr((uint32_t *)JPEG_DATA, JPEG_CHARSIZE);
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)JPEG_DATA, JPEG_CHARSIZE);

		while (HAL_DMA_GetState(&hdma_usart1_tx) == HAL_DMA_STATE_BUSY);
		/* JPEG�ֽڳ������� */
		JPEG_CHARSIZE = 0;
		/* ������������־ */
		snape_one = 0;

		/* ��ʼ��һ֡�ɼ� */
		__HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IER_FRAME_IE);
		HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)JPEG_DATA, JPEG_BUF_SIZE);
		__BSP_LED_Ficker(100);
	}
}