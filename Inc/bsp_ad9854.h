/**
 * @file bsp_ad9854.h
 * @author July (Email: JulyCub@163.com)
 * @brief AD9854 Driver
 * @version 0.1
 * @date 2023.07.24
 * @pinset
 *          --
 *          MRESET      - PE6;
 *          UD/ICLK     - PC13;
 *          RSCLK/WR    - PE3;
 *          RD          - PE2;
 *          OSK         - PE4;
 *          FSK	  	    - PE5;
 *          D0		    - PF0;
 *          D1		    - PF1;
 *          D2		    - PF2;
 *          D3		    - PF3;
 *          D4		    - PF4;
 *          D5		    - PF5;
 *          D6		    - PF6;
 *          D7		    - PF7;
 *          A0		    - PF8;
 *          A1		    - PF9;
 *          A2		    - PF10;
 *          A3		    - PF11;
 *          A4		    - PF12;
 *          A5		    - PF13;
 *          VDD         �߼���Դ(3.3V)
 *          VSS         GND(0V)
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __AD9854_H
#define __AD9854_H
#include "bsp_config.h"
#include "bsp_sys.h"

#define AD9854_RST PEout(6)           // AD9854��λ�˿�
#define AD9854_UDCLK PCout(13)        // AD9854����ʱ��
#define AD9854_WR PEout(3)            // AD9854дʹ�ܣ�����Ч
#define AD9854_RD PEout(2)            // AD9854��ʹ�ܣ�����Ч
#define AD9854_OSK PEout(4)           // AD9854 OSK���ƶ�
#define AD9854_FSK_BPSK_HOLD PEout(5) // AD9854 FSK,BPSK,HOLD���ƽţ���AD9854оƬ29��

#define AD9854_DataBus GPIOF->BSRR
#define AD9854_AdrBus GPIOF->BSRR

#define AUTO 1   // �Զ�ɨƵ
#define MANUAL 0 // FSK���ƽſ���ɨƵ

#ifndef uint
#define uint unsigned int
#endif
#ifndef uchar
#define uchar unsigned char
#endif
#ifndef ulong
#define ulong unsigned long
#endif

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
