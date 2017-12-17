#include "hal_uart.h"
#include "interFace_uart.h"

#define UART1_MAX_LEN		100
#define UART2_MAX_LEN		2800
#define UART3_MAX_LEN		100

u8 senddata1[UART1_MAX_LEN];
u8 revtdata1[UART1_MAX_LEN];

u8 senddata2[UART2_MAX_LEN];
u8 revtdata2[UART2_MAX_LEN];

u8 senddata3[UART3_MAX_LEN];
u8 revtdata3[UART3_MAX_LEN];

struct hw_uart_device uart11_device;
struct hw_uart_device uart21_device;
struct hw_uart_device uart31_device;
/******************************************************************* 
函数功能:  回调函数  485片选时所用
入口参数: 
返 回 值: 
相关调用:
备    注: 
修改信息:         
********************************************************************/
static void uart_485_cs(u8 stat)
{
  if(stat)
  {
    ;//GPIOA->BSRR = GPIO_Pin_8;
  }
  else
  {
    ;//GPIOA->BRR = GPIO_Pin_8;
  }
}

/******************************************************************* 
函数功能: 开机调用应用层的注册信息
入口参数: 无
返 回 值: 
相关调用: 应用层初始化串口1设备的实体 
备    注: 应用层无须调用，由dev_uart中初始化调用        
********************************************************************/
bool Uart1_back_app_Init(void)
{  
  //启用GPIOA时钟
  RCC->APB2ENR|=RCC_APB2Periph_GPIOA;
  //推逸输出 PA8
  GPIOA->CRH &= 0xfffffff0;
  GPIOA->CRH |= 0x00000003;
  
  return TRUE;
}


bool Uart2_back_app_Init(void)
{  
  //启用GPIOA时钟
  RCC->APB2ENR|=RCC_APB2Periph_GPIOA;
  //推逸输出 PA8
  GPIOA->CRH &= 0xfffffff0;
  GPIOA->CRH |= 0x00000003;
  
  return TRUE;
}

bool Uart3_back_app_Init(void)
{  

  return TRUE;
}

/******************************************************************* 
函数功能: 串口1 初始
入口参数: 
返 回 值: 
相关调用:
备    注: 
修改信息:         
********************************************************************/
void Uart1_device_Init(void)
{
    uart1_device=&uart11_device;

	/* device interface */
	uart1_device->uart_device_parent.init = Uart1_back_app_Init;
	uart1_device->uart_485_cs=uart_485_cs;

	uart1_device->sendadder=senddata1;
	uart1_device->revtadder=revtdata1;	
	uart1_device->dmasize=UART1_MAX_LEN;
}


/******************************************************************* 
函数功能: 串口2 初始
入口参数: 
返 回 值: 
相关调用:
备    注: 
修改信息:         
********************************************************************/
void Uart2_device_Init(void)
{
       uart2_device=&uart21_device;
	/* device interface */
	uart2_device->uart_device_parent.init = Uart2_back_app_Init;
	uart2_device->uart_485_cs=uart_485_cs;

	uart2_device->sendadder=senddata2;
	uart2_device->revtadder=revtdata2;	
	uart2_device->dmasize=UART2_MAX_LEN;
	  
	
}

/******************************************************************* 
函数功能: 串口3 初始
入口参数: 
返 回 值: 
相关调用:
备    注: 
修改信息:         
********************************************************************/
void Uart3_device_Init(void)
{
        uart3_device=&uart31_device;
	/* device interface */
	uart3_device->uart_device_parent.init = Uart3_back_app_Init;
	uart3_device->uart_485_cs=uart_485_cs;

	uart3_device->sendadder=senddata3;
	uart3_device->revtadder=revtdata3;	
	uart3_device->dmasize=UART3_MAX_LEN;
	  
	
}
