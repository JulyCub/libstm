/**
 * @file bsp_decode.c
 * @author July (Email: JulyCub@163.com)
 * @brief The board support package for decode.
 * @version 0.1
 * @date 2022.06.28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "stdio.h"
#include "bsp_decode.h"

/**
 * @brief 16λ����ת�޷��Ÿ�����
 * @param arry Դ���ݣ�����Ϊ16
 * @attention ��8λ����,��8λС��
 * @return float
 */
float BSP_Arry16ToUFloat(uint16_t arry[])
{
    uint16_t temp = 0;
    float result = 0;

    for (uint8_t i = 0; i < 16; i++)
    {
        temp <<= 1;
        temp += arry[i];
        switch (i)
        {
        case 7:
            result += temp;
            temp = 0;
            break;
        case 15:
            result += temp % 100 / 100.0;
            temp = 0;
            break;
        default:
            break;
        }
    }
    return result;
}

/**
 * @brief 24λ����ת�޷��Ÿ�����
 * @param arry Դ���ݣ�����Ϊ24
 * @attention ��8λΪ2λ����,��16λλ4λС��
 * @return ת�����
 */
float BSP_Arry24ToUFloat(uint8_t arry[])
{
    uint16_t temp = 0;
    float result = 0;

    for (uint8_t i = 0; i < 24; i++)
    {
        temp <<= 1;
        temp += arry[i];
        switch (i)
        {
        case 7:
            result += temp;
            temp = 0;
            break;
        case 15:
            result += temp % 100 / 100.0;
            temp = 0;
            break;
        case 23:
            result += temp % 100 / 10000.0;
            break;
        default:
            break;
        }
    }
    return result;
}

/**
 * @brief ��һ��
 *
 * @param data ��������
 * @param result ��һ�����
 * @param len �������ݳ���
 * @param threshold ��ֵ
 */
void BSP_DataNormalization(uint16_t data[], uint16_t result[], uint16_t len, uint16_t threshold)
{
    for (uint16_t i = 0; i < len; i++)
    {
        if (data[i] <= threshold)
            result[i] = 0;
        else
            result[i] = 1;
    }
}

/**
 * @brief �첨�㷨
 *
 * @param data Դ����
 * @param result ������
 * @param data_len ���ݳ���
 * @param bitlen λ��
 */
void BSP_DigitalDetector(uint16_t *data, uint16_t *result, uint16_t data_len, uint16_t bitlen)
{
    uint8_t lock = 0;
    uint16_t Zeros = 0;

    for (uint16_t i = 0; i < data_len - 1; i++)
    {
        if (*(data + i + 1) - *(data + i) == -1) /* �½��ؼ�� */
        {
            *(result + i) = 1;
            *(result + i + 1) = 1;

            /* Zeros���� */
            lock = 1;
            while (lock)
            {
                i++; /* ���ݵ�ַ���� */
                /* �����ؼ�� */
                if (*(data + i))
                    lock = 0; /* Zeros���� */
                Zeros++;      /* 0ֵ�ۼ� */
            }
            /* λ0 */
            if (Zeros >= bitlen)
            {
                for (uint16_t j = 1; j < Zeros; j++)
                {
                    *(result + (i - j)) = 0;
                }
            }
            /* λ1 */
            else
            {
                for (uint16_t j = 0; j < Zeros; j++)
                {
                    *(result + (i - j)) = 1;
                }
            }
            Zeros = 0;
        }
        else
        {
            *(result + i) = 1;
        }
    }
}

/**
 * @brief ֡������ȡ
 *
 * @version 1.0
 * @param data Դ����
 * @param result ��ȡ���
 * @param data_len Դ���ݳ���
 * @param bitlen λ��
 * @param headtaillen ֡ͷ/β���
 *
 * @return 1-��ȡ������ 0-δ��ȡ������
 */
