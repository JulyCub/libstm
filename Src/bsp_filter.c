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

#include "bsp_filter.h"

#if __CMSIS_DSP

/* �˲�����ͷ�ļ� */
/* MATLAB DIGITALFILTERDESIGNER ������� */
#include "fdacoefs.h"

/**
 * @brief FIR�˲���
 * @details �ĺ������� CMSIS DSP lib��arm_fir_f32_hpʵ�ָ�ͨ/��ͨ/��ͨ/�����˲���
 * @attention �û�������ͨ��MATLAB������������ factor����ز���
 *
 * @param data_buff Դ���ݵ�ַ
 * @param out_buff �˲������ݵ�ַ
 * @param cache_buff �����ַ, �涨��С: num_taps + block_size - 1
 * @param factor �˲�����
 * @param sample_num ��������
 * @param block_size ÿ�δ���Ĳ�������
 * @param num_taps �˲���������
 * @param debug �Ƿ��ӡ�˲�����
 */
void bsp_arm_fir_f32(float32_t *data_buff,
                     float32_t *out_buff, 
                     float32_t *cache_buff,
                     float32_t *factor,
                     uint32_t sample_num,
                     uint32_t block_size,
                     uint32_t num_taps,
                     uint8_t debug)
{
    arm_fir_instance_f32 FIR_InitStruct;

    /* ��ʼ���ṹ��FIR_InitStruct */
    arm_fir_init_f32(&FIR_InitStruct,
                     num_taps,
                     factor,
                     cache_buff,
                     block_size);

    /* ʵ��FIR�˲�������ÿ�δ��� sample_num / block_size ���� */
    for (uint32_t i = 0; i < (sample_num / block_size); i++)
    {
        arm_fir_f32(&FIR_InitStruct,
                    data_buff + (i * block_size),
                    out_buff + (i * block_size), 
                    block_size);
    }

    /* ��ӡ�˲����� */
    if (debug){
        for (uint32_t i = 0; i < sample_num; i++)
        {
            printf("input&output:%f, %f\n", data_buff[i], out_buff[i]);
        }
    }
}

// TODO: LMS�˲���
void bsp_arm_lms_f32(void)
{

}
#endif

/**
 * @brief���㻬��ƽ���˲�
 * @param data ��Ϣ��
 * @param fnum ����������
 * @return float���
 */
float Onepointfilter(int16_t data, const uint16_t fnum)
{
    static int16_t buf[fnum];
    static int16_t index = 0, flag = 0;
    static float sum = 0;

    // �滻֮ǰλ���ϵ�����
    sum += data - buf[index];
    buf[index] = data;
    index++;

    // ��������ѭ�����õ�������
    if (index == fnum)
    {
        index = 0;
        flag = 1;
    }

    // ���û�г������������м�����ȡ������ƽ��
    if (flag == 0)
        return sum / index;
    else
        return sum / fnum;
}

/**
 * @brief ��������ƽ���˲�
 * @param in ��������
 * @param out �������
 * @param N ���鳤��
 */
void linearSmooth3(double in[], double out[], int N)
{
    int i;
    if (N < 3)
    {
        for (i = 0; i <= N - 1; i++)
        {
            out[i] = in[i];
        }
    }
    else
    {
        out[0] = (5.0 * in[0] + 2.0 * in[1] - in[2]) / 6.0;

        for (i = 1; i <= N - 2; i++)
        {
            out[i] = (in[i - 1] + in[i] + in[i + 1]) / 3.0;
        }

        out[N - 1] = (5.0 * in[N - 1] + 2.0 * in[N - 2] - in[N - 3]) / 6.0;
    }
}

/**
 * @brief �������ƽ���˲�
 * @param in ��������
 * @param out �������
 * @param N ���鳤��
 */
void linearSmooth5(double in[], double out[], int N)
{
    int i;
    if (N < 5)
    {
        for (i = 0; i <= N - 1; i++)
        {
            out[i] = in[i];
        }
    }
    else
    {
        out[0] = (3.0 * in[0] + 2.0 * in[1] + in[2] - in[4]) / 5.0;
        out[1] = (4.0 * in[0] + 3.0 * in[1] + 2 * in[2] + in[3]) / 10.0;
        for (i = 2; i <= N - 3; i++)
        {
            out[i] = (in[i - 2] + in[i - 1] + in[i] + in[i + 1] + in[i + 2]) / 5.0;
        }
        out[N - 2] = (4.0 * in[N - 1] + 3.0 * in[N - 2] + 2 * in[N - 3] + in[N - 4]) / 10.0;
        out[N - 1] = (3.0 * in[N - 1] + 2.0 * in[N - 2] + in[N - 3] - in[N - 5]) / 5.0;
    }
}

