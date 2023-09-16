/**
 * @file bsp_dht11.c
 * @author fire
 * @brief DHT11 Hardware Support
 * @version 0.1
 * @date 2023.08.21
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "bsp_dht11.h"

// /**
//  * @brief ����DHT11�õ���I/O��
//  */
// void DHT11_GPIO_Config(void)
// {
// 	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
// 	GPIO_InitTypeDef GPIO_InitStruct;

// 	/*����DHT11��ص�GPIO����ʱ��*/
// 	DHT11_GPIO_CLK_ENABLE();
// 	/*ѡ��Ҫ���Ƶ�GPIO����*/
// 	GPIO_InitStruct.Pin = DHT11_PIN;

// 	/*�������ŵ��������Ϊ�������*/
// 	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

// 	/*��������Ϊ����ģʽ*/
// 	GPIO_InitStruct.Pull = GPIO_NOPULL;

// 	/*������������Ϊ���� */
// 	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

// 	/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
// 	HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
// }

// /*
//  * ��������DHT11_Mode_IPU
//  * ����  ��ʹDHT11-DATA���ű�Ϊ��������ģʽ
//  */
// static void DHT11_Mode_IPU(void)
// {
// 	GPIO_InitTypeDef GPIO_InitStruct;

// 	/*ѡ��Ҫ���Ƶ�DHT11_PORT����*/
// 	GPIO_InitStruct.Pin = DHT11_PIN;

// 	/*��������ģʽΪ��������ģʽ*/
// 	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

// 	GPIO_InitStruct.Pull = GPIO_NOPULL;

// 	/*������������Ϊ���� */
// 	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

// 	/*���ÿ⺯������ʼ��DHT11_PORT*/
// 	HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
// }

// /*
//  * ��������DHT11_Mode_Out_PP
//  * ����  ��ʹDHT11-DATA���ű�Ϊ�������ģʽ
//  */
// static void DHT11_Mode_Out_PP(void)
// {

// 	GPIO_InitTypeDef GPIO_InitStruct;

// 	/*ѡ��Ҫ���Ƶ�DHT11_PORT����*/
// 	GPIO_InitStruct.Pin = DHT11_PIN;

// 	/*�������ŵ��������Ϊ�������*/
// 	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

// 	GPIO_InitStruct.Pull = GPIO_NOPULL;

// 	/*������������Ϊ���� */
// 	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

// 	/*���ÿ⺯������ʼ��DHT11_PORT*/
// 	HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
// }

// /*
//  * ��DHT11��ȡһ���ֽڣ�MSB����
//  */
// static uint8_t Read_Byte(void)
// {
// 	uint8_t i, temp = 0;

// 	for (i = 0; i < 8; i++)
// 	{
// 		/*ÿbit��50us�͵�ƽ���ÿ�ʼ����ѯֱ���ӻ����� ��50us �͵�ƽ ����*/
// 		while (DHT11_DATA_IN() == Bit_RESET);

// 		/*DHT11 ��26~28us�ĸߵ�ƽ��ʾ��0������70us�ߵ�ƽ��ʾ��1����
// 		 *ͨ����� x us��ĵ�ƽ��������������״ ��x ���������ʱ
// 		 */
// 		delay_ms(1); // ��ʱx us �����ʱ��Ҫ��������0������ʱ�伴��

// 		if (DHT11_DATA_IN() == Bit_SET) /* x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1�� */
// 		{
// 			/* �ȴ�����1�ĸߵ�ƽ���� */
// 			while (DHT11_DATA_IN() == Bit_SET);

// 			temp |= (uint8_t)(0x01 << (7 - i)); // �ѵ�7-iλ��1��MSB����
// 		}
// 		else // x us��Ϊ�͵�ƽ��ʾ���ݡ�0��
// 		{
// 			temp &= (uint8_t) ~(0x01 << (7 - i)); // �ѵ�7-iλ��0��MSB����
// 		}
// 	}
// 	return temp;
// }
// /*
//  * һ�����������ݴ���Ϊ40bit����λ�ȳ�
//  * 8bit ʪ������ + 8bit ʪ��С�� + 8bit �¶����� + 8bit �¶�С�� + 8bit У���
//  */
// uint8_t BSP_DHT11_ReadData(BSP_DHT11Data_TypeDef *DHT11_Data)
// {
// 	uint16_t count;
// 	/*���ģʽ*/
// 	DHT11_Mode_Out_PP();
// 	/*��������*/
// 	DHT11_DATA_OUT(DHT11_LOW);
// 	/*��ʱ18ms*/
// 	delay_ms(20);

// 	/*�������� ������ʱ30us*/
// 	DHT11_DATA_OUT(DHT11_HIGH);

// 	delay_ms(1); // ��ʱ30us

// 	/*������Ϊ���� �жϴӻ���Ӧ�ź�*/
// 	DHT11_Mode_IPU();

