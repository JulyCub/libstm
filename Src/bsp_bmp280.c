/**
 * @file bsp_bmp280.c
 * @author July (Email: JulyCub@163.com)
 * @brief BMP280 Hardware
 * @version 0.1
 * @date 2023.09.02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "bsp_bmp280.h"

BMP280 bmp280_inst;
BMP280 *bmp280 = &bmp280_inst;

void BSP_BMP280_SDA_InputMode(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = BMP280_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(BMP280_GPIO_PORT, &GPIO_InitStruct);
}

void BSP_BMP280_SDA_OutputMode(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = BMP280_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(BMP280_GPIO_PORT, &GPIO_InitStruct);
}

void BSP_BMP280_SDA_SendByte(uint16_t val)
{
    if (val) {
        BMP280_GPIO_PORT->BSRR |= BMP280_SDA_PIN;
    }
    else {
        BMP280_GPIO_PORT->ODR |= BMP280_SDA_PIN;
    }
}

void BSP_BMP280_SCL_SendByte(uint16_t val)
{
    if (val) {
        BMP280_GPIO_PORT->BSRR |= BMP280_SCL_PIN;
    }
    else {
        BMP280_GPIO_PORT->ODR |= BMP280_SCL_PIN;
    }
}

uint8_t BSP_BMP280_SDA_RecvByte(void)
{
    if (HAL_GPIO_ReadPin(BMP280_GPIO_PORT, BMP280_SDA_PIN) == GPIO_PIN_SET)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static void delay(uint16_t n)
{
    uint16_t i;
    for (i = 0; i > n; i++);
}

void BSP_BMP280_I2C_Start(void)
{
    BSP_BMP280_SDA_SendByte(1);
    delay(DELAY_TIME);
    BSP_BMP280_SCL_SendByte(1);
    delay(DELAY_TIME);
    BSP_BMP280_SDA_SendByte(0);
    delay(DELAY_TIME);
    BSP_BMP280_SCL_SendByte(0);
    delay(DELAY_TIME);
}

void BSP_BMP280_I2C_Stop(void)
{
    BSP_BMP280_SDA_SendByte(0);
    delay(DELAY_TIME);
    BSP_BMP280_SCL_SendByte(0);
    delay(DELAY_TIME);
    BSP_BMP280_SDA_SendByte(1);
    delay(DELAY_TIME);
    BSP_BMP280_SCL_SendByte(1);
    delay(DELAY_TIME);
}

uint8_t BSP_BMP280_I2C_WaitACK(void)
{
    uint8_t cErrTime = 5;

    BSP_BMP280_SDA_InputMode();
    delay(DELAY_TIME);
    BSP_BMP280_SCL_SendByte(1);
    delay(DELAY_TIME);
    while(BSP_BMP280_SDA_RecvByte())
    {
        cErrTime--;
        delay(DELAY_TIME);
        if (0 == cErrTime)
        {
            BSP_BMP280_SDA_OutputMode();
            BSP_BMP280_I2C_Stop();
            return ERROR;
        }
    }
    BSP_BMP280_SDA_OutputMode();
    BSP_BMP280_SCL_SendByte(0);
    delay(DELAY_TIME);
    return SUCCESS;
}

uint8_t BSP_BMP280_WaitNotACK(void)
{
    uint8_t cErrTime = 5;

    BSP_BMP280_SDA_InputMode();
    delay(DELAY_TIME);
    BSP_BMP280_SCL_SendByte(1);
    delay(DELAY_TIME);
    while (BSP_BMP280_SDA_RecvByte())
    {
        cErrTime--;
        delay(DELAY_TIME);
        if (0 == cErrTime)
        {
            BSP_BMP280_SDA_OutputMode();
            BSP_BMP280_I2C_Stop();
            return ERROR;
        }
    }
    BSP_BMP280_SDA_OutputMode();
    BSP_BMP280_SCL_SendByte(0);
    delay(DELAY_TIME);
    return SUCCESS;
}

void BSP_BMP280_I2C_SendACK(void)
{
    BSP_BMP280_SDA_SendByte(0);
    delay(DELAY_TIME * 2);
    BSP_BMP280_SCL_SendByte(1);
    delay(DELAY_TIME);
    BSP_BMP280_SCL_SendByte(0);
    delay(DELAY_TIME);
}

void BSP_BMP280_I2C_SendNotACK(void)
{
    BSP_BMP280_SDA_SendByte(1);
    delay(DELAY_TIME * 2);
    BSP_BMP280_SCL_SendByte(1);
    delay(DELAY_TIME);
    BSP_BMP280_SCL_SendByte(0);
    delay(DELAY_TIME);
}

void BSP_BMP280_I2C_SendByte(uint8_t byte)
{
    uint8_t i = 8;
    while (i--)
    {
        BSP_BMP280_SCL_SendByte(0);
        delay(DELAY_TIME);
        BSP_BMP280_SDA_SendByte(byte & 0x80);
        delay(DELAY_TIME);
        byte += byte;
        delay(DELAY_TIME);
        BSP_BMP280_SCL_SendByte(1);
        delay(DELAY_TIME);
    }
    BSP_BMP280_SCL_SendByte(0);
    delay(DELAY_TIME);
}

uint8_t BSP_BMP280_I2C_RecvByte(void)
{
    uint8_t i = 8;
    uint8_t byte = 0;

    BSP_BMP280_SDA_InputMode();
    while (i--)
    {
        byte += byte;
        BSP_BMP280_SCL_SendByte(0);
        delay(DELAY_TIME * 2);
        BSP_BMP280_SCL_SendByte(1);
        delay(DELAY_TIME);
        byte |= BSP_BMP280_SDA_RecvByte();
    }
    BSP_BMP280_SCL_SendByte(0);
    delay(DELAY_TIME);
    BSP_BMP280_SDA_OutputMode();
    return byte;
}

static uint8_t bmp280_read_byte(uint8_t reg_addr)
{
    uint8_t data;

    BSP_BMP280_I2C_Start();
    BSP_BMP280_I2C_SendByte(BMP280_ADDRESS << 1 | 0);
    BSP_BMP280_I2C_WaitACK();
    BSP_BMP280_I2C_SendByte(reg_addr);
    BSP_BMP280_I2C_WaitACK();

    BSP_BMP280_I2C_Start();
    BSP_BMP280_I2C_SendByte(BMP280_ADDRESS << 1 | 1);
    BSP_BMP280_I2C_WaitACK();
    data = BSP_BMP280_I2C_RecvByte();
    BSP_BMP280_I2C_Stop();
    return data;
}

static void bmp280_write_byte(uint8_t reg_addr, uint8_t data)
{
    BSP_BMP280_I2C_Start();
    BSP_BMP280_I2C_SendByte(BMP280_ADDRESS << 1);
    BSP_BMP280_I2C_WaitACK();
    BSP_BMP280_I2C_SendByte(reg_addr);
    BSP_BMP280_I2C_WaitACK();

    BSP_BMP280_I2C_SendByte(data);
    BSP_BMP280_I2C_WaitACK();
    BSP_BMP280_I2C_Stop();
}

// 设置BMP过采样因子 MODE
// BMP280_SLEEP_MODE||BMP280_FORCED_MODE||BMP280_NORMAL_MODE
static void bmp280_set_temoversamp(BMP_OVERSAMPLE_MODE *Oversample_Mode)
{
    u8 Regtmp;
    Regtmp = ((Oversample_Mode->T_Osample) << 5) |
             ((Oversample_Mode->P_Osample) << 2) |
             ((Oversample_Mode)->WORKMODE);

    bmp280_write_byte(BMP280_CTRLMEAS_REG, Regtmp);
}

// 设置保持时间和滤波器分频因子
static void bmp280_set_standby_filter(BMP_CONFIG *BMP_Config)
{
    u8 Regtmp;
    Regtmp = ((BMP_Config->T_SB) << 5) |
             ((BMP_Config->FILTER_COEFFICIENT) << 2) |
             ((BMP_Config->SPI_EN));

    bmp280_write_byte(BMP280_CONFIG_REG, Regtmp);
}

BMP280_S32_t t_fine; // 用于计算补偿
#ifdef USE_FIXED_POINT_COMPENSATE
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
    BMP280_S32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((BMP280_S32_t)dig_T1 << 1))) * ((BMP280_S32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((BMP280_S32_t)dig_T1)) * ((adc_T >> 4) - ((BMP280_S32_t)dig_T1))) >> 12) *
            ((BMP280_S32_t)dig_T3)) >>
           14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P)
{
    BMP280_S64_t var1, var2, p;
    var1 = ((BMP280_S64_t)t_fine) - 128000;
    var2 = var1 * var1 * (BMP280_S64_t)dig_P6;
    var2 = var2 + ((var1 * (BMP280_S64_t)dig_P5) << 17);
    var2 = var2 + (((BMP280_S64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (BMP280_S64_t)dig_P3) >> 8) + ((var1 * (BMP280_S64_t)dig_P2) << 12);
    var1 = (((((BMP280_S64_t)1) << 47) + var1)) * ((BMP280_S64_t)dig_P1) >> 33;
    if (var1 == 0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((BMP280_S64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((BMP280_S64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)dig_P7) << 4);
    return (BMP280_U32_t)p;
}
#else

// Returns temperature in DegC, double precision. Output value of “51.23” equals 51.23 DegC.
// t_fine carries fine temperature as global value
double bmp280_compensate_T_double(BMP280_S32_t adc_T)
{
    double var1, var2, T;
    var1 = (((double)adc_T) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
    var2 = ((((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0) *
            (((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0)) *
           ((double)dig_T3);
    t_fine = (BMP280_S32_t)(var1 + var2);
    T = (var1 + var2) / 5120.0;
    return T;
}

// Returns pressure in Pa as double. Output value of “96386.2” equals 96386.2 Pa = 963.862 hPa
double bmp280_compensate_P_double(BMP280_S32_t adc_P)
{
    double var1, var2, p;
    var1 = ((double)t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double)dig_P4) * 65536.0);
    var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)dig_P1);
    if (var1 == 0.0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576.0 - (double)adc_P;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double)dig_P7)) / 16.0;
    return p;
}
#endif

/**
 * @brief read the chip ID of bmp280
 * @return uint8_t the chip ID
 */