/**
 * @brief �ߵ�����ƽ���˲�
 * @param in ��������
 * @param out �������
 * @param N ���鳤��
 */
void linearSmooth7(double in[], double out[], int N)
{
    int i;
    if (N < 7)
    {
        for (i = 0; i <= N - 1; i++)
        {
            out[i] = in[i];
        }
    }
    else
    {
        out[0] = (13.0 * in[0] + 10.0 * in[1] + 7.0 * in[2] + 4.0 * in[3] +
                  in[4] - 2.0 * in[5] - 5.0 * in[6]) /
                 28.0;

        out[1] = (5.0 * in[0] + 4.0 * in[1] + 3 * in[2] + 2 * in[3] +
                  in[4] - in[6]) /
                 14.0;

        out[2] = (7.0 * in[0] + 6.0 * in[1] + 5.0 * in[2] + 4.0 * in[3] +
                  3.0 * in[4] + 2.0 * in[5] + in[6]) /
                 28.0;

        for (i = 3; i <= N - 4; i++)
        {
            out[i] = (in[i - 3] + in[i - 2] + in[i - 1] + in[i] + in[i + 1] + in[i + 2] + in[i + 3]) / 7.0;
        }

        out[N - 3] = (7.0 * in[N - 1] + 6.0 * in[N - 2] + 5.0 * in[N - 3] +
                      4.0 * in[N - 4] + 3.0 * in[N - 5] + 2.0 * in[N - 6] + in[N - 7]) /
                     28.0;

        out[N - 2] = (5.0 * in[N - 1] + 4.0 * in[N - 2] + 3.0 * in[N - 3] +
                      2.0 * in[N - 4] + in[N - 5] - in[N - 7]) /
                     14.0;

        out[N - 1] = (13.0 * in[N - 1] + 10.0 * in[N - 2] + 7.0 * in[N - 3] +
                      4 * in[N - 4] + in[N - 5] - 2 * in[N - 6] - 5 * in[N - 7]) /
                     28.0;
    }
}

/**
 * @brief ������ƽ���˲�
 * @param in ��������
 * @param out �������
 * @param N ���鳤��
 */
void quadraticSmooth5(double in[], double out[], int N)
{
    int i;
    if (N < 5)
    {
        for (i = 0; i <= N - 1; i++)
        {
            out[i] = in[i];
        }
    }
    else
    {
        out[0] = (31.0 * in[0] + 9.0 * in[1] - 3.0 * in[2] - 5.0 * in[3] + 3.0 * in[4]) / 35.0;
        out[1] = (9.0 * in[0] + 13.0 * in[1] + 12 * in[2] + 6.0 * in[3] - 5.0 * in[4]) / 35.0;
        for (i = 2; i <= N - 3; i++)
        {
            out[i] = (-3.0 * (in[i - 2] + in[i + 2]) +
                      12.0 * (in[i - 1] + in[i + 1]) + 17 * in[i]) /
                     35.0;
        }
        out[N - 2] = (9.0 * in[N - 1] + 13.0 * in[N - 2] + 12.0 * in[N - 3] + 6.0 * in[N - 4] - 5.0 * in[N - 5]) / 35.0;
        out[N - 1] = (31.0 * in[N - 1] + 9.0 * in[N - 2] - 3.0 * in[N - 3] - 5.0 * in[N - 4] + 3.0 * in[N - 5]) / 35.0;
    }
}

/**
 * @brief �ߵ����ƽ���˲�
 * @param in ��������
 * @param out �������
 * @param N ���鳤��
 */
void quadraticSmooth7(double in[], double out[], int N)
{
    int i;
    if (N < 7)
    {
        for (i = 0; i <= N - 1; i++)
        {
            out[i] = in[i];
        }
    }
    else
    {
        out[0] = (32.0 * in[0] + 15.0 * in[1] + 3.0 * in[2] - 4.0 * in[3] -
                  6.0 * in[4] - 3.0 * in[5] + 5.0 * in[6]) /
                 42.0;

        out[1] = (5.0 * in[0] + 4.0 * in[1] + 3.0 * in[2] + 2.0 * in[3] +
                  in[4] - in[6]) /
                 14.0;

        out[2] = (1.0 * in[0] + 3.0 * in[1] + 4.0 * in[2] + 4.0 * in[3] +
                  3.0 * in[4] + 1.0 * in[5] - 2.0 * in[6]) /
                 14.0;
        for (i = 3; i <= N - 4; i++)
        {
            out[i] = (-2.0 * (in[i - 3] + in[i + 3]) +
                      3.0 * (in[i - 2] + in[i + 2]) +
                      6.0 * (in[i - 1] + in[i + 1]) + 7.0 * in[i]) /
                     21.0;
        }
        out[N - 3] = (1.0 * in[N - 1] + 3.0 * in[N - 2] + 4.0 * in[N - 3] +
                      4.0 * in[N - 4] + 3.0 * in[N - 5] + 1.0 * in[N - 6] - 2.0 * in[N - 7]) /
                     14.0;

        out[N - 2] = (5.0 * in[N - 1] + 4.0 * in[N - 2] + 3.0 * in[N - 3] +
                      2.0 * in[N - 4] + in[N - 5] - in[N - 7]) /
                     14.0;

        out[N - 1] = (32.0 * in[N - 1] + 15.0 * in[N - 2] + 3.0 * in[N - 3] -
                      4.0 * in[N - 4] - 6.0 * in[N - 5] - 3.0 * in[N - 6] + 5.0 * in[N - 7]) /
                     42.0;
    }
}

