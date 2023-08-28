/*
 ******************************************************************************
 * @file		:bsp_coding.h
 * @brief		:The board support package for digital coding.
 * @version		:0.2.0
 * @author		:July
 * @date		:2022.06.27	-v0
 * @updated     :2023.02.27 -v1
 * @updated     :2023.02.27 -v2
 ******************************************************************************
 */

#ifndef __BSP_CODING_H__
#define __BSP_CODING_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_config.h"

/* �Ϳ������� */
extern int bsp_baker7bit[];
extern int bsp_baker11bit[];
extern int bsp_baker13bit[];
/* �Ϳ��������ϵ�� */
#define XCORR_BAKER_7 7
#define XCORR_BAKER_11 11
#define XCORR_BAKER_13 13

/* ���뷽ʽѡ�� */
#define ENCODING_MODE_SELECT 0

/* arr_charת������ */
void BSP_charToBin(char *num, uint16_t *code, uint8_t len);
/* ʮ����ת������ */
void BSP_DecToBin(int num, uint16_t code[], unsigned char len);
/* ������ת������ */
void BSP_FloatTo16BCD(float num, uint16_t code[]);
void BSP_FloatTo24BCD(float num, uint16_t code[]);

uint8_t BSP_ParityCheck_OddEncode(uint8_t *ori_data, uint8_t *out_date, uint8_t len);
uint8_t BSP_ParityCheck_EvenEncode(uint8_t *ori_data, uint8_t *out_date, uint8_t len);

void BSP_HanmingEncode(uint8_t *ori_data, uint8_t *out_data, const uint8_t data_len, uint8_t type);


#if ENCODING_MODE_SELECT
/* ������� */
void _RZ(uint16_t str[], uint16_t code[], char len);
/* ��������� */
void _NRZ(uint16_t str[], uint16_t code[], char len);
/* ����˹�ر��� */
void _MCST(uint16_t str[], uint16_t code[], char len);
/* �������˹�ر��� */
void _DFMCST(uint16_t str[], uint16_t code[], char len);
#endif /* !ENCODING_MODE_SELECT */

#ifdef __cplusplus
}
#endif
#endif
