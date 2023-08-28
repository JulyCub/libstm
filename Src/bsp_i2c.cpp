/**
 * @file bsp_i2c.cpp
 * @author July (Email: JulyCub@163.com)
 * @brief i2C Hardware
 * @version 0.1
 * @date 2023.08.28
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "bsp_i2c.hpp"

#define DELAY_TIME 20

/**
 * @brief ��ʼ��I2C
 *
 * @param name �豸����
 * @param port GPIO�˿ں�
 * @param sda_pin SDA����
 * @param scl_pin SCL����
 */
void BSP_I2C_CLASS::Init(const std::string &name,
                         GPIO_TypeDef port,
                         uint16_t sda_pin,
                         uint16_t scl_pin)
{
  device = name;
  Port = port;

  __BSP_RCC_GPIO_ENABLE(Port);

  SDA_Init.Pin = sda_pin;
  SDA_Init.Mode = GPIO_MODE_OUTPUT_PP;
  SDA_Init.Pull = GPIO_PULLUP;
  SDA_Init.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(Port, &SDA_Init);

  SCL_Init.Pin = scl_pin;
  SCL_Init.Mode = GPIO_MODE_OUTPUT_PP;
  SCL_Init.Pull = GPIO_PULLUP;
  SCL_Init.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(Port, &SCL_Init);
}

/**
 * @brief DeInit I2C
 */
void BSP_I2C_CLASS::DeInit(void)
{
  HAL_GPIO_DeInit(Port, SDA_Init.Pin);
  HAL_GPIO_DeInit(Port, SCL_Init.Pin);
}

/**
 * @brief SDA������ģʽ����
 * @param None
 * @retval None
 */
void BSP_I2C_CLASS::SDA_Input_Mode(void)
{
  SDA_Init.Mode = GPIO_MODE_INPUT;
  HAL_GPIO_Init(Port, &SDA_Init);
}

/**
 * @brief SDA�����ģʽ����
 * @param None
 * @retval None
 */
void BSP_I2C_CLASS::SDA_Output_Mode(void)
{
  SDA_Init.Mode = GPIO_MODE_OUTPUT_OD;
  HAL_GPIO_Init(Port, &SDA_Init);
}

/**
 * @brief SDA�����һ��λ
 * @param val ���������
 * @retval None
 */
void BSP_I2C_CLASS::SDA_Output(uint16_t val)
{
  if (val)
  {
    Port->BSRR |= SDA_Init.Pin;
  }
  else
  {
    Port->ODR |= SDA_Init.Pin;
  }
}

/**
 * @brief SCL�����һ��λ
 * @param val ���������
 * @retval None
 */
void BSP_I2C_CLASS::SCL_Output(uint16_t val)
{
  if (val)
  {
    Port->BSRR |= SCL_Init.Pin;
  }
  else
  {
    Port->ODR |= SCL_Init.Pin;
  }
}

/**
 * @brief SDA����һλ
 * @param None
 * @retval GPIO����һλ
 */