uint8_t BSP_ExtractEffectiveData(uint16_t *data,
                                 uint16_t *result,
                                 uint16_t data_len,
                                 uint16_t bitlen,
                                 uint16_t headtaillen)
{
    uint8_t lock = 0;
    uint16_t Ones = 0;
    uint16_t cache = 0;

    for (uint16_t i = 0; i < data_len; i++)
    {
        /* ֡ͷ��� */
        if ((*(data + i + 1) - *(data + i)) == 1) /* �����ؼ�� */
        {
            /* Ones���� */
            lock = 1;
            while (lock)
            {
                Ones++;
                i++;
                if (!*(data + i)) /* �½��ؼ�� */
                    lock = 0;
            }
            /* ֡ͷ�ж� */
            if (Ones > headtaillen)
            {
                /* 1ֵ�ۼ������� */
                Ones = 0;
                for (; i < data_len;)
                {
                    /* ֡β��� */
                    if (*(data + 1)) /* �����ؼ�� */
                    {
                        /* Ones���� */
                        lock = 1;
                        while (lock)
                        {
                            Ones++;
                            i++;
                            if (!*(data + i)) /* �½��ؼ�� */
                                lock = 0;
                        }
                        /* ֡β�ж� */
                        if (Ones > headtaillen + bitlen)
                        {
                            /* ����ƫ�� */
                            uint16_t excursion = Ones + cache - bitlen;

                            cache += bitlen;
                            /* ������ȡ */
                            *(result + 0) = 2;
                            for (uint16_t j = 1; j < cache; j++)
                            {
                                /* ������ȡ */
                                *(result + j) = *(data + i - excursion + j);
                            }
                            *(result + cache) = 2;
                            /* Success */
                            return 1;
                        }
                        else if (Ones > headtaillen)
                        {
                            /* ����ƫ�� */
                            uint16_t excursion = Ones + cache;

                            /* ������ȡ */
                            *(result + 0) = 1;
                            for (uint16_t j = 1; j < cache; j++)
                            {
                                /* ������ȡ */
                                *(result + j) = *(data + i - excursion + j);
                            }
                            *(result + cache) = 1;
                            /* Success */
                            return 1;
                        }
                        else
                        {
                            /* ���ݻ��� */
                            cache += Ones;
                            /* 1�ۼ������� */
                            Ones = 0;
                        }
                    }
                    else
                    {
                        /* ���ݻ��� */
                        cache++;
                        i++;
                    }
                }
            }
            else
            {
                /* 1�ۼ������� */
                Ones = 0;
            }
        }
    }
    /* Faile */
    return 0;
}

/**
 * @brief ֡������ȡ
 *
 * @version 1.1
 * @param data Դ����
 * @param result ���
 * @param data_len ���ݳ���
 * @param bitlen λ��
 * @param headtaillen ֡ͷ/β���
 * @return uint8_t 1-��ȡ������ 0-δ��ȡ������
 */
uint8_t BSP_ExtractEffectiveData_v2(uint16_t *data,
                                    uint16_t *result,
                                    uint16_t data_len,
                                    uint16_t bitlen,
                                    uint16_t headtaillen)
{
    uint16_t place = 0;
    uint16_t Ones = 0;
    uint16_t cache = 0;

    while (place < data_len)
    {
        do
        {
            Ones++;
            place++;
            /* �½����ж� */
        } while (__BSP_FALEDGE_DET(*(data + place), *(data + place + 1)));
        if (Ones >= headtaillen)
        {
            /* ��¼����ê�� */
            cache = place;
            /* 1�ۼ������� */
            Ones = 0;
            /* �������ж� */
            if (__BSP_TOPEDGE_DET(*(data + place), (*(data + place + 1))))
            {
                do
                {
                    if (*(data + place))
                        Ones++;
                    if (Ones >= headtaillen)
                    {
                        // TODO:����δд�� -> 1.֡β�ж� 2.֡��ȡ
                        break;
                    }
                    /* �½����ж� */
                } while (__BSP_FALEDGE_DET(*(data + place), *(data + place + 1)));
            }
            else
            {
                /* ���һ��� */
                place++;
            }
        }
    }
    return 0;
}

/**
 * @brief ֡������ȡ
 *
 * @version 1.2
 * @param data Դ����
 * @param result ���
 * @param data_len Դ���ݳ���
 * @param frame_len ֡����
 * @return uint8_t 1-��ȡ������ 0-δ��ȡ������
 */
