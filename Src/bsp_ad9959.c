/**
 ******************************************************************************
 * @file			:bsp_ad9959.c
 * @brief			:The board support package for AD9959.
 * @version			:0.1.1
 * @author			:July
 * @date			:2022.07.01
 * @updata			:2022.07.14
 ******************************************************************************
 * @pinset		:
 *
 *				F1RC:
 *							GPIOB_PIN_3 	---> P0
 *							GPIOB_PIN_4 	---> P1
 *							GPIOB_PIN_5		---> P2
 *							GPIOB_PIN_6		---> P3
 *							GPIOB_PIN_7		---> SD0
 *							GPIOC_PIN_10	---> SD1
 *							GPIOC_PIN_11	---> SD2
 *							GPIOC_PIN_9		---> SD3
 *							GPIOC_PIN_12	---> PDC/PWR
 *							GPIOA_PIN_15	---> UPDATA
 *							GPIOC_PIN_5		---> RST
 *							GPIOC_PIN_7		---> CS
 *							GPIOC_PIN_8		---> SCK
 *
 *				F4ZG:
 *							GPIOD_PIN_9 	---> P0
 *							GPIOE_PIN_15 	---> P1
 *							GPIOE_PIN_13	---> P2
 *							GPIOE_PIN_11	---> P3
 *							GPIOE_PIN_9		---> SD0
 *							GPIOE_PIN_7		---> SD1
 *							GPIOD_PIN_0		---> SD2
 *							GPIOD_PIN_1		---> SD3
 *							GPIOD_PIN_8 	---> PDC/PWR
 *							GPIOE_PIN_12	---> UPDATA
 *							GPIOE_PIN_14	---> RST
 *							GPIOE_PIN_10	---> CS
 *							GPIOE_PIN_8 	---> SCK
 *
 *******************************************************************************
 */

#include "bsp_ad9959.h"
#include "gpio.h"

#if AD9959_USART_DRIVE
uint8_t ad9959_mode[][10] = {"POINT", "SWEEP", "FSK2", "FSK4", "AM"};
uint32_t ad9959_param = 0;
#else

uint32_t FRE_Send = 0;

uint8_t AD9959_Mode = 0;		   // ɨƵ���ߵ�Ƶ
uint8_t AD9959_Wave_Show_Mode = 1; // ����ɨƵ--1  ������ɨƵ--0

/********************************ɨƵ/ɨ��*********************************/
/*
 * ͨ��
 * */
uint8_t AD9959_SweepWaveFlag = 0;	 // �Ƿ���ʾ����
uint32_t AD9959_SweepCount = 0;		 // extern����ʱ����--ms
uint32_t AD9959_SweepCountTimes = 0; // extern����ʱ����
uint32_t AD9959_SweepTime = 2;		 // ɨƵ���ʱ��--ms

/*
 * ɨƵ
 * */
uint32_t AD9959_SweepMaxFre = 1000000; // ���ɨƵƵ��--Hz
uint32_t AD9959_SweepMinFre = 1000;	   // ��СɨƵƵ��--Hz
uint32_t AD9959_SweepStepFre = 1000;   // ɨƵ����Ƶ��--Hz

/*
 * ɨ��  0~16383(��Ӧ�Ƕȣ�0��~360��)
 * */
uint32_t AD9959_SweepMaxPha = 16383; // ���ɨ����λ
uint32_t AD9959_SweepMinPha = 0;	 // ��Сɨ����λ
uint32_t AD9959_SweepStepPha = 450;	 // ɨ�ಽ����λ

/********************************��Ƶ*********************************/
uint32_t AD9959_FixedMaxFre = 300000; // ���̶����Ƶ��--Hz
uint32_t AD9959_FixedMinFre = 10000;  // ��С�̶����Ƶ��--Hz
uint32_t AD9959_FixedStepFre = 10000; // �����̶����Ƶ��--Hz
uint32_t AD9959_FixedAmpli = 1023;	  // ������Ƶ�ʷ���--Hz
uint32_t AD9959_FixedNowFre = 15000;  // ��ʱ���Ƶ��--Hz

uint32_t AD9959_NowSinFre[5] = {0};
uint32_t AD9959_NowSinAmp[5] = {0};
uint32_t AD9959_NowSinPhr[5] = {0};

int AD9959_FixedPhase_Differ = 0;	   // ��λ
int AD9959_FixedPhase_Differ_Last = 0; // ��λ
uint8_t AD9959_FixedAmpli_Differ = 0;  // ����
int AD9959_FixedAmpli_Eror = -40;	   // ��λ����У׼

uint8_t CSR_DATA0[1] = {0x10}; // �� CH0
uint8_t CSR_DATA1[1] = {0x20}; // �� CH1
uint8_t CSR_DATA2[1] = {0x40}; // �� CH2
uint8_t CSR_DATA3[1] = {0x80}; // �� CH3

double ACC_FRE_FACTOR = 8.589934592; // Ƶ������8.589934592=(2^32)/500000000 ����500M=25M*20(��Ƶ���ɱ��)

