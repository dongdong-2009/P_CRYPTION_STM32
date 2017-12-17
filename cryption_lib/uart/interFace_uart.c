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
��������:  �ص�����  485Ƭѡʱ����
��ڲ���: 
�� �� ֵ: 
��ص���:
��    ע: 
�޸���Ϣ:         
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
��������: ��������Ӧ�ò��ע����Ϣ
��ڲ���: ��
�� �� ֵ: 
��ص���: Ӧ�ò��ʼ������1�豸��ʵ�� 
��    ע: Ӧ�ò�������ã���dev_uart�г�ʼ������        
********************************************************************/
bool Uart1_back_app_Init(void)
{  
  //����GPIOAʱ��
  RCC->APB2ENR|=RCC_APB2Periph_GPIOA;
  //������� PA8
  GPIOA->CRH &= 0xfffffff0;
  GPIOA->CRH |= 0x00000003;
  
  return TRUE;
}


bool Uart2_back_app_Init(void)
{  
  //����GPIOAʱ��
  RCC->APB2ENR|=RCC_APB2Periph_GPIOA;
  //������� PA8
  GPIOA->CRH &= 0xfffffff0;
  GPIOA->CRH |= 0x00000003;
  
  return TRUE;
}

bool Uart3_back_app_Init(void)
{  

  return TRUE;
}

/******************************************************************* 
��������: ����1 ��ʼ
��ڲ���: 
�� �� ֵ: 
��ص���:
��    ע: 
�޸���Ϣ:         
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
��������: ����2 ��ʼ
��ڲ���: 
�� �� ֵ: 
��ص���:
��    ע: 
�޸���Ϣ:         
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
��������: ����3 ��ʼ
��ڲ���: 
�� �� ֵ: 
��ص���:
��    ע: 
�޸���Ϣ:         
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
