/**
 * @file bsp_ad9854.h
 * @author July (Email: JulyCub@163.com)
 * @brief AD9854 Driver
 * @version 0.1
 * @date 2023.07.24
 * @pinset
 *          --
 *          RESET ����PA6;
 *          UDCLK ����PA4;
 *          WR    ����PA5;
 *          RD    ����PA8;
 *          OSK   ����PA2;
 *          FDATA ����PB10;
 *          D0		����PC0;
 *          D1		����PC1;
 *          D2		����PC2;
 *          D3		����PC3;
 *          D4		����PC4;
 *          D5		����PC5;
 *          D6		����PC6;
 *          D7		����PC7;
 *          A0		����PC8;
 *          A1		����PC9;
 *          A2		����PC10;
 *          A3		����PC11;
 *          A4		����PC12;
 *          A5		����PC13;
 *          VDD--�߼���Դ(3.3V)
 *          VSS--GND(0V)
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __AD9854_H
#define __AD9854_H
#include "bsp_config.h"
#include "bsp_sys.h"

#define AD9854_RST PAout(6)            // AD9854��λ�˿�
#define AD9854_UDCLK PAout(4)          // AD9854����ʱ��
#define AD9854_WR PAout(5)             // AD9854дʹ�ܣ�����Ч
#define AD9854_RD PAout(8)             // AD9854��ʹ�ܣ�����Ч
#define AD9854_OSK PAout(2)            // AD9854 OSK���ƶ�
#define AD9854_FSK_BPSK_HOLD PBout(10) // AD9854 FSK,BPSK,HOLD���ƽţ���AD9854оƬ29��

#define AD9854_DataBus GPIOC->BSRR
#define AD9854_AdrBus GPIOC->BSRR

#define AUTO 1   // �Զ�ɨƵ
#define MANUAL 0 // FSK���ƽſ���ɨƵ

#define uint unsigned int
#define uchar unsigned char
#define ulong unsigned long

//**************************���²���Ϊ��������********************************

void AD9854_IO_Init(void);              // AD9854��Ҫ�õ���IO�ڳ�ʼ��
void AD9854_WR_Byte(u32 addr, u32 dat); // AD9854���п�д����
void Freq_convert(long Freq);           // �����ź�Ƶ������ת��
void Freq_double_convert(double Freq);  // �����ź�Ƶ������ת��,��ڲ���Ϊdouble����ʹ�źŵ�Ƶ�ʸ���ȷ

void AD9854_InitSingle(void);                        // AD9854��Ƶģʽ��ʼ��
void AD9854_SetSine(ulong Freq, uint Shape);         // AD9854���Ҳ���������,Ƶ��Ϊ����
void AD9854_SetSine_double(double Freq, uint Shape); // AD9854���Ҳ���������,Ƶ�ʿ�ΪС��

void AD9854_InitFSK(void);                    // AD9854��FSKģʽ��ʼ��
void AD9854_SetFSK(ulong Freq1, ulong Freq2); // AD9854��FSK��������

void AD9854_InitRFSK(uchar autoSweepEn);                                                 // AD9854��RFSKģʽ��ʼ�� ���Ե�Ƶģʽ��ɨƵ
void AD9854_SetRFSK(ulong Freq_Low, ulong Freq_High, ulong Freq_Up_Down, ulong FreRate); // AD9854��RFSKɨƵ��������

void AD9854_InitChirp(void);                                                     // AD9854��Chirpģʽ��ʼ��
void AD9854_SetChirp(ulong Freq, uint Shape, ulong Freq_Up_Down, ulong FreRate); // AD9854��ChirpɨƵ��������

void AD9854_InitBPSK(void);                                            // AD9854��BPSKģʽ��ʼ��
void AD9854_SetBPSK(ulong Freq, uint Shape, uint Phase1, uint Phase2); // AD9854��BPSK��������

void AD9854_InitOSK(void);                       // AD9854��OSKģʽ��ʼ��
void AD9854_SetOSK(ulong Freq, uchar RateShape); // AD9854��OSK��������

#endif