uint8_t FR1_DATA[3] = {0xD0, 0x00, 0x00}; // VCO gain control[23]=1ϵͳʱ�Ӹ���255Mhz; PLL[22:18]=10100,20��Ƶ,20*25M=500MHZ; Charge pump control = 75uA
uint8_t FR2_DATA[2] = {0x00, 0x00};		  // ˫����ɨ�裬������ʼֵɨ������ֵ���ִӽ���ֵɨ����ʼֵ
// uint8_t FR2_DATA[2] = {0x80,0x00};// ������ɨ�裬������ʼֵɨ������ֵ���ִ���ʼֵɨ������ֵ���Դ�����
uint8_t CFR_DATA[3] = {0x00, 0x03, 0x02}; // default Value = 0x000302 ͨ�����ܼĴ���

uint8_t CPOW0_DATA[2] = {0x00, 0x00}; // default Value = 0x0000   @ = POW/2^14*360

uint8_t LSRR_DATA[2] = {0x00, 0x00}; // default Value = 0x----

uint8_t RDW_DATA[4] = {0x00, 0x00, 0x00, 0x00}; // default Value = 0x--------

uint8_t FDW_DATA[4] = {0x00, 0x00, 0x00, 0x00}; // default Value = 0x--------

uint32_t SinFre[5] = {40000, 40000, 40000, 40000};
uint32_t SinAmp[5] = {1023, 1023, 1023, 1023};
uint32_t SinPhr[5] = {0, 455, 4096 * 2 - 1, 4096 * 3 - 1};

#ifdef __BSP_STM32F1
void AD9959_Drv_Init(void)
{
	/* GPIO Ports Clock Enable */
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/*Configure GPIOA pin Output Level */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);

	/*Configure GPIOB pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_RESET);

	/*Configure GPIOC pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 |
						  GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
#else
void AD9959_Drv_Init(void)
{
	/* GPIO Ports Clock Enable */
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	GPIO_InitStruct.Pin = AD9959_PS1_Pin | AD9959_PS2_Pin | AD9959_PS3_Pin |
						  AD9959_SDIO0_Pin | AD9959_SDIO1_Pin | AD9959_UPDATE_Pin |
						  AD9959_CS_Pin | AD9959_SCLK_Pin | AD9959_Reset_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = AD9959_PS0_Pin | AD9959_SDIO2_Pin | AD9959_PWR_Pin | AD9959_SDIO3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}
#endif

// ��ʱ
void delay1(uint32_t length)
{
	length = length * 12;
	while (length--)
		;
}
// IO�ڳ�ʼ��
void Intserve(void)
{
	AD9959_PWR_RESET;
	AD9959_CS_SET;
	AD9959_SCLK_RESET;
	AD9959_UPDATE_RESET;
	AD9959_PS0_RESET;
	AD9959_PS1_RESET;
	AD9959_PS2_RESET;
	AD9959_PS3_RESET;
	AD9959_SDIO0_RESET;
	AD9959_SDIO1_RESET;
	AD9959_SDIO2_RESET;
	AD9959_SDIO3_RESET;
}
// AD9959��λ
void IntReset(void)
{
	AD9959_Reset_RESET;
	delay1(1); // 1
	AD9959_Reset_SET;
	delay1(30); // 30
	AD9959_Reset_RESET;
}
// AD9959��������
void IO_Update(void)
{
	AD9959_UPDATE_RESET;
	delay1(2); // 2
	AD9959_UPDATE_SET;
	delay1(4); // 4
	AD9959_UPDATE_RESET;
}
/*--------------------------------------------
�������ܣ�������ͨ��SPI��AD9959д����
RegisterAddress: �Ĵ�����ַ
NumberofRegisters: �����ֽ���
*RegisterData: ������ʼ��ַ
temp: �Ƿ����IO�Ĵ���
----------------------------------------------*/
void WriteData_AD9959(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData, uint8_t temp)
{
	uint8_t ControlValue = 0;
	uint8_t ValueToWrite = 0;
	uint8_t RegisterIndex = 0;
	uint8_t i = 0;

	ControlValue = RegisterAddress;
	// д���ַ
	AD9959_SCLK_RESET;
	AD9959_CS_RESET;
	for (i = 0; i < 8; i++)
	{
		AD9959_SCLK_RESET;
		if (0x80 == (ControlValue & 0x80))
			AD9959_SDIO0_SET;
		else
			AD9959_SDIO0_RESET;
		AD9959_SCLK_SET;
		ControlValue <<= 1;
	}
	AD9959_SCLK_RESET;
	// д������
	for (RegisterIndex = 0; RegisterIndex < NumberofRegisters; RegisterIndex++)
	{
		ValueToWrite = RegisterData[RegisterIndex];
		for (i = 0; i < 8; i++)
		{
			AD9959_SCLK_RESET;
			if (0x80 == (ValueToWrite & 0x80))
				AD9959_SDIO0_SET;
			else
				AD9959_SDIO0_RESET;
			AD9959_SCLK_SET;
			ValueToWrite <<= 1;
		}
		AD9959_SCLK_RESET;
	}
	if (temp != 0)
		IO_Update();
	AD9959_CS_SET;
}

