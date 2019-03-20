/**
  ******************************************************************************
  * File Name          : CAN.c
  * Description        : This file provides code for the configuration
  *                      of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 12;
  hcan.Init.Mode = CAN_MODE_LOOPBACK;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_3TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration    
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();
  
    /**CAN GPIO Configuration    
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
void CAN1_Config(void)
{
	CAN_FilterTypeDef CAN_FilterType;
	CAN_FilterType.FilterBank=0;
	CAN_FilterType.FilterIdHigh=0x0000;
	CAN_FilterType.FilterIdLow=0x0000;
	CAN_FilterType.FilterMaskIdHigh=0x0000;
	CAN_FilterType.FilterMaskIdLow=0x0000;
	CAN_FilterType.FilterFIFOAssignment=CAN_RX_FIFO0;
	CAN_FilterType.FilterMode=CAN_FILTERMODE_IDMASK;
	CAN_FilterType.FilterScale=CAN_FILTERSCALE_32BIT;
	CAN_FilterType.FilterActivation=ENABLE;
	CAN_FilterType.SlaveStartFilterBank=14;
	if(HAL_CAN_ConfigFilter(&hcan,&CAN_FilterType)!=HAL_OK)
	{
		Error_Handler();
	}
	if(HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK)
	{
		Error_Handler();
	}
	if(HAL_CAN_Start(&hcan)!=HAL_OK)
	{
		Error_Handler();
	}
}
/***********************************************
�������ܣ�can��������
��ڲ�����
			ide��	0����׼֡
					1����չ֡
			id��	֡ID
			len��	���ݳ���
			data��	����
����ֵ��0���ɹ���1��ʧ��
************************************************/
uint8_t Can_TxMessage(uint8_t ide,uint32_t id,uint8_t len,uint8_t *data)
{
	uint32_t   TxMailbox;
	CAN_TxHeaderTypeDef CAN_TxHeader;
	HAL_StatusTypeDef   HAL_RetVal; 
	uint16_t i=0;
	if(ide == 0)
	{
		CAN_TxHeader.IDE = CAN_ID_STD;	//��׼֡
		CAN_TxHeader.StdId = id;
	}
	else 
	{
		CAN_TxHeader.IDE = CAN_ID_EXT;			//��չ֡
		CAN_TxHeader.ExtId = id;
	}
	CAN_TxHeader.DLC = len;
	CAN_TxHeader.RTR = CAN_RTR_DATA;//����֡,CAN_RTR_REMOTEң��֡
	CAN_TxHeader.TransmitGlobalTime = DISABLE;
	while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0)
	{
		i++;
		if(i>0xfffe)
			return 1;
	}
	HAL_Delay(500);
	HAL_RetVal = HAL_CAN_AddTxMessage(&hcan,&CAN_TxHeader,data,&TxMailbox);
	if(HAL_RetVal != HAL_OK)
		return 1;
	return 0;
}
/*******************************************
�������ܣ������жϻص�����
����ֵ����
********************************************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef CAN_RxHeader;
	HAL_StatusTypeDef HAL_Retval;
	uint8_t Rx_Data[8];
	uint8_t Data_Len=0;
	uint32_t ID=0;
	uint8_t i;
	HAL_Retval = HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&CAN_RxHeader,Rx_Data);
	if(HAL_Retval == HAL_OK)
	{
		Data_Len = CAN_RxHeader.DLC;
		if(CAN_RxHeader.IDE)
			ID = CAN_RxHeader.ExtId;
		else
			ID = CAN_RxHeader.StdId;
		printf("id:%x\r\n",ID);
		printf("Data_Len:%d\r\n",Data_Len);
		for(i=0;i<8;i++)
			printf("Rx_Data[%d]=%x\r\n",i,Rx_Data[i]);	
	}
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