uint8_t BSP_I2C_CLASS::SDA_Input(void)
{
  if (HAL_GPIO_ReadPin(Port, SDA_Init.Pin) == GPIO_PIN_SET)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
 * @brief I2C�Ķ�����ʱ
 * @param None
 * @retval None
 */
void BSP_I2C_CLASS::delay(uint16_t n)
{
  uint32_t i;
  for (i = 0; i < n; ++i)
    ;
}

/**
 * @brief I2C��ʼ�ź�
 * @param None
 * @retval None
 */
void BSP_I2C_CLASS::Start(void)
{
  BSP_I2C_CLASS::SDA_Output(1);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  BSP_I2C_CLASS::SCL_Output(1);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  BSP_I2C_CLASS::SDA_Output(0);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  BSP_I2C_CLASS::SCL_Output(0);
  BSP_I2C_CLASS::delay(DELAY_TIME);
}

/**
 * @brief I2C�����ź�
 * @param None
 * @retval None
 */
void BSP_I2C_CLASS::Stop(void)
{
  BSP_I2C_CLASS::SCL_Output(0);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  BSP_I2C_CLASS::SDA_Output(0);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  BSP_I2C_CLASS::SCL_Output(1);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  BSP_I2C_CLASS::SDA_Output(1);
  BSP_I2C_CLASS::delay(DELAY_TIME);
}

/**
 * @brief I2C�ȴ�ȷ���ź�
 * @param None
 * @retval None
 */
uint8_t BSP_I2C_CLASS::WaitACK(void)
{
  unsigned short cErrTime = 5;

  BSP_I2C_CLASS::SDA_Input_Mode();
  BSP_I2C_CLASS::delay(DELAY_TIME);
  BSP_I2C_CLASS::SCL_Output(1);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  while (BSP_I2C_CLASS::SDA_Input())
  {
    cErrTime--;
    BSP_I2C_CLASS::delay(DELAY_TIME);
    if (0 == cErrTime)
    {
      BSP_I2C_CLASS::SDA_Output_Mode();
      BSP_I2C_CLASS::Stop();
      return ERROR;
    }
  }
  BSP_I2C_CLASS::SDA_Output_Mode();
  BSP_I2C_CLASS::SCL_Output(0);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  return SUCCESS;
}

/**
 * @brief I2C�ȴ�NO ACK�ź�
 * @param None
 * @retval None
 */
uint8_t BSP_I2C_CLASS::WaitNotACK(void)
{
  unsigned short cErrTime = 5;
  BSP_I2C_CLASS::SDA_Input_Mode();
  BSP_I2C_CLASS::delay(DELAY_TIME);
  BSP_I2C_CLASS::SCL_Output(1);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  while (!BSP_I2C_CLASS::SDA_Input())
  {
    cErrTime--;
    BSP_I2C_CLASS::delay(DELAY_TIME);
    if (0 == cErrTime)
    {
      BSP_I2C_CLASS::SDA_Output_Mode();
      BSP_I2C_CLASS::Stop();
      return ERROR;
    }
  }
  BSP_I2C_CLASS::SDA_Output_Mode();
  BSP_I2C_CLASS::SCL_Output(0);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  return SUCCESS;
}

/**
 * @brief I2C����ȷ���ź�
 * @param None
 * @retval None
 */
void BSP_I2C_CLASS::SendACK(void)
{
  BSP_I2C_CLASS::SDA_Output(0);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  BSP_I2C_CLASS::SCL_Output(1);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  BSP_I2C_CLASS::SCL_Output(0);
  BSP_I2C_CLASS::delay(DELAY_TIME);
}

/**
 * @brief I2C���ͷ�ȷ���ź�
 * @param None
 * @retval None
 */
void BSP_I2C_CLASS::SendNotACK(void)
{
  BSP_I2C_CLASS::SDA_Output(1);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  BSP_I2C_CLASS::SCL_Output(1);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  BSP_I2C_CLASS::SCL_Output(0);
  BSP_I2C_CLASS::delay(DELAY_TIME);
}

/**
 * @brief I2C����һ���ֽ�
 * @param byte ��Ҫ���͵��ֽ�
 * @retval None
 */
void BSP_I2C_CLASS::SendByte(uint8_t byte)
{
  unsigned char i = 8;
  while (i--)
  {
    BSP_I2C_CLASS::SCL_Output(0);
    BSP_I2C_CLASS::delay(DELAY_TIME);
    BSP_I2C_CLASS::SDA_Output(byte & 0x80);
    BSP_I2C_CLASS::delay(DELAY_TIME);
    byte += byte;
    BSP_I2C_CLASS::delay(DELAY_TIME);
    BSP_I2C_CLASS::SCL_Output(1);
    BSP_I2C_CLASS::delay(DELAY_TIME);
  }
  BSP_I2C_CLASS::SCL_Output(0);
  BSP_I2C_CLASS::delay(DELAY_TIME);
}

/**
 * @brief I2C����һ���ֽ�
 * @param None
 * @retval ���յ����ֽ�
 */
uint8_t BSP_I2C_CLASS::ReceiveByte(void)
{
  unsigned char i = 8;
  unsigned char byte = 0;
  BSP_I2C_CLASS::SDA_Input_Mode();
  while (i--)
  {
    byte += byte;
    BSP_I2C_CLASS::SCL_Output(0);
    BSP_I2C_CLASS::delay(DELAY_TIME);
    BSP_I2C_CLASS::delay(DELAY_TIME);
    BSP_I2C_CLASS::SCL_Output(1);
    BSP_I2C_CLASS::delay(DELAY_TIME);
    byte |= BSP_I2C_CLASS::SDA_Input();
  }
  BSP_I2C_CLASS::SCL_Output(0);
  BSP_I2C_CLASS::delay(DELAY_TIME);
  BSP_I2C_CLASS::SDA_Output_Mode();
  return byte;
}