/************************************************************
** �������� ��void AD9959_WriteData(u8 RegisterAddress, u8 NumberofRegisters, u8 *RegisterData)
** �������� ��ʹ��ģ��SPI��AD9959д����
** ��ڲ��� ��RegisterAddress: �Ĵ�����ַ
						NumberofRegisters: Ҫд����ֽ���
						*RegisterData: ������ʼ��ַ
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void AD9959_WriteData(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData)
{
	uint8_t ControlValue = 0;
	uint8_t ValueToWrite = 0;
	uint8_t RegisterIndex = 0;
	uint8_t i = 0;

	ControlValue = RegisterAddress;
	// д���ַ
	SCLK = 0;
	CS = 0;
	for (i = 0; i < 8; i++)
	{
		SCLK = 0;
		if (0x80 == (ControlValue & 0x80))
			SDIO0 = 1;
		else
			SDIO0 = 0;
		SCLK = 1;
		ControlValue <<= 1;
	}
	SCLK = 0;
	// д������
	for (RegisterIndex = 0; RegisterIndex < NumberofRegisters; RegisterIndex++)
	{
		ValueToWrite = RegisterData[RegisterIndex];
		for (i = 0; i < 8; i++)
		{
			SCLK = 0;
			if (0x80 == (ValueToWrite & 0x80))
				SDIO0 = 1;
			else
				SDIO0 = 0;
			SCLK = 1;
			ValueToWrite <<= 1;
		}
		SCLK = 0;
	}
	CS = 1;
}

/************************************************************
** �������� ��void Write_CFTW0(uint32_t fre)
** �������� ��дCFTW0ͨ��Ƶ��ת���ּĴ���
** ��ڲ��� �� Freq:	д��Ƶ�ʣ���Χ0~200 000 000 Hz
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_CFTW0(uint32_t fre)
{
	uint8_t CFTW0_DATA[4] = {0x00, 0x00, 0x00, 0x00}; // �м����
	uint32_t Temp;
	Temp = (uint32_t)fre * ACC_FRE_FACTOR;
	CFTW0_DATA[3] = (uint8_t)Temp;
	CFTW0_DATA[2] = (uint8_t)(Temp >> 8);
	CFTW0_DATA[1] = (uint8_t)(Temp >> 16);
	CFTW0_DATA[0] = (uint8_t)(Temp >> 24);
	AD9959_WriteData(CFTW0_ADD, 4, CFTW0_DATA); // CTW0 address 0x04
}

/************************************************************
** �������� ��void Write_ACR(uint16_t Ampli)
** �������� ��дACRͨ������ת���ּĴ���
** ��ڲ��� ��Ampli:    �������,��Χ0~1023������ֵ0~1023��Ӧ�������0~500mVpp����
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_ACR(uint16_t Ampli)
{
	uint32_t A_temp = 0;
	uint8_t ACR_DATA[3] = {0x00, 0x00, 0x00}; // default Value = 0x--0000 Rest = 18.91/Iout
	A_temp = Ampli | 0x1000;

	ACR_DATA[1] = (uint8_t)(A_temp >> 8);	// ��λ����
	ACR_DATA[2] = (uint8_t)A_temp;			// ��λ����
	AD9959_WriteData(ACR_ADD, 3, ACR_DATA); // ACR address 0x06.CHn�趨����
}

/************************************************************
** �������� ��void Write_CPOW0(uint16_t Phase)
** �������� ��дCPOW0ͨ����λת���ּĴ���
** ��ڲ��� ��Phase:		�����λ,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_CPOW0(uint16_t Phase)
{
	uint8_t CPOW0_data[2] = {0x00, 0x00};
	CPOW0_data[1] = (uint8_t)Phase;
	CPOW0_data[0] = (uint8_t)(Phase >> 8);

	AD9959_WriteData(CPOW0_ADD, 2, CPOW0_data); // CPOW0 address 0x05.CHn�趨��λ
}

/************************************************************
** �������� ��void Write_LSRR(uint8_t rsrr,uint8_t fsrr)
** �������� ��дLSRR����ɨ��б�ʼĴ���
** ��ڲ��� ��	rsrr:	����б��,��Χ��0~255
							fsrr:	�½�б��,��Χ��0~255
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_LSRR(uint8_t rsrr, uint8_t fsrr)
{
	uint8_t LSRR_data[2] = {0x00, 0x00};

	LSRR_data[1] = rsrr;
	LSRR_data[0] = fsrr; // ��8λ�½�б��

	AD9959_WriteData(LSRR_ADD, 2, LSRR_data);
}

/************************************************************
** �������� ��void Write_RDW(uint32_t r_delta)
** �������� ��дRDW���������Ĵ���
** ��ڲ��� ��r_delta:��������,0-4294967295
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_RDW(uint32_t r_delta)
{
	uint8_t RDW_data[4] = {0x00, 0x00, 0x00, 0x00}; // �м����

	RDW_data[3] = (uint8_t)r_delta;
	RDW_data[2] = (uint8_t)(r_delta >> 8);
	RDW_data[1] = (uint8_t)(r_delta >> 16);
	RDW_data[0] = (uint8_t)(r_delta >> 24);
	AD9959_WriteData(RDW_ADD, 4, RDW_data);
}

/************************************************************
** �������� ��void Write_FDW(uint32_t f_delta)
** �������� ��дFDW�½������Ĵ���
** ��ڲ��� ��f_delta:�½�����,0-4294967295
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_FDW(uint32_t f_delta)
{
	uint8_t FDW_data[4] = {0x00, 0x00, 0x00, 0x00}; // �м����

	FDW_data[3] = (uint8_t)f_delta;
	FDW_data[2] = (uint8_t)(f_delta >> 8);
	FDW_data[1] = (uint8_t)(f_delta >> 16);
	FDW_data[0] = (uint8_t)(f_delta >> 24);
	AD9959_WriteData(FDW_ADD, 4, FDW_data);
}

/************************************************************
** �������� ��void Write_Profile_Fre(uint8_t profile,uint32_t data)
** �������� ��дProfile�Ĵ���
** ��ڲ��� ��profile:	profile��(0~14)
			  data:	д��Ƶ�ʣ���Χ0~200 000 000 Hz
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_Profile_Fre(uint8_t profile, uint32_t data)
{
	uint8_t profileAddr;
	uint8_t Profile_data[4] = {0x00, 0x00, 0x00, 0x00}; // �м����
	uint32_t Temp;
	Temp = (uint32_t)data * ACC_FRE_FACTOR; // ������Ƶ�����ӷ�Ϊ�ĸ��ֽ�  4.294967296=(2^32)/500000000
	Profile_data[3] = (uint8_t)Temp;
	Profile_data[2] = (uint8_t)(Temp >> 8);
	Profile_data[1] = (uint8_t)(Temp >> 16);
	Profile_data[0] = (uint8_t)(Temp >> 24);
	profileAddr = PROFILE_ADDR_BASE + profile;

	AD9959_WriteData(profileAddr, 4, Profile_data);
}

/************************************************************
** �������� ��void Write_Profile_Ampli(uint8_t profile,uint16_t data)
** �������� ��дProfile�Ĵ���
** ��ڲ��� ��profile:	profile��(0~14)
			  data:	 д�����,��Χ0~1023��
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_Profile_Ampli(uint8_t profile, uint16_t data)
{
	uint8_t profileAddr;
	uint8_t Profile_data[4] = {0x00, 0x00, 0x00, 0x00}; // �м����

	// Ϊ���ȵ���ʱ��������Ϊ[31:22]λ
	Profile_data[1] = (uint8_t)(data << 6); //[23:22]
	Profile_data[0] = (uint8_t)(data >> 2); //[31:24]

	profileAddr = PROFILE_ADDR_BASE + profile;

	AD9959_WriteData(profileAddr, 4, Profile_data); // д��32λ����
}

/************************************************************
** �������� ��void Write_Profile_Phase(uint8_t profile,uint16_t data)
** �������� ��дProfile�Ĵ���
** ��ڲ��� ��profile:	profile��(0~14)
			  data:	 д����λ,��Χ��0~16383
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void Write_Profile_Phase(uint8_t profile, uint16_t data)
{
	uint8_t profileAddr;
	uint8_t Profile_data[4] = {0x00, 0x00, 0x00, 0x00}; // �м����

	// Ϊ��λ����ʱ��λ����Ϊ[31:18]λ
	Profile_data[1] = (uint8_t)(data << 2); //[23:18]
	Profile_data[0] = (uint8_t)(data >> 6); //[31:24]

	profileAddr = PROFILE_ADDR_BASE + profile;

	AD9959_WriteData(profileAddr, 4, Profile_data); // д��32λ����
}

/************************************************************
** �������� ��void AD9959_Modulation_Init(uint8_t Channel,uint8_t Modulation,uint8_t Sweep_en,uint8_t Nlevel)
** �������� �����ø���ͨ���ĵ���ģʽ��
** ��ڲ��� �� Channel:  	���ͨ�� CH0-CH3
							Modulation:	����ģʽDISABLE_Mod��ASK��FSK��PSK
							Sweep_en:		����ɨ��ģʽ SWEEP_ENABLE���á�SWEEP_DISABLE�����ã�����ʱNlevelֻ����LEVEL_MOD_2
							Nlevel��		���Ƶ�ƽѡ�� LEVEL_MOD_2��4��8��16
** ���ڲ��� ����
** ����˵�� ���罫���Ƶ�ƽ����Ϊ2��ƽ����ʱ����Ӧ��P0-P3�����ϵĸߵ͵�ƽ�ֱ����CH0-CH3ͨ��(�����Ӧͨ�������Ļ�)
							�罫���Ƶ�ƽ����Ϊ4��ƽ����ʱ����Ӧ��P0��P1��P2,P3�����ϵĸߵ͵�ƽ�ֱ����CH0-CH1ͨ��(�����Ӧͨ�������Ļ�)
							����AD9959ֻ��P0-P3,4�����ڵ��ƿ��Ƶ����ţ���������4��ƽ����ʱ��ֻ����2��ͨ��ͬʱ����Ϊ���������
							8��ƽ��16��ƽ����ʱ��ֻ����1��ͨ��ͬʱ����Ϊ������������ʵ����ü���ƽ����������Ӧ������

**ע�⣡���������ó�4��ƽ����ʱ�����ͨ��ֻ��ѡ��CH0-1
							���ó�8,16��ƽ����ʱ�����ͨ��ֻ��ѡ��CH0
							������δ������ͨ�����ݣ����巽����ο�AD9959оƬ�ֲ�22-23ҳ������FR1[14:12]Ϊ��Ӧֵ��
**************************************************************/
void AD9959_Modulation_Init(uint8_t Channel, uint8_t Modulation, uint8_t Sweep_en, uint8_t Nlevel)
{
	uint8_t i = 0;
	uint8_t CHANNEL[1] = {0x00};
	uint8_t FR1_data[3];
	uint8_t FR2_data[2];
	uint8_t CFR_data[3];
	for (i = 0; i < 3; i++) // ����Ĭ��ֵ
	{
		FR1_data[i] = FR1_DATA[i];
		CFR_data[i] = CFR_DATA[i];
	}
	FR2_data[0] = FR2_DATA[0];
	FR2_data[1] = FR2_DATA[1];

	CHANNEL[0] = Channel;
	AD9959_WriteData(CSR_ADD, 1, CHANNEL); // ���ƼĴ���д��CHnͨ����ѡ��CHn���������þ����CHn

	FR1_data[1] = Nlevel;
	CFR_data[0] = Modulation;
	CFR_data[1] |= Sweep_en;
	CFR_data[2] = 0x00;

	if (Channel != 0) // ��ͨ����ʹ��
	{
		AD9959_WriteData(FR1_ADD, 3, FR1_data); // д���ܼĴ���1
		AD9959_WriteData(FR2_ADD, 2, FR2_data); // д���ܼĴ���1
		AD9959_WriteData(CFR_ADD, 3, CFR_data); // дͨ�����ܼĴ���
	}
}

