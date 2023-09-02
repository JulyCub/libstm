/**
 * @file bsp_gp2y.c
 * @author fire
 * @brief PM2.5/�۳����
 * @version 0.1
 * @date 2023.08.22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "bsp_gp2y.h"

static uint16_t buffer[MAX_NUM]; /*��������ȫ�ֱ���*/
ADC_HandleTypeDef ADC_Handle;

/**
 * @brief  ���黺��ADC��ֵ
 * @param  valve��ѹ����ֵ
 * @retval ��
 */
void GP2Y_GetDate(uint16_t valve)
{
  static uint8_t k = 0;

  buffer[k] = valve;
  k++;
  if (k == MAX_NUM)
  {
    k = 0;
  }
}

/**
 * @brief  �����ڻ���ADC��ѹ�ɼ�����
 * @param  ��
 * @retval ��
 */
void Collect_data(void)
{
  uint8_t i;

  for (i = 0; i < 2; i++)
  {
    GP2Y_ILED_SET(ILED_HIGH);
    /* �ȴ�ģ�������ѹ�ȶ� */
    delay_us(280);

    /* ��ʼadcת����������� */
    HAL_ADC_Start(&ADC_Handle);

    while (__HAL_ADC_GET_FLAG(&ADC_Handle, ADC_FLAG_EOC) != SET);

    /* ���黺��ADCת����ֵ*/
    GP2Y_GetDate(HAL_ADC_GetValue(&ADC_Handle));

    /* ���ADC�����־λ*/
    __HAL_ADC_CLEAR_FLAG(&ADC_Handle, ADC_FLAG_EOC);

    /* �ر�ILED�� */
    GP2Y_ILED_SET(ILED_LOW);

    HAL_Delay(5);
  }
}

/**
 * @brief  ��ѹֵת��Ϊ�ҳ�Ũ��ֵ,��λ��g/m3
 * @param  dens  �ҳ�Ũ��ֵ
 * @retval ADC_ConvertFlagŨ��ת����־λ
 */
double GP2Y_Convert(void)
{
  uint8_t i;
  uint16_t sum = 0;
  double voltage, dens;

  for (i = 0; i < MAX_NUM; i++)
  {
    sum += buffer[i];
  }
  voltage = (sum / 10.0) * (3300 / 4096.0) * 11; /* ʵ��ƽ����ѹֵ */

  if (voltage <= NO_DUST_VOLTAGE)
  {
    return 0;
  }
  else
  {
    dens = (voltage - NO_DUST_VOLTAGE) * RATIO; /* ��ѹֵ����ɻҳ�Ũ��ֵ */
    return dens;
  }
}

double GP2Y_GetDens(void)
{
  double dens;
  /*���ɼ�����ADת����ֵ���浽������*/
  Collect_data();
  dens = GP2Y_Convert();
  return dens;
}

/**
 * @brief  ADC�������ú���
 * @param  ��
 * @retval ��
 */
static void ADC_GPIO_Mode_Config(void)
{
  /* ����һ��GPIO_InitTypeDef���͵Ľṹ�� */
  GPIO_InitTypeDef GPIO_InitStruct;
  /* ʹ��ADC���ŵ�ʱ�� */
  GP2Y_ADC_GPIO_CLK_ENABLE();
  /* ʹ��ILEDʱ�� */
  GP2Y_ILED_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GP2Y_ADC_PIN;
  /* ����Ϊģ�����룬����Ҫ�������� */
  HAL_GPIO_Init(GP2Y_ADC_GPIO_PORT, &GPIO_InitStruct);

  /*ѡ��Ҫ���Ƶ�GPIO����*/
  GPIO_InitStruct.Pin = GP2Y_ILED_PIN;
  /*�������ŵ��������Ϊ�������*/
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  /*������������Ϊ���� */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  /*��ʼ�� ILED IO*/
  HAL_GPIO_Init(GP2Y_ILED_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GP2Y_ILED_PORT, GP2Y_ILED_PIN, (GPIO_PinState)0);
}

/**
 * @brief  ADC����ģʽ���ú���
 * @param  ��
 * @retval ��
 */
static void ADC_Mode_Config(void)
{
  ADC_ChannelConfTypeDef ADC_Config;

  RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;
  /*            ����ADC3ʱ��Դ             */
  /*    HSE Frequency(Hz)    = 25000000   */
  /*         PLL_M                = 5     */
  /*         PLL_N                = 160   */
  /*         PLL_P                = 25    */
  /*         PLL_Q                = 2     */
  /*         PLL_R                = 2     */
  /*     ADC_ker_clk         = 32000000   */
  RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  RCC_PeriphClkInit.PLL2.PLL2FRACN = 0;
  RCC_PeriphClkInit.PLL2.PLL2M = 5;
  RCC_PeriphClkInit.PLL2.PLL2N = 160;
  RCC_PeriphClkInit.PLL2.PLL2P = 25;
  RCC_PeriphClkInit.PLL2.PLL2Q = 2;
  RCC_PeriphClkInit.PLL2.PLL2R = 2;
  RCC_PeriphClkInit.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
  RCC_PeriphClkInit.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  RCC_PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLL3;
  HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);

  /* ʹ��ADCʱ�� */
  GP2Y_ADC_CLK_ENABLE();
  ADC_Handle.Instance = GP2Y_ADC;
  // ʹ��Boostģʽ
  // ADC_Handle.Init.BoostMode = ENABLE;
  // ADCʱ��1��Ƶ
  ADC_Handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  // ʹ������ת��ģʽ
  ADC_Handle.Init.ContinuousConvMode = ENABLE;
  // ���ݴ�������ݼĴ�����
  ADC_Handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  // �رղ�����ת��ģʽ
  ADC_Handle.Init.DiscontinuousConvMode = DISABLE;
  // ����ת��
  ADC_Handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  // �������
  ADC_Handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  // �رյ͹����Զ��ȴ�
  ADC_Handle.Init.LowPowerAutoWait = DISABLE;
  // �������ʱ������д��
  ADC_Handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  // ��ʹ�ܹ�����ģʽ
  ADC_Handle.Init.OversamplingMode = DISABLE;
  // �ֱ���Ϊ��16bit
  ADC_Handle.Init.Resolution = ADC_RESOLUTION_12B;
  // ��ʹ�ܶ�ͨ��ɨ��
  ADC_Handle.Init.ScanConvMode = DISABLE;
  // ��ʼ�� ADC
  HAL_ADC_Init(&ADC_Handle);

  // ʹ��ͨ��6
  ADC_Config.Channel = GP2Y_ADC_CHANNEL;
  // ת��˳��Ϊ1
  ADC_Config.Rank = ADC_REGULAR_RANK_1;
  // ��������Ϊ64.5������
  ADC_Config.SamplingTime = ADC_SAMPLETIME_64CYCLES_5;
  // ��ʹ�ò������Ĺ���
  ADC_Config.SingleDiff = ADC_SINGLE_ENDED;
  // ����ADCͨ��
  HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);
  // ʹ��ADC1��2
  ADC_Enable(&ADC_Handle);
}

/**
 * @brief  ADC��ʼ������
 * @param  ��
 * @retval ��
 */
void BSP_GP2Y_Init(void)
{

  ADC_GPIO_Mode_Config();
  ADC_Mode_Config();
}