/**
 * @brief �������ƽ���˲�
 * @param in ��������
 * @param out �������
 * @param N ���鳤��
 */
void cubicSmooth5(double in[], double out[], int N)
{

    int i;
    if (N < 5)
    {
        for (i = 0; i <= N - 1; i++)
            out[i] = in[i];
    }

    else
    {
        out[0] = (69.0 * in[0] + 4.0 * in[1] - 6.0 * in[2] + 4.0 * in[3] - in[4]) / 70.0;
        out[1] = (2.0 * in[0] + 27.0 * in[1] + 12.0 * in[2] - 8.0 * in[3] + 2.0 * in[4]) / 35.0;
        for (i = 2; i <= N - 3; i++)
        {
            out[i] = (-3.0 * (in[i - 2] + in[i + 2]) + 12.0 * (in[i - 1] + in[i + 1]) + 17.0 * in[i]) / 35.0;
        }
        out[N - 2] = (2.0 * in[N - 5] - 8.0 * in[N - 4] + 12.0 * in[N - 3] + 27.0 * in[N - 2] + 2.0 * in[N - 1]) / 35.0;
        out[N - 1] = (-in[N - 5] + 4.0 * in[N - 4] - 6.0 * in[N - 3] + 4.0 * in[N - 2] + 69.0 * in[N - 1]) / 70.0;
    }
    return;
}

/**
 * @brief �ߵ�����ƽ���˲�
 * @param in ��������
 * @param out �������
 * @param N ���鳤��
 */
void cubicSmooth7(double in[], double out[], int N)
{
    int i;
    if (N < 7)
    {
        for (i = 0; i <= N - 1; i++)
        {
            out[i] = in[i];
        }
    }
    else
    {
        out[0] = (39.0 * in[0] + 8.0 * in[1] - 4.0 * in[2] - 4.0 * in[3] +
                  1.0 * in[4] + 4.0 * in[5] - 2.0 * in[6]) /
                 42.0;
        out[1] = (8.0 * in[0] + 19.0 * in[1] + 16.0 * in[2] + 6.0 * in[3] -
                  4.0 * in[4] - 7.0 * in[5] + 4.0 * in[6]) /
                 42.0;
        out[2] = (-4.0 * in[0] + 16.0 * in[1] + 19.0 * in[2] + 12.0 * in[3] +
                  2.0 * in[4] - 4.0 * in[5] + 1.0 * in[6]) /
                 42.0;
        for (i = 3; i <= N - 4; i++)
        {
            out[i] = (-2.0 * (in[i - 3] + in[i + 3]) +
                      3.0 * (in[i - 2] + in[i + 2]) +
                      6.0 * (in[i - 1] + in[i + 1]) + 7.0 * in[i]) /
                     21.0;
        }
        out[N - 3] = (-4.0 * in[N - 1] + 16.0 * in[N - 2] + 19.0 * in[N - 3] +
                      12.0 * in[N - 4] + 2.0 * in[N - 5] - 4.0 * in[N - 6] + 1.0 * in[N - 7]) /
                     42.0;
        out[N - 2] = (8.0 * in[N - 1] + 19.0 * in[N - 2] + 16.0 * in[N - 3] +
                      6.0 * in[N - 4] - 4.0 * in[N - 5] - 7.0 * in[N - 6] + 4.0 * in[N - 7]) /
                     42.0;
        out[N - 1] = (39.0 * in[N - 1] + 8.0 * in[N - 2] - 4.0 * in[N - 3] -
                      4.0 * in[N - 4] + 1.0 * in[N - 5] + 4.0 * in[N - 6] - 2.0 * in[N - 7]) /
                     42.0;
    }
}