/************************************************************
** �������� ��void AD9959_SetFSK(uint8_t Channel, uint32_t *data,uint16_t Phase)
** �������� ������FSK���ƵĲ���
** ��ڲ��� ��Channel:  ���ͨ�� CH0-CH3
							*data:	����Ƶ�����ݵ���ʼ��ַ
							Phase:	�����λ,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
** ���ڲ��� ����
** ����˵�� ��FSKʱ�źŷ���Ĭ��Ϊ���
**************************************************************/
void AD9959_SetFSK(uint8_t Channel, uint32_t *data, uint16_t Phase)
{
	uint8_t i = 0;
	uint8_t CHANNEL[1] = {0x00};

	CHANNEL[0] = Channel;
	AD9959_WriteData(CSR_ADD, 1, CHANNEL); // ���ƼĴ���д��CHnͨ����ѡ��CHn���������þ����CHn
	Write_CPOW0(Phase);					   // ������λ

	Write_CFTW0(data[0]);
	for (i = 0; i < 15; i++)
		Write_Profile_Fre(i, data[i + 1]);
}

/************************************************************
** �������� ��void AD9959_SetASK(uint8_t Channel, uint32_t *data,uint32_t fre,uint16_t Phase)
** �������� ������ASK���ƵĲ���
** ��ڲ��� ��Channel:  ���ͨ�� CH0-CH3
							*data: �����������ݵ���ʼ��ַ
							Freq:		���Ƶ�ʣ���Χ0~200 000 000 Hz
							Phase:	�����λ,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void AD9959_SetASK(uint8_t Channel, uint16_t *data, uint32_t fre, uint16_t Phase)
{
	uint8_t i = 0;
	uint8_t CHANNEL[1] = {0x00};

	CHANNEL[0] = Channel;
	AD9959_WriteData(CSR_ADD, 1, CHANNEL); // ���ƼĴ���д��CHnͨ����ѡ��CHn���������þ����CHn
	Write_CFTW0(fre);					   // ����Ƶ��
	Write_CPOW0(Phase);					   // ������λ

	Write_ACR(data[0]);
	for (i = 0; i < 15; i++)
		Write_Profile_Ampli(i, data[i + 1]);
}

/************************************************************
** �������� ��void AD9959_SetPSK(uint8_t Channel, uint16_t *data,uint32_t fre,uint16_t Phase)
** �������� ������PSK���ƵĲ���
** ��ڲ��� ��Channel:  ���ͨ�� CH0-CH3
							*data:	������λ���ݵ���ʼ��ַ
							Freq:		���Ƶ�ʣ���Χ0~200 000 000 Hz
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void AD9959_SetPSK(uint8_t Channel, uint16_t *data, uint32_t Freq)
{
	uint8_t i = 0;
	uint8_t CHANNEL[1] = {0x00};

	CHANNEL[0] = Channel;
	AD9959_WriteData(CSR_ADD, 1, CHANNEL); // ���ƼĴ���д��CHnͨ����ѡ��CHn���������þ����CHn
	Write_CFTW0(Freq);

	Write_CPOW0(data[0]);
	for (i = 0; i < 15; i++)
		Write_Profile_Phase(i, data[i + 1]);
}

/************************************************************
** �������� ��void AD9959_SetFre_Sweep(uint8_t Channel, uint32_t s_data,uint32_t e_data,uint8_t fsrr,uint8_t rsrr,uint32_t r_delta,uint32_t f_delta,uint16_t Phase)
** �������� ����������ɨƵ�Ĳ���
** ��ڲ��� ��Channel:  ���ͨ�� CH0-CH3
							s_data:	��ʼƵ�ʣ���Χ0~200 000 000 Hz
							e_data:	����Ƶ�ʣ���Χ0~200 000 000 Hz
							r_delta:��������Ƶ��,0~200 000 000Hz
							f_delta:�½�����Ƶ��,0~200 000 000Hz

							rsrr:		����б��,��Χ��1~255��ϵͳʱ��Ϊ500Mhzʱһ��������ԼΪ8ns
							fsrr:		�½�б��,��Χ��1~255
							Ampli:	�������,��Χ0~1023������ֵ0~1023��Ӧ�������0~500mVpp����
							Phase:	�����λ,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
** ���ڲ��� ����
** ����˵�� ��Ƶ����Ƶ���ͣ��ʱ�� dT = Xsrr*8 ��λns��ɨ�����=(��ʼ-����)/����
							ɨƵ��ʱ��=��ɨ��Ƶ����*dT
**************************************************************/
void AD9959_SetFre_Sweep(uint8_t Channel, uint32_t s_data, uint32_t e_data, uint32_t r_delta, uint32_t f_delta, uint8_t rsrr, uint8_t fsrr, uint16_t Ampli, uint16_t Phase)
{
	uint8_t CHANNEL[1] = {0x00};
	uint32_t Fer_data = 0;

	CHANNEL[0] = Channel;
	AD9959_WriteData(CSR_ADD, 1, CHANNEL); // ���ƼĴ���д��CHnͨ����ѡ��CHn���������þ����CHn

	Write_CPOW0(Phase); // ������λ
	Write_ACR(Ampli);	// ��������

	Write_LSRR(rsrr, fsrr); // б��

	Fer_data = (uint32_t)r_delta * ACC_FRE_FACTOR; // Ƶ��ת���ɿ�����
	Write_RDW(Fer_data);						   // ��������

	Fer_data = (uint32_t)f_delta * ACC_FRE_FACTOR;
	Write_FDW(Fer_data); // �½�����

	Write_CFTW0(s_data);		  // ��ʼƵ��
	Write_Profile_Fre(0, e_data); // ����Ƶ��
}