uint8_t BSP_ExtractEffectiveData_v3(uint16_t *data,
                                        uint16_t *result,
                                        uint16_t data_len,
                                        uint16_t frame_len)
{
    uint16_t place = 0;
    
    /* ���һ����ҵ�֡ͷ */
    do
    {
        place++;
    } while (!__BSP_FALEDGE_DET(*(data + place), *(data + place + 1)));
    /* ֡������ȡ */
    for (uint16_t i = 0; i < frame_len; i++)
    {
        result[i] = data[place + i];
        if (place + i >= data_len){
            /* Faile */
            return 0;
        }
    }
    /* success */
    return 1;
}

/**
 * @brief ���ݹ淶��
 *
 * @param data Դ����
 * @param datalen ���ݳ���
 * @param bitlen λ��
 * @param framenum ֡����
 */
void BSP_DataStandardization(uint16_t *data, uint16_t datalen, uint16_t bitlen, uint8_t framenum)
{
    uint16_t Adder = 0;

    for (uint16_t i = 0; i < bitlen * framenum; i += bitlen)
    {
        for (uint16_t j = 1; j < bitlen; j++)
        {
            if (*(data + j + i))
                Adder++;
        }
        if (Adder >= bitlen / 2)
        {
            for (uint16_t j = 0; j < bitlen; j++)
                *(data + j + i) = 1;
        }
        else
        {
            for (uint16_t j = 0; j < bitlen; j++)
                *(data + j + i) = 0;
        }
        Adder = 0;
    }
}

/**
 * @brief ���鶨������
 *
 * @param data Դ����
 * @param result ���
 * @param datalen ���ݳ���
 * @param bitlen λ��
 * @param framenum ֡����
 */
void BSP_ReadcodeFromArry(uint16_t *data, uint8_t *result, uint16_t datalen, uint16_t bitlen, uint8_t framenum)
{
    uint16_t i = 0, j = 0;

    for (i = bitlen / 2; i < datalen; i += bitlen)
    {
        if (*(data + i))
            *(result + j) = 1;
        else
            *(result + j) = 0;
        j++;
    }
}

/**
 * @brief ���ض��ۻ������㷨
 * @param data Դ����
 * @param result ���
 * @param datalen ���ݳ���
 * @param threshold ��ֵ
 */
void BSP_CumMonitorEdgePairs(uint16_t *data, uint16_t *result, uint16_t datalen, uint16_t threshold)
{
    uint16_t place = 0;
    uint16_t cache = 0;
    uint16_t sum = 0;

    while (place < datalen)
    {
        /* �����ؼ�� */
        if (__BSP_TOPEDGE_DET(*(data + place), *(data + place + 1)))
        {
            /* ����ê���¼ */
            cache = place;
            do
            {
                place++;                                                        /* ���һ��� */
                sum++;                                                          /* ���ضԵ����ۼ� */
            } while (!__BSP_TOPEDGE_DET(*(data + place), *(data + place + 1))); /* ��һ�������ؽ��� */
            /* ���ݳ߶ȼ��� */
            cache = place - cache;
            /* ��ֵ�Ƚ� */
            if (sum >= threshold)
            {
                /* �������� */
                for (uint16_t i = 0; i <= cache; i++)
                {
                    result[place - cache + i] = 0;
                }
                /* �ۼ������� */
                sum = 0;
            }
            else
            {
                /* ������һ */
                for (uint16_t i = 0; i <= cache; i++)
                {
                    result[place - cache + i] = 1;
                }
                /* �ۼ������� */
                sum = 0;
            }
        }
        else
        {
            /* ��ֵ�洢 */
            result[place] = data[place];
            /* ���һ��� */
            place++;
        }
    }
}

/**
 * @brief ��ֵ�淶�㷨
 * 
 * @param data Դ����
 * @param result ���
 * @param data_len Դ���ݳ���
 * @param bits λ��
 * @param threshold ��ֵ
 */
