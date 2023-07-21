/**
 ******************************************************************************
 * @file			:bsp_usart_hmi.h
 * @brief			:The board support package for USART HMI.
 * @version			:0.1.3
 * @author			:July
 * @date			:2022.06.26
 ******************************************************************************
 * @updata			:ver 0.1.3
 * @refr			:	stuc -> hmitextTypedef
 * 				 		stuc -> hmicurveTypedef
 * @add				:	stuc -> hmibuttonTypedef
 * 				 		stuc -> hmipageTypedef
 ******************************************************************************
 */

#ifndef __BSP_USART_HMI_H__
#define __BSP_USART_HMI_H__

#include "bsp_config.h"
#include "bsp_usart.h"

/* ����ָ��϶� */
#define HMI_MULT_ODR 20

/* �ı��ؼ� */
typedef struct
{
	const uint8_t page;		  /* ����ҳ�� */
	const uint8_t id;		  /* �ؼ� ID */
	const uint8_t objname[4]; /* �ؼ����� */
	uint8_t drag;			  /* �Ƿ�֧���϶� 0-�� 1-�� */
	uint8_t aph;			  /* ��͸���� */
	uint8_t effect;			  /* ������Ч */
	uint8_t pco;			  /* ����ɫ */
	uint8_t font;			  /* �ֿ� */
	uint8_t pw;				  /* �Ƿ���ʾΪ���� */
} hmitextTypedef;

/* ����/���οؼ� */
typedef struct
{
	const uint8_t page;		  /* ����ҳ�� */
	const uint8_t id;		  /* �ؼ� ID */
	const uint8_t chnl[4];	  /* ͨ�� */
	const uint8_t objname[4]; /* �ؼ����� */
	uint8_t drag;			  /* �Ƿ�֧���϶� 0-�� 1-�� */
	uint8_t aph;			  /* ��͸���� */
	uint8_t effect;			  /* ������Ч */
	uint8_t bco;			  /* ����ɫ */
	uint8_t gdc;			  /* ������ɫ */
	uint8_t gdw;			  /* ������ */
	uint8_t gdh;			  /* ����߶� */
	uint16_t dis;			  /* �������Űٷֱȣ���С10�����1000 */
} hmicurveTypedef;

/* ��ť�ؼ� */
typedef struct
{
	const uint8_t page;		  /* ����ҳ�� */
	const uint8_t id;		  /* �ؼ� ID */
	const uint8_t objname[4]; /* �ؼ����� */
	uint8_t drag;			  /* �Ƿ�֧���϶� 0-�� 1-�� */
	uint8_t aph;			  /* ��͸���� */
	uint8_t effect;			  /* ������Ч */
	uint8_t font;			  /* �ֿ� */
} hmibuttonTypedef;

/* ���ֿؼ� */
typedef struct
{
	const uint8_t page;		  /* ����ҳ�� */
	const uint8_t id;		  /* �ؼ� ID */
	const uint8_t objname[4]; /* �ؼ����� */
	uint8_t drag;			  /* �Ƿ�֧���϶� 0-�� 1-�� */
	uint8_t aph;			  /* ��͸���� */
	uint8_t effect;			  /* ������Ч */
	uint8_t font;			  /* �ֿ� */
} hminumberTypedef;

/* ҳ�� */
typedef struct
{
	uint8_t id;
	uint8_t name[12];
	hmitextTypedef text[8];
	hmicurveTypedef curve;
	hmibuttonTypedef button[4];
} hmipageTypedef;

/* ����/���οؼ�ͨ���궨�� */
#define HMI_CHANNEL_1 1U
#define HMI_CHANNEL_2 2U
#define HMI_CHANNEL_3 3U
#define HMI_CHANNEL_4 4U

/* ���ñ��� */
extern uint8_t hmi_chnl;
extern uint8_t hmi_id;
extern uint8_t hmi_page;
extern uint16_t hmi_point;
extern uint16_t hmi_point_counts;

/* ָ�����֡ */
#define __prifend1 bsprif1("\xff\xff\xff")
#define __prifend2 bsprif2("\xff\xff\xff")
#define __prifend3 bsprif3("\xff\xff\xff")
/* �л�ҳ�� */
#define __switchpage(__PAGE__)                     \
	{                                              \
		bsprif1("page %d", hmi_page = (__PAGE__)); \
		__prifend1;                                 \
	}
/* ���߿ؼ�����һ���� */
#define __sendpoint(__ID__, __CHNL__, __POINT__)                                                    \
	{                                                                                               \
		bsprif1("add %d,%d,%d", hmi_id = (__ID__), hmi_chnl = (__CHNL__), hmi_point = (__POINT__)); \
		__prifend1;                                                                                  \
	}
/* ���߿ؼ���Ӷ����(͸��) */
#define __sendpoints(__ID__, __CHNL__, __COUNT__)                                                           \
	{                                                                                                       \
		bsprif1("addt %d,%d,%d", hmi_id = (__ID__), hmi_chnl = (__CHNL__), hmi_point_counts = (__COUNT__)); \
		__prifend1;                                                                                          \
	}

/* HMI����ָ����� */
uint8_t bsp_HMI_RxOder(uint8_t *odr);
/* ��ͨ������͸������ */
void bsp_HMI_sendCurveSingle(uint8_t id, uint8_t chnl, uint8_t points[], uint16_t len, uint8_t page);

#endif /* __BSP_USART_HMI_H__ */