/************************************************************
** �������� ��void AD9959_SetAmp_Sweep(uint8_t Channel, uint32_t s_Ampli,uint16_t e_Ampli,uint32_t r_delta,uint32_t f_delta,uint8_t rsrr,uint8_t fsrr,uint32_t fre,uint16_t Phase)
** �������� ����������ɨ���Ĳ���
** ��ڲ��� ��Channel:  ���ͨ�� CH0-CH3
							s_Ampli:	��ʼ���ȣ�����ֵ0~1023��Ӧ�������0~500mVpp����
							e_Ampli:	�������ȣ�

							r_delta:	������������,0~1023
							f_delta:	�½���������,0~1023

							rsrr:			����б��,��Χ��1~255��ϵͳʱ��Ϊ500Mhzʱһ��������ԼΪ8ns
							fsrr:			�½�б��,��Χ��1~255

							fre:			���Ƶ�ʣ���Χ0~200 000 000 Hz
							Phase:		�����λ,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
** ���ڲ��� ����
** ����˵�� ������������ͣ��ʱ�� dT = Xsrr*8 ��λns��ɨ�����=(��ʼ-����)/����
							ɨ����ʱ��=��ɨ�������*dT
**************************************************************/
void AD9959_SetAmp_Sweep(uint8_t Channel, uint32_t s_Ampli, uint16_t e_Ampli, uint32_t r_delta, uint32_t f_delta, uint8_t rsrr, uint8_t fsrr, uint32_t fre, uint16_t Phase)
{
	uint8_t CHANNEL[1] = {0x00};
	uint8_t ACR_data[3] = {0x00, 0x00, 0x00};

	CHANNEL[0] = Channel;
	AD9959_WriteData(CSR_ADD, 1, CHANNEL); // ���ƼĴ���д��CHnͨ����ѡ��CHn���������þ����CHn

	Write_CFTW0(fre);	// ����Ƶ��
	Write_CPOW0(Phase); // ������λ

	Write_LSRR(rsrr, fsrr); // б��

	Write_RDW(r_delta << 22); // ��������

	Write_FDW(f_delta << 22); // �½�����

	ACR_data[1] = (uint8_t)(s_Ampli >> 8);	// ��λ����
	ACR_data[2] = (uint8_t)s_Ampli;			// ��λ����
	AD9959_WriteData(ACR_ADD, 3, ACR_data); // ACR �趨��ʼ����

	Write_Profile_Ampli(0, e_Ampli); // ��������
}