uint8_t BSP_BMP280_ReadID(void)
{
    return bmp280_read_byte(BMP280_CHIPID_REG);
}

/**
 * @brief init bmp280
 */
void BSP_BMP280_Init(void)
{
    uint8_t Lsb, Msb;
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __BSP_RCC_GPIO_ENABLE(BMP280_GPIO_PORT);

    GPIO_InitStruct.Pin = BMP280_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BMP280_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BMP280_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BMP280_GPIO_PORT, &GPIO_InitStruct);

    // Temperature sensor correction value
    Lsb = bmp280_read_byte(BMP280_DIG_T1_LSB_REG);
    Msb = bmp280_read_byte(BMP280_DIG_T1_MSB_REG);
    bmp280->T1 = (((u16)Msb) << 8) + Lsb;
    Lsb = bmp280_read_byte(BMP280_DIG_T2_LSB_REG);
    Msb = bmp280_read_byte(BMP280_DIG_T2_MSB_REG);
    bmp280->T2 = (((u16)Msb) << 8) + Lsb;
    Lsb = bmp280_read_byte(BMP280_DIG_T3_LSB_REG);
    Msb = bmp280_read_byte(BMP280_DIG_T3_MSB_REG);
    bmp280->T3 = (((u16)Msb) << 8) + Lsb;

    // Atmospheric sensor correction value
    Lsb = bmp280_read_byte(BMP280_DIG_P1_LSB_REG);
    Msb = bmp280_read_byte(BMP280_DIG_P1_MSB_REG);
    bmp280->P1 = (((u16)Msb) << 8) + Lsb;
    Lsb = bmp280_read_byte(BMP280_DIG_P2_LSB_REG);
    Msb = bmp280_read_byte(BMP280_DIG_P2_MSB_REG);
    bmp280->P2 = (((u16)Msb) << 8) + Lsb;
    Lsb = bmp280_read_byte(BMP280_DIG_P3_LSB_REG);
    Msb = bmp280_read_byte(BMP280_DIG_P3_MSB_REG);
    bmp280->P3 = (((u16)Msb) << 8) + Lsb;
    Lsb = bmp280_read_byte(BMP280_DIG_P4_LSB_REG);
    Msb = bmp280_read_byte(BMP280_DIG_P4_MSB_REG);
    bmp280->P4 = (((u16)Msb) << 8) + Lsb;
    Lsb = bmp280_read_byte(BMP280_DIG_P5_LSB_REG);
    Msb = bmp280_read_byte(BMP280_DIG_P5_MSB_REG);
    bmp280->P5 = (((u16)Msb) << 8) + Lsb;
    Lsb = bmp280_read_byte(BMP280_DIG_P6_LSB_REG);
    Msb = bmp280_read_byte(BMP280_DIG_P6_MSB_REG);
    bmp280->P6 = (((u16)Msb) << 8) + Lsb;
    Lsb = bmp280_read_byte(BMP280_DIG_P7_LSB_REG);
    Msb = bmp280_read_byte(BMP280_DIG_P7_MSB_REG);
    bmp280->P7 = (((u16)Msb) << 8) + Lsb;
    Lsb = bmp280_read_byte(BMP280_DIG_P8_LSB_REG);
    Msb = bmp280_read_byte(BMP280_DIG_P8_MSB_REG);
    bmp280->P8 = (((u16)Msb) << 8) + Lsb;
    Lsb = bmp280_read_byte(BMP280_DIG_P9_LSB_REG);
    Msb = bmp280_read_byte(BMP280_DIG_P9_MSB_REG);
    bmp280->P9 = (((u16)Msb) << 8) + Lsb;

    bmp280_write_byte(BMP280_RESET_REG, BMP280_RESET_VALUE);

    BMP_OVERSAMPLE_MODE BMP_OVERSAMPLE_MODEStructure;
    BMP_OVERSAMPLE_MODEStructure.P_Osample = BMP280_P_MODE_3;
    BMP_OVERSAMPLE_MODEStructure.T_Osample = BMP280_T_MODE_1;
    BMP_OVERSAMPLE_MODEStructure.WORKMODE = BMP280_NORMAL_MODE;
    bmp280_set_temoversamp(&BMP_OVERSAMPLE_MODEStructure);

    BMP_CONFIG BMP_CONFIGStructure;
    BMP_CONFIGStructure.T_SB = BMP280_T_SB1;
    BMP_CONFIGStructure.FILTER_COEFFICIENT = BMP280_FILTER_MODE_4;
    BMP_CONFIGStructure.SPI_EN = DISABLE;

    bmp280_set_standby_filter(&BMP_CONFIGStructure);
}

