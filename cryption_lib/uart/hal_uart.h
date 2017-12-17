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
   E_uart_tc=0x40,                  //发送完成
   E_uart_idle=0x80,               //接收完成
}UartEvent;

struct hw_uart_device
{
	struct hw_device uart_device_parent;
	void  (*uart_485_cs)(u8 stat);    //发送为1  接收为0
	u8 event;                   //事件
	u16 dmasize;                //dma处量数据最大长度
	u8 *sendadder;              //发送地址
	u8 *revtadder;              //接收地址
	u16 revtlen;                //接收长度
};


//【内部结构指针,外部引用必须定义实体,由接口文件中初始化】
extern struct hw_uart_device *uart1_device;
extern struct hw_uart_device *uart2_device;
extern struct hw_uart_device *uart3_device;
void Uart_Dma_Clr(u8 com);
//【中断回调】
void Uatr1_Back_IRQHandler();
void Uatr2_Back_IRQHandler();
void Uatr3_Back_IRQHandler();
//【初始化】   波特率   （8bit,1stop,n）  remap 0=默认IO  1=映射IO
void UART1_Config_Init(u32 baudrate,u8 remap);
void UART2_Config_Init(u32 baudrate,u8 remap);
void UART3_Config_Init(u32 baudrate,u8 remap);
//【事件】
UartEvent Uart_Get_Event(u8 com);                   //获取事件
void      Uart_Clr_Event(u8 com,UartEvent event);   //清除事件
u16       Uart_Get_Len(u8 com);                     //读取有效长度

//【数据发送】
extern hw_size_t Uart_Write_Data(u8 com, void* buffer, hw_size_t size);

//取 DMA数据 指针
extern u8 *Uart_Get_Data(u8 com,  hw_size_t *size);

#endif








/*
event  8:  1 IDLE  为DMA接收完成到空闲态 用户此时可处理数据包
event  7:  1 发送结束标识


*/