/************************************************************
** �������� ��void AD9959_SetPhase_Sweep(uint8_t Channel, uint16_t s_data,uint16_t e_data,uint16_t r_delta,uint16_t f_delta,uint8_t rsrr,uint8_t fsrr,uint32_t fre,uint16_t Ampli)
** �������� ����������ɨ��Ĳ���
** ��ڲ��� ��Channel:  ���ͨ�� CH0-CH3
							s_data:	��ʼ��λ����Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
							e_data:	������λ��
							r_delta:��������,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
							f_delta:�½�����,

							rsrr:		����б��,��Χ��1~255��ϵͳʱ��Ϊ500Mhzʱһ��������ԼΪ8ns
							fsrr:		�½�б��,��Χ��1~255
							fre:		���Ƶ�ʣ���Χ0~200 000 000 Hz
							Ampli:	�������,��Χ0~1023������ֵ0~1023��Ӧ�������0~500mVpp����
** ���ڲ��� ����
** ����˵�� ��Ƶ����Ƶ���ͣ��ʱ�� dT = Xsrr*8 ��λns��ɨ�����=(��ʼ-����)/����
							ɨƵ��ʱ��=��ɨ��Ƶ����*dT
**************************************************************/
void AD9959_SetPhase_Sweep(uint8_t Channel, uint16_t s_data, uint16_t e_data, uint16_t r_delta, uint16_t f_delta, uint8_t rsrr, uint8_t fsrr, uint32_t fre, uint16_t Ampli)
{
	uint8_t CHANNEL[1] = {0x00};

	CHANNEL[0] = Channel;
	AD9959_WriteData(CSR_ADD, 1, CHANNEL); // ���ƼĴ���д��CHnͨ����ѡ��CHn���������þ����CHn

	Write_CFTW0(fre); // ����Ƶ��
	Write_ACR(Ampli); // ��������

	Write_LSRR(rsrr, fsrr); // б��

	Write_RDW(r_delta << 18); // ��������

	Write_FDW(f_delta << 18); // �½�����

	Write_CPOW0(s_data);			// ��ʼ��λ
	Write_Profile_Phase(0, e_data); // ������λ
}

