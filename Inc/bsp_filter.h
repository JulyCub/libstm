/**
 * @file bsp_filter.c
 * @author July (Email: JulyCub@163.com)
 * @brief The board support package for filter.
 * @version 0.1
 * @date 2022.07.02
 * @update 2023.07.24
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __BSP_FILTER_H__
#define __BSP_FILTER_H__

#include "bsp_config.h"

#if __CMSIS_DSP
#include "arm_math.h"

/* FIR */
#define FIR_LENGTH_SAMPLES 4096 /* �������� */
#define FIR_BLOCK_SIZE 1        /* ����һ��arm_fir_f32����Ĳ�������� */
#define FIR_FACTOR_NUM 32       /* �˲���ϵ������ */

/* LMS */
#define LMS_LENGTH_SAMPLES 4096 /* �������� */
#define LMS_BLOCK_SIZE 1        /* ����һ��arm_lms_f32����Ĳ�������� */
#define LMS_FACTOR_NUM 32       /* �˲���ϵ������ */

void bsp_arm_fir_f32(float32_t *data_buff,
                     float32_t *out_buff,
                     float32_t *cache_buff,
                     float32_t *factor,
                     uint32_t sample_num,
                     uint32_t block_size,
                     uint32_t num_taps,
                     uint8_t debug);


#endif

/* ƽ���˲��㷨 */
float Onepointfilter(int16_t data, uint16_t fnum);
void linearSmooth3(double in[], double out[], int N);
void linearSmooth5(double in[], double out[], int N);
void linearSmooth7(double in[], double out[], int N);
void quadraticSmooth5(double in[], double out[], int N);
void quadraticSmooth7(double in[], double out[], int N);
void cubicSmooth5(double in[], double out[], int N);
void cubicSmooth7(double in[], double out[], int N);

#endif