void BSP_SumValueStandard(uint16_t *data, uint16_t *result, uint16_t data_len, uint8_t bits, uint16_t threshold)
{
    /* λ�� */
    const uint16_t bit_len = data_len / bits;
    /* ��ֵ */
    uint16_t sum = 0;
    /* ������ */
    uint16_t counter = 0;
    /* ��ǰλ�� */
    uint16_t nowbit = 0;

    for (uint16_t i = 0; i < data_len; i++)
    {
        /* ��λ����� */
        if (counter++ < bit_len)
        {
            sum += data[i];
        }
        else
        {
            /* ���������� */
            counter = 0;
            /* ��ֵ�ж� */
            if (sum < threshold)
            {
                /* ������ 0 */
                for (uint16_t j = nowbit * bit_len; j < (nowbit + 1) * bit_len; j++)
                {
                    result[j] = 0;
                }
            }
            else
            {
                /* ������ 1 */
                for (uint16_t j = nowbit * bit_len; j < (nowbit + 1) * bit_len; j++)
                {
                    result[j] = 1;
                }
            }
            /* ��ֵ���� */
            sum = 0;
            /* ��ǰλ����һ */
            nowbit++;
        }
    }
}

/**
 * @brief ��У�����
 * @param ori_data Դ����(��У�����)
 * @param out_date ���������
 * @param len Դ���ݳ���
 * @return uint8_t 1-����ɹ� 0-����ʧ��
 */
uint8_t BSP_ParityCheck_OddDecode(uint8_t *ori_data, uint8_t *out_date, uint8_t len)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < len - 1; i++)
    {
        if (*(ori_data + i) == 1)
        {
            count++;
        }
    }
    return (count % 2) != *(ori_data + len - 1);
}

/**
 * @brief żУ�����
 * @param ori_data Դ����(��У�����)
 * @param out_date ���������
 * @param len Դ���ݳ���
 * @return uint8_t 1-����ɹ� 0-����ʧ��
 */
uint8_t BSP_ParityCheck_EvenDecode(uint8_t *ori_data, uint8_t *out_date, uint8_t len)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < len - 1; i++)
    {
        if (*(ori_data + i) == 1)
        {
            count++;
        }
    }
    return (count % 2) == *(ori_data + len - 1);
}

/**
 * @brief ��������루����1λ�����ܣ�
 * @param received_data ���յ��ı������ݣ������룩
 * @param decoded_data ������ԭʼ����
 * @param data_len ���ݳ���
 * @param type ��żУ������ 0-żУ�� 1-��У��
 * @return true ����ɹ���false ����ʧ�ܣ��޷���������
 */
uint8_t BSP_HammingDecode(uint8_t *received_data, uint8_t *decoded_data, const uint8_t data_len, uint8_t type)
{
    uint8_t parityLen = 0;
    while (data_len + parityLen + 1 > pow(2, parityLen))
    {
        parityLen++;
    }

    uint8_t temp_data[data_len + parityLen];
    for (uint8_t i = 0; i < data_len + parityLen; i++)
    {
        temp_data[i] = received_data[i];
    }

    uint8_t errorPos = 0;
    for (uint8_t i = 0; i < parityLen; i++)
    {
        uint8_t pos = 1 << i;
        uint8_t count = 0;

        for (uint8_t j = 0; j < data_len + parityLen; j++)
        {
            if ((((j + 1) & pos) == pos) && ((j + 1) != pos))
            {
                if (temp_data[j] == 1)
                {
                    count++;
                }
            }
        }

        if ((type && ((count + 1) % 2 != temp_data[pos - 1])) || (!type && (count % 2 != temp_data[pos - 1])))
        {
            errorPos += pos;
        }
    }

    if (errorPos > 0)
    {
        temp_data[errorPos - 1] = (temp_data[errorPos - 1] + 1) % 2;
    }
    uint8_t j = 0, y = 0;
    for (uint8_t i = 0; i < data_len + parityLen; i++)
    {
        if (i == (1 << y) - 1)
        {
            y++;
        }
        else
        {
            decoded_data[j] = temp_data[i];
            j++;
        }
    }
    return errorPos == 0;
}