/*---------------------------------------
�������ܣ�����ͨ�����Ƶ��
Channel:  ���ͨ��
Freq:     ���Ƶ��
---------------------------------------*/
void AD9959_WriteFreq(uint8_t Channel, uint32_t Freq)
{
	uint8_t CFTW0_DATA[4] = {0x00, 0x00, 0x00, 0x00}; // �м����
	uint32_t Temp;
	Temp = (uint32_t)Freq * 8.589934592; // ������Ƶ�����ӷ�Ϊ�ĸ��ֽ�  8.589934592=(2^32)/500000000 ����500M=25M*20(��Ƶ���ɱ��)
	CFTW0_DATA[3] = (uint8_t)Temp;
	CFTW0_DATA[2] = (uint8_t)(Temp >> 8);
	CFTW0_DATA[1] = (uint8_t)(Temp >> 16);
	CFTW0_DATA[0] = (uint8_t)(Temp >> 24);
	if (Channel == 0)
	{
		WriteData_AD9959(CSR_ADD, 1, CSR_DATA0, 1);	   // ���ƼĴ���д��CH0ͨ��
		WriteData_AD9959(CFTW0_ADD, 4, CFTW0_DATA, 1); // CTW0 address 0x04.���CH0�趨Ƶ��
	}
	else if (Channel == 1)
	{
		WriteData_AD9959(CSR_ADD, 1, CSR_DATA1, 1);	   // ���ƼĴ���д��CH1ͨ��
		WriteData_AD9959(CFTW0_ADD, 4, CFTW0_DATA, 1); // CTW0 address 0x04.���CH1�趨Ƶ��
	}
	else if (Channel == 2)
	{
		WriteData_AD9959(CSR_ADD, 1, CSR_DATA2, 1);	   // ���ƼĴ���д��CH2ͨ��
		WriteData_AD9959(CFTW0_ADD, 4, CFTW0_DATA, 1); // CTW0 address 0x04.���CH2�趨Ƶ��
	}
	else if (Channel == 3)
	{
		WriteData_AD9959(CSR_ADD, 1, CSR_DATA3, 1);	   // ���ƼĴ���д��CH3ͨ��
		WriteData_AD9959(CFTW0_ADD, 4, CFTW0_DATA, 1); // CTW0 address 0x04.���CH3�趨Ƶ��
	}
}
/*---------------------------------------
�������ܣ�����ͨ���������
Channel:  ���ͨ��
Ampli:    �������
---------------------------------------*/
void AD9959_WriteAmpl(uint8_t Channel, uint16_t Ampli)
{
	uint16_t A_temp;						  //=0x23ff;
	uint8_t ACR_DATA[3] = {0x00, 0x00, 0x00}; // default Value = 0x--0000 Rest = 18.91/Iout
	A_temp = Ampli | 0x1000;
	ACR_DATA[2] = (uint8_t)A_temp;		  // ��λ����
	ACR_DATA[1] = (uint8_t)(A_temp >> 8); // ��λ����

	if (Channel == 0)
	{
		WriteData_AD9959(CSR_ADD, 1, CSR_DATA0, 1);
		WriteData_AD9959(ACR_ADD, 3, ACR_DATA, 1);
	}

	else if (Channel == 1)
	{
		WriteData_AD9959(CSR_ADD, 1, CSR_DATA1, 1);
		WriteData_AD9959(ACR_ADD, 3, ACR_DATA, 1);
	}

	else if (Channel == 2)
	{
		WriteData_AD9959(CSR_ADD, 1, CSR_DATA2, 1);
		WriteData_AD9959(ACR_ADD, 3, ACR_DATA, 1);
	}

	else if (Channel == 3)
	{
		WriteData_AD9959(CSR_ADD, 1, CSR_DATA3, 1);
		WriteData_AD9959(ACR_ADD, 3, ACR_DATA, 1);
	}
}
/*---------------------------------------
�������ܣ�����ͨ�������λ
Channel:  ���ͨ��
Phase:    �����λ,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
---------------------------------------*/
void AD9959_WritePhase(uint8_t Channel, uint16_t Phase)
{
	uint16_t P_temp = 0;
	P_temp = (uint16_t)Phase;
	CPOW0_DATA[1] = (uint8_t)P_temp;
	CPOW0_DATA[0] = (uint8_t)(P_temp >> 8);
	if (Channel == 0)
	{
		WriteData_AD9959(CSR_ADD, 1, CSR_DATA0, 1);
		WriteData_AD9959(CPOW0_ADD, 2, CPOW0_DATA, 1);
	}

	else if (Channel == 1)
	{
		WriteData_AD9959(CSR_ADD, 1, CSR_DATA1, 1);
		WriteData_AD9959(CPOW0_ADD, 2, CPOW0_DATA, 1);
	}

	else if (Channel == 2)
	{
		WriteData_AD9959(CSR_ADD, 1, CSR_DATA2, 1);
		WriteData_AD9959(CPOW0_ADD, 2, CPOW0_DATA, 1);
	}

	else if (Channel == 3)
	{
		WriteData_AD9959(CSR_ADD, 1, CSR_DATA3, 1);
		WriteData_AD9959(CPOW0_ADD, 2, CPOW0_DATA, 1);
	}
}