/**
 * @brief get now bmp280 status
 * @param status_flag BMP280_MEASURING || BMP280_IM_UPDATE
 * @return uint8_t
 */
uint8_t BSP_BMP280_GetStatus(uint8_t status_flag)
{
    uint8_t flag;
    flag = bmp280_read_byte(BMP280_STATUS_REG);
    if (flag & status_flag)
        return SET;
    else
        return RESET;
}

/**
 * @brief get the pressure
 * @return double pressure -Pa
 */
double BSP_BMP280_Get_Pressure(void)
{
    uint8_t XLsb, Lsb, Msb;
    long signed Bit32;
    double pressure;

    XLsb = bmp280_read_byte(BMP280_PRESSURE_XLSB_REG);
    Lsb = bmp280_read_byte(BMP280_PRESSURE_LSB_REG);
    Msb = bmp280_read_byte(BMP280_PRESSURE_MSB_REG);
    Bit32 = ((long)(Msb << 12)) | ((long)(Lsb << 4)) | (XLsb >> 4);
    pressure = bmp280_compensate_P_double(Bit32);
    return pressure;
}

/**
 * @brief get the temperature
 * @return double T -°C
 */
double BSP_BMP280_Get_Temperature(void)
{
    uint8_t XLsb, Lsb, Msb;
    long signed Bit32;
    double temperature;

    XLsb = bmp280_read_byte(BMP280_TEMPERATURE_XLSB_REG);
    Lsb = bmp280_read_byte(BMP280_TEMPERATURE_LSB_REG);
    Msb = bmp280_read_byte(BMP280_TEMPERATURE_MSB_REG);
    Bit32 = ((long)(Msb << 12)) | ((long)(Lsb << 4)) | (XLsb >> 4);
    temperature = bmp280_compensate_T_double(Bit32);
    return temperature;
}
