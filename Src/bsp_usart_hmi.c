/**
 * @file bsp_usart_hmi.c
 * @author July (Email: JulyCub@163.com)
 * @brief The board support package for USART HMI.
 * @version 1.0
 * @date 2022.07.06
 * @update 2023.07.31
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "bsp_usart_hmi.h"
#include "string.h"

/* HMIͨ�� */
uint8_t    hmi_chnl;
/* �ؼ�ID */
uint8_t 	hmi_id;		
/* ҳ���� */
uint8_t 	hmi_page;	
uint16_t 	hmi_point; 
uint16_t 	hmi_point_counts;

/**
 * @brief ��ͨ������͸������
 * @param id �ؼ�ID
 * @param chnl ͨ����
 * @param points ���ߵ㼯
 * @param len �㼯����
 * @param page ҳ����
 */
void BSP_HMI_SendCurveSingle(uint8_t id, uint8_t chnl, uint8_t points[], uint16_t len, uint8_t page)
{
	uint16_t	i = len - 1;

	/* �л�ҳ�� */
	__switchpage(page);
	/* ��㷢������ */
	__sendpoints(id, chnl, len);
	while(i >= 0){
		bsprif1("\"%d\"", points[i--]);
		if(i < len - 1){
			bsprif1("\",\"");
		}
	}
}
