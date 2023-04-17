/*
 ******************************************************************************
 * @file		:bsp_coding.h
 * @brief		:The board support package for digital coding.
 * @version		:0.1.0
 * @author		:July
 * @date		:2022.06.27	-v0
 * @undate      :2023.02.27 -v1
 ******************************************************************************
 */

#ifndef __BSP_CODING_H__
#define __BSP_CODING_H__

#include "bsp_config.h"

/* -@- */
#if 0
#define __RZ
#elif 0
#define __NRZ
#elif 0
#define __MCST
#elif 0
#define __DFMCST
#endif

/* �Ϳ������� */
extern int bsp_baker7bit[];
extern int bsp_baker11bit[];
extern int bsp_baker13bit[];
/* �Ϳ��������ϵ�� */
#define XCORR_BAKER_7 7
#define XCORR_BAKER_11 11
#define XCORR_BAKER_13 13

/* arr_charת������ */
void bsp_charToBin(char *num, uint16_t *code, uint8_t len);
/* ʮ����ת������ */
void bsp_DecToBin(int num, uint16_t code[], unsigned char len);
/* ������ת������ */
void bsp_FloatToBin(float num, uint16_t code[]);
void bsp_FloatToBCD(float num, uint16_t code[]);
/* ������� */
_Bool RZ(uint16_t str[], uint16_t code[], char len);
/* ��������� */
_Bool NRZ(uint16_t str[], uint16_t code[], char len);
/* ����˹�ر��� */
_Bool MCST(uint16_t str[], uint16_t code[], char len);
/* ����˹�ؽ��� */
_Bool MCST_Decode(uint16_t code[], uint16_t decode[]);
/* �������˹�ر��� */
_Bool DFMCST(uint16_t str[], uint16_t code[], char len);

#endif
