#ifndef __hal_uart_H_
#define __hal_uart_H_

#include "stm32f10x.h"

#include "interFace.h"

typedef enum _UartEvent_
{
   event0=0x01,
   event1=0x02,
   event2=0x04,
   event3=0x08,
   event4=0x10,
   event5=0x20,
   E_uart_tc=0x40,                  //�������
   E_uart_idle=0x80,               //�������
}UartEvent;

struct hw_uart_device
{
	struct hw_device uart_device_parent;
	void  (*uart_485_cs)(u8 stat);    //����Ϊ1  ����Ϊ0
	u8 event;                   //�¼�
	u16 dmasize;                //dma����������󳤶�
	u8 *sendadder;              //���͵�ַ
	u8 *revtadder;              //���յ�ַ
	u16 revtlen;                //���ճ���
};


//���ڲ��ṹָ��,�ⲿ���ñ��붨��ʵ��,�ɽӿ��ļ��г�ʼ����
extern struct hw_uart_device *uart1_device;
extern struct hw_uart_device *uart2_device;
extern struct hw_uart_device *uart3_device;
void Uart_Dma_Clr(u8 com);
//���жϻص���
void Uatr1_Back_IRQHandler();
void Uatr2_Back_IRQHandler();
void Uatr3_Back_IRQHandler();
//����ʼ����   ������   ��8bit,1stop,n��  remap 0=Ĭ��IO  1=ӳ��IO
void UART1_Config_Init(u32 baudrate,u8 remap);
void UART2_Config_Init(u32 baudrate,u8 remap);
void UART3_Config_Init(u32 baudrate,u8 remap);
//���¼���
UartEvent Uart_Get_Event(u8 com);                   //��ȡ�¼�
void      Uart_Clr_Event(u8 com,UartEvent event);   //����¼�
u16       Uart_Get_Len(u8 com);                     //��ȡ��Ч����

//�����ݷ��͡�
extern hw_size_t Uart_Write_Data(u8 com, void* buffer, hw_size_t size);

//ȡ DMA���� ָ��
extern u8 *Uart_Get_Data(u8 com,  hw_size_t *size);

#endif








/*
event  8:  1 IDLE  ΪDMA������ɵ�����̬ �û���ʱ�ɴ������ݰ�
event  7:  1 ���ͽ�����ʶ


*/

