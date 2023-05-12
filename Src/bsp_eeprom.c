/**
 * @file bsp_eeprom.c
 * @author July (Email: JulyCub@163.com)
 * @brief EEPROM implementation
 * @version 0.1
 * @date 2023.03.21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "bsp_eeprom.h"
#include "bsp_i2c.h"

/**
 * @brief eeprom ��ʼ��
 */
void bsp_eeprom_Init(void)
{
	I2CInit();
}

/**
 * @brief eeprom ���ֽ�д��
 * 
 * @param data д������
 * @param data_addr д���ַ
 * @return uint8_t 
 */
uint8_t bsp_eeprom_writeByte(uint8_t data, uint8_t data_addr)
{
	I2CStart();
	/* �豸��ַ������λ */
	I2CSendByte(EEPROM_ADDR | 0x00);
	I2CWaitAck();
	/* �����ֵ�ַ */
	I2CSendByte(data_addr);
	I2CWaitAck();
	/* ���� */
	I2CSendByte(data);
	I2CWaitAck();
	I2CStop();
	return SUCCESS;
}

/**
 * @brief eeprom ��ҳд��
 * 
 * @param data д������
 * @param data_addr д���ַ
 * @param num д���ֽ���
 * @return uint8_t
 */
uint8_t bsp_eeprom_writePage(uint8_t * data, uint8_t data_addr, uint8_t num)
{
	I2CStart();
	/* �豸��ַ������λ */
	I2CSendByte(EEPROM_ADDR | 0x00);
	I2CWaitAck();
	/* ���ݶε�ַ */
	I2CSendByte(data_addr);
	I2CWaitAck();
	/* ���� */
	for (uint8_t i = 0; i < num; i++)
	{
		I2CSendByte(*(data + i));
		I2CWaitAck();
	}
	I2CStop();
	return SUCCESS;
}

/**
 * @brief eeprom ���ֽڶ�ȡ
 * 
 * @param data ��ȡ���ݴ�ŵ�ַ
 * @param data_addr ��ȡ��ַ
 * @return uint8_t 
 */
uint8_t bsp_eeprom_readByte(uint8_t * data, uint8_t data_addr)
{
	I2CStart();
	I2CSendByte(EEPROM_ADDR | 0x00);
	I2CWaitAck();
	I2CSendByte(data_addr);
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(EEPROM_ADDR | 0x01);
	I2CWaitAck();
	*data = I2CReceiveByte();
	I2CWaitNoAck();
	I2CStop();
	return SUCCESS;
}

/**
 * @brief eeprom ���ֽ�������ȡ
 * 
 * @param data ��ȡ���ݴ�ŵ�ַ
 * @param data_addr ��ȡ��ַ
 * @param num ��ȡ�ֽ���
 * @return uint8_t 
 */
uint8_t bsp_eeprom_readSeqByte(uint8_t * data, uint8_t data_addr, uint8_t num)
{
	I2CStart();
	I2CSendByte(EEPROM_ADDR | 0x00);
	I2CWaitAck();
	I2CSendByte(data_addr);
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(EEPROM_ADDR | 0x01);
	I2CWaitAck();
	for (uint8_t i = 0; i < num; i++)
	{
		*(data + i) = I2CReceiveByte();
		if (i == num - 1)
			I2CWaitNoAck();
		else
			I2CSendAck();
	}
	I2CStop();
	return SUCCESS;
}