// AD9959��ʼ��
void AD9959_Init(void)
{
	uint8_t FR1_DATA[3] = {0xD0, 0x00, 0x00}; // 20��Ƶ Charge pump control = 75uA FR1<23> -- VCO gain control =0ʱ system clock below 160 MHz;

	AD9959_Drv_Init();

	Intserve(); // IO�ڳ�ʼ��
	IntReset(); // AD9959��λ

	WriteData_AD9959(FR1_ADD, 3, FR1_DATA, 1); // д���ܼĴ���1
	WriteData_AD9959(FR2_ADD, 2, FR2_DATA, 1);

#if 0
	WriteData_AD9959(CFR_ADD,3,CFR_DATA,1);
	WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,0);
	WriteData_AD9959(ACR_ADD,3,ACR_DATA,0);
	WriteData_AD9959(LSRR_ADD,2,LSRR_DATA,0);
	WriteData_AD9959(RDW_ADD,2,RDW_DATA,0);
	WriteData_AD9959(FDW_ADD,4,FDW_DATA,1);
#endif

	// д���ʼƵ��
	AD9959_WriteFreq(3, SinFre[3]);
	AD9959_WriteFreq(0, SinFre[0]);
	AD9959_WriteFreq(1, SinFre[1]);
	AD9959_WriteFreq(2, SinFre[2]);

	AD9959_WritePhase(3, SinPhr[3]);
	AD9959_WritePhase(0, SinPhr[0]);
	AD9959_WritePhase(1, SinPhr[1]);
	AD9959_WritePhase(2, SinPhr[2]);

	AD9959_WriteAmpl(3, SinAmp[3]);
	AD9959_WriteAmpl(0, SinAmp[0]);
	AD9959_WriteAmpl(1, SinAmp[1]);
	AD9959_WriteAmpl(2, SinAmp[2]);
}
#endif
