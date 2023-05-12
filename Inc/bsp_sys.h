/*
 ******************************************************************************
 * @file    :bsp_sys.h
 * @brief   :The board support package for sys.
 * @version :0.1.0
 * @author  :����ԭ��
 * @date    :2022.06.28
 ******************************************************************************
 * @attention
 *
 *    λ������,ʵ��51���Ƶ�GPIO���ƹ���
 *    ����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
 *
 ******************************************************************************
 */

#ifndef __BSP_SYS_H__
#define __BSP_SYS_H__

#include "bsp_config.h"

#ifdef __BSP_STM32F1_ENABLED
#include "stm32f1xx.h"
#endif
#ifdef __BSP_STM32F4_ENABLED
#include "stm32f4xx.h"
#endif
#ifdef __BSP_STM32G4_ENABLED
#include "stm32g4xx.h"
#endif
#ifdef __BSP_STM32H7_ENABLED
#include "stm32h7xx.h"
#endif

/**********************************************************
                    JTAGģʽ���ö���
**********************************************************/

#define JTAG_SWD_Enable 0x00000000  // ��λֵ
#define JNTRST_Disable 0x00000001   // JNTRST�����ͷ�
#define SWD_Enable 0x00000010       // JTAG�رգ�SWD����
#define JTAG_SWD_Disable 0x00000100 // JTAG��SWD���ر�

/**********************************************************
                    λ��������غ궨��
              �ο���CM3Ȩ��ָ�ϡ���87 ~ 92ҳ
**********************************************************/

#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0X02000000 + ((addr & 0x000FFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

/**********************************************************
                      GPIO��ַӳ��
              ����ַ���ϼĴ���ƫ�Ƶ�ַ���
**********************************************************/
#ifdef __BSP_STM32F1_ENABLED

#define GPIOA_ODR_Addr (GPIOA_BASE + 12) // 0x4001080C
#define GPIOB_ODR_Addr (GPIOB_BASE + 12) // 0x40010C0C
#define GPIOC_ODR_Addr (GPIOC_BASE + 12) // 0x4001100C
#define GPIOD_ODR_Addr (GPIOD_BASE + 12) // 0x4001140C
#define GPIOE_ODR_Addr (GPIOE_BASE + 12) // 0x4001180C
#define GPIOF_ODR_Addr (GPIOF_BASE + 12) // 0x40011A0C
#define GPIOG_ODR_Addr (GPIOG_BASE + 12) // 0x40011E0C

#define GPIOA_IDR_Addr (GPIOA_BASE + 8) // 0x40010808
#define GPIOB_IDR_Addr (GPIOB_BASE + 8) // 0x40010C08
#define GPIOC_IDR_Addr (GPIOC_BASE + 8) // 0x40011008
#define GPIOD_IDR_Addr (GPIOD_BASE + 8) // 0x40011408
#define GPIOE_IDR_Addr (GPIOE_BASE + 8) // 0x40011808
#define GPIOF_IDR_Addr (GPIOF_BASE + 8) // 0x40011A08
#define GPIOG_IDR_Addr (GPIOG_BASE + 8) // 0x40011E08

#endif

#ifdef __BSP_STM32F4_ENABLED

#define GPIOA_IDR_Addr (GPIOA_BASE + 10)
#define GPIOB_IDR_Addr (GPIOB_BASE + 10)
#define GPIOC_IDR_Addr (GPIOC_BASE + 10)
#define GPIOD_IDR_Addr (GPIOD_BASE + 10)
#define GPIOE_IDR_Addr (GPIOE_BASE + 10)
#define GPIOF_IDR_Addr (GPIOF_BASE + 10)
#define GPIOG_IDR_Addr (GPIOG_BASE + 10)

#define GPIOA_ODR_Addr (GPIOA_BASE + 14)
#define GPIOB_ODR_Addr (GPIOB_BASE + 14)
#define GPIOC_ODR_Addr (GPIOC_BASE + 14)
#define GPIOD_ODR_Addr (GPIOD_BASE + 14)
#define GPIOE_ODR_Addr (GPIOE_BASE + 14)
#define GPIOF_ODR_Addr (GPIOF_BASE + 14)
#define GPIOG_ODR_Addr (GPIOG_BASE + 14)

#endif

#ifdef __BSP_STM32H7_ENABLED

#define GPIOA_IDR_Addr (GPIOA_BASE + 10)
#define GPIOB_IDR_Addr (GPIOB_BASE + 10)
#define GPIOC_IDR_Addr (GPIOC_BASE + 10)
#define GPIOD_IDR_Addr (GPIOD_BASE + 10)
#define GPIOE_IDR_Addr (GPIOE_BASE + 10)
#define GPIOF_IDR_Addr (GPIOF_BASE + 10)
#define GPIOG_IDR_Addr (GPIOG_BASE + 10)

#define GPIOA_ODR_Addr (GPIOA_BASE + 14)
#define GPIOB_ODR_Addr (GPIOB_BASE + 14)
#define GPIOC_ODR_Addr (GPIOC_BASE + 14)
#define GPIOD_ODR_Addr (GPIOD_BASE + 14)
#define GPIOE_ODR_Addr (GPIOE_BASE + 14)
#define GPIOF_ODR_Addr (GPIOF_BASE + 14)
#define GPIOG_ODR_Addr (GPIOG_BASE + 14)

#endif

/**********************************************************
             ʵ�ֵ�һIO������������51��IO����
                   nֵҪС��IO������Ŀ
**********************************************************/

#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n) // ���
#define PAin(n) BIT_ADDR(GPIOA_IDR_Addr, n)  // ����

#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n) // ���
#define PBin(n) BIT_ADDR(GPIOB_IDR_Addr, n)  // ����

#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n) // ���
#define PCin(n) BIT_ADDR(GPIOC_IDR_Addr, n)  // ����

#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n) // ���
#define PDin(n) BIT_ADDR(GPIOD_IDR_Addr, n)  // ����

#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n) // ���
#define PEin(n) BIT_ADDR(GPIOE_IDR_Addr, n)  // ����

#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr, n) // ���
#define PFin(n) BIT_ADDR(GPIOF_IDR_Addr, n)  // ����

#define PGout(n) BIT_ADDR(GPIOG_ODR_Addr, n) // ���
#define PGin(n) BIT_ADDR(GPIOG_IDR_Addr, n)  // ����

#endif