// 	/*�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������*/
// 	if (DHT11_DATA_IN() == Bit_RESET)
// 	{
// 		count = 0;
// 		/*��ѯֱ���ӻ����� ��80us �͵�ƽ ��Ӧ�źŽ���*/
// 		while (DHT11_DATA_IN() == Bit_RESET)
// 		{
// 			count++;
// 			if (count > 1000)
// 				return 0;
// 			delay_ms(1);
// 		}

// 		count = 0;
// 		/*��ѯֱ���ӻ������� 80us �ߵ�ƽ �����źŽ���*/
// 		while (DHT11_DATA_IN() == Bit_SET)
// 		{
// 			count++;
// 			if (count > 1000)
// 				return 0;
// 			delay_ms(1);
// 		}
// 		/*��ʼ��������*/
// 		DHT11_Data->humi_int = Read_Byte();

// 		DHT11_Data->humi_deci = Read_Byte();

// 		DHT11_Data->temp_int = Read_Byte();

// 		DHT11_Data->temp_deci = Read_Byte();

// 		DHT11_Data->check_sum = Read_Byte();

// 		/*��ȡ���������Ÿ�Ϊ���ģʽ*/
// 		DHT11_Mode_Out_PP();
// 		/*��������*/
// 		DHT11_DATA_OUT(DHT11_HIGH);

// 		/*����ȡ�������Ƿ���ȷ*/
// 		if (DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int + DHT11_Data->temp_deci)
// 			return 1;
// 		else
// 			return 0;
// 	}
// 	else
// 	{
// 		return 0;
// 	}
// }

// /**
//  * @brief ��ʼ��DHT11
//  */
// void BSP_DHT11_Init(void)
// {
// 	DHT11_GPIO_Config();
// }

uint16_t tem;
uint16_t him;

void DHT11_IO_IN(void) // ��ʼ���ӿڶ���������������
{
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = DHT11_Pin;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStructure);
}

void DHT11_IO_OUT(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = DHT11_Pin;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStructure);
}
// ��ʼ��DHT11��IO�� DQ ͬʱ���DHT11�Ĵ���
// ����1:������
// ����0:����
uint8_t DHT11_Init(void)
{
	DHT11_Rst();
	return DHT11_Check();
}
// ��λDHT11
void DHT11_Rst(void)
{
	DHT11_IO_OUT();	   // ����Ϊ���
	DHT11_DQ_OUT_LOW;  // ����DQ
	delay_ms(20);	   // ��������18ms
	DHT11_DQ_OUT_HIGH; // DQ=1
	delay_us(30);	   // ��������20~40us
}

// �ȴ�DHT11�Ļ�Ӧ
// ����1:δ��⵽DHT11�Ĵ���
// ����0:����
uint8_t DHT11_Check(void)
{
	uint8_t retry = 0;
	DHT11_IO_IN();					   // ����Ϊ���
	while (DHT11_DQ_IN && retry < 100) // DHT11������40~80us
	{
		retry++;
		delay_us(1);
	};
	if (retry >= 100)
		return 1;
	else
		retry = 0;
	while (!DHT11_DQ_IN && retry < 100) // DHT11���ͺ���ٴ�����40~80us
	{
		retry++;
		delay_us(1);
	};
	//	printf("0000!!!");
	if (retry >= 100)
		return 1;
	//	printf("11111!!!");
	return 0;
}

// ��DHT11��ȡһ��λ
// ����ֵ��1/0
uint8_t DHT11_Read_Bit(void)
{
	uint8_t retry = 0;
	while (DHT11_DQ_IN && retry < 100) // �ȴ���Ϊ�͵�ƽ
	{
		retry++;
		delay_us(1);
	}
	retry = 0;
	while (!DHT11_DQ_IN && retry < 100) // �ȴ���ߵ�ƽ
	{
		retry++;
		delay_us(1);
	}
	delay_us(40); // �ȴ�40us
	if (DHT11_DQ_IN)
		return 1;
	else
		return 0;
}

// ��DHT11��ȡһ���ֽ�
// ����ֵ������������
uint8_t DHT11_Read_Byte(void)
{
	uint8_t i, dat;
	dat = 0;
	for (i = 0; i < 8; i++)
	{
		dat <<= 1;
		dat |= DHT11_Read_Bit();
	}
	return dat;
}
// ��DHT11��ȡһ������
// temp:�¶�ֵ(��Χ:0~50��)
// humi:ʪ��ֵ(��Χ:20%~90%)
// ����ֵ��0,����;1,��ȡʧ��
uint8_t DHT11_Read_Data(uint16_t *temp, uint16_t *humi)
{
	uint8_t buf[5];
	uint8_t i;
	DHT11_Rst();
	if (DHT11_Check() == 0)
	{
		for (i = 0; i < 5; i++) // ��ȡ40λ����
		{
			buf[i] = DHT11_Read_Byte();
		}
		if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
		{
			*humi = (buf[0] << 8) + buf[1];
			*temp = (buf[2] << 8) + buf[3];
		}
	}
	else
		return 1;
	return 0;
}
