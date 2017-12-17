#include "stm32f10x.h"
#include "hal_uart.h"
#include "string.h"


#define USART1_DR_Base  0x40013804
#define USART2_DR_Base  0x40004404
#define USART3_DR_Base  0x40004804

static void _uart1_gpio_init(u8 remap);
static void _uart2_gpio_init(u8 remap);
static void _uart3_gpio_init(u8 remap);
static void _uart1_dma_configuration();
static void _uart2_dma_configuration();
static void _uart3_dma_configuration();
static void _uart_setbaudrate(USART_TypeDef* USARTx,uint32_t value);


struct hw_uart_device *uart1_device;
struct hw_uart_device *uart2_device;
struct hw_uart_device *uart3_device;
/******************************************************************* 
函数功能: 
入口参数: 无
返 回 值: 
相关调用:
备    注: 
修改信息:   
********************************************************************/
static void _uart1_gpio_init(u8 remap)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
   
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA   |
  	                 RCC_APB2Periph_USART1  |
                         RCC_APB2Periph_AFIO, ENABLE) ;
                       
  GPIOA->CRH&=0XFFFFF00F; 
  GPIOA->CRH|=0X000008B0;//IO状态设置 10pin_上拉输入  9pin_推挽输出
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    /* Configure USART1 Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure USART1 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  
  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  USART_ClearFlag(USART1, USART_FLAG_TC); /* 清发送外城标志，Transmission Complete flag */

  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
  //USART_ITConfig(USART1, USART_IT_TC, ENABLE);
  //USART_ITConfig(USART1, USART_IT_FE, ENABLE);
}


/******************************************************************* 
函数功能: 
入口参数: 无
返 回 值: 
相关调用:
备    注: 
修改信息:   
********************************************************************/
static void _uart2_gpio_init(u8 remap)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE) ;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE) ;
                       
  GPIOA->CRL&=0XFFFF00FF; 
  GPIOA->CRL|=0X00008B00;//IO状态设置 10pin_上拉输入  9pin_推挽输出

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    /* Configure USART1 Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure USART1 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  USART_ClearFlag(USART2, USART_FLAG_TC); /* 清发送外城标志，Transmission Complete flag */

  USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
}


/******************************************************************* 
函数功能: 
入口参数: 无
返 回 值: 
相关调用:
备    注: 
修改信息:   
********************************************************************/
static void _uart3_gpio_init(u8 remap)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO ,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE) ;
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  
  if(remap)  //映射的USART3 
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD ,ENABLE);
    GPIO_PinRemapConfig(GPIO_FullRemap_USART3,ENABLE); 
    /* Configure USART1 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    /* Configure USART1 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
  }
  else     //正常
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    /* Configure USART1 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* Configure USART1 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  }
  
  
  
  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  USART_ClearFlag(USART3, USART_FLAG_TC); /* 清发送外城标志，Transmission Complete flag */
  
  USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
}

/****************************************************************
*函 数 名：
*功能描述：串口设置波特率
*参    数：9600 或 19200
*反 回 值：
****************************************************************/
static void _uart_setbaudrate(USART_TypeDef* USARTx,uint32_t value)
{
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate =value;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USARTx, &USART_InitStructure);
  USART_Cmd(USARTx, ENABLE);
}


/****************************************************************
*函 数 名：
*功能描述：串口设置波特率
*参    数：发送地址  接收地址
*反 回 值：
****************************************************************/
static void _uart1_dma_configuration()
{
  DMA_InitTypeDef DMA_InitStructure;

  /* DMA1 Channel6 (triggered by USART1 Rx event) Config */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 ,
                        ENABLE);
  
  /* DMA1 Channel5 (triggered by USART1 Rx event) Config */
  DMA_DeInit(DMA1_Channel5);
  DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr =(uint32_t)uart1_device->revtadder;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize =uart1_device->dmasize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  
  DMA_Cmd(DMA1_Channel5, ENABLE);
  
  /* DMA1 Channel4 (triggered by USART1 Tx event) Config */
  DMA_DeInit(DMA1_Channel4);
  DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr =(uint32_t)uart1_device->sendadder;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_Init(DMA1_Channel4, &DMA_InitStructure);
  
  USART_ITConfig(USART1, USART_IT_TC, ENABLE);
  USART_DMACmd(USART1, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);
}


/****************************************************************
*函 数 名：
*功能描述：串口设置波特率
*参    数：发送地址  接收地址
*反 回 值：
****************************************************************/
static void _uart2_dma_configuration()
{
  DMA_InitTypeDef DMA_InitStructure;
  
  /* DMA1 Channel6 (triggered by USART1 Rx event) Config */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 ,
                        ENABLE);
  
  /* DMA1 Channel5 (triggered by USART1 Rx event) Config */
  DMA_DeInit(DMA1_Channel6);
  DMA_InitStructure.DMA_PeripheralBaseAddr = USART2_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr =(uint32_t)uart2_device->revtadder;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize =uart2_device->dmasize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel6, &DMA_InitStructure);
  
  DMA_Cmd(DMA1_Channel6, ENABLE);
  
  
  // DMA1 Channel7 (triggered by USART2 Tx event) Config
  DMA_DeInit(DMA1_Channel7);
  DMA_InitStructure.DMA_PeripheralBaseAddr = USART2_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart2_device->sendadder;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_Init(DMA1_Channel7, &DMA_InitStructure);
  
  USART_ITConfig(USART2, USART_IT_TC, ENABLE);
  USART_DMACmd(USART2, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);
  
}

/****************************************************************
*函 数 名：
*功能描述：串口设置波特率
*参    数：发送地址  接收地址
*反 回 值：
****************************************************************/
static void _uart3_dma_configuration()
{
  DMA_InitTypeDef DMA_InitStructure;
  
  /* DMA1 Channel6 (triggered by USART1 Rx event) Config */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 ,
                        ENABLE);
  
  /* DMA1 Channel5 (triggered by USART1 Rx event) Config */
  DMA_DeInit(DMA1_Channel3);
  DMA_InitStructure.DMA_PeripheralBaseAddr = USART3_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr =(uint32_t)uart3_device->revtadder;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize =uart3_device->dmasize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);
  
  DMA_Cmd(DMA1_Channel3, ENABLE);
  
  
  // DMA1 Channel7 (triggered by USART2 Tx event) Config
  DMA_DeInit(DMA1_Channel2);
  DMA_InitStructure.DMA_PeripheralBaseAddr = USART3_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart3_device->sendadder;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_Init(DMA1_Channel2, &DMA_InitStructure);
  
  USART_ITConfig(USART3, USART_IT_TC, ENABLE);
  USART_DMACmd(USART3, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);
  
}


UartEvent Uart_Get_Event(u8 com)
{
  UartEvent e;
  if(com==1)
  {
    if(!DMA1_Channel5->CNDTR) Uart_Dma_Clr(com);
    e= (UartEvent)uart1_device->event;
  }else if(com==2)
  {
    if(!DMA1_Channel6->CNDTR) Uart_Dma_Clr(com);
    e= (UartEvent)uart2_device->event;
  }else if(com==3)
  {
    if(!DMA1_Channel3->CNDTR) Uart_Dma_Clr(com);
    e= (UartEvent)uart3_device->event;
  }
  return e;
}

void Uart_Clr_Event(u8 com,UartEvent event)
{
  if(com==1)
  {
    uart1_device->event&=~event;
  }else if(com==2)
  {
    uart2_device->event&=~event;
  }else if(com==3)
  {
    uart3_device->event&=~event;
  }
}

void Uart_Set_Event(u8 com,UartEvent event)
{
  if(com==1)
  {
    uart1_device->event&=~event;
    uart1_device->event|=event;
  }else if(com==2)
  {
    uart2_device->event&=~event;
    uart2_device->event|=event;
  }else if(com==3)
  {
    uart3_device->event&=~event;
    uart3_device->event|=event;
  }
}

void Uart_Dma_Clr(u8 com)
{
  if(com==1)
  {
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA1_Channel4->CNDTR=0;
    DMA_Cmd(DMA1_Channel5, DISABLE);
    DMA1_Channel5->CNDTR=uart1_device->dmasize;
    DMA_Cmd(DMA1_Channel5, ENABLE);
  }else if(com==2)
  {
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA1_Channel7->CNDTR=0;
    DMA_Cmd(DMA1_Channel6, DISABLE);
    DMA1_Channel6->CNDTR=uart2_device->dmasize;
    DMA_Cmd(DMA1_Channel6, ENABLE);
  }else if(com==3)
  {
    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA1_Channel2->CNDTR=0;
    DMA_Cmd(DMA1_Channel3, DISABLE);
    DMA1_Channel3->CNDTR=uart3_device->dmasize;
    DMA_Cmd(DMA1_Channel3, ENABLE);
  }
}

//返回串口数据长度
u16 Uart_Get_Len(u8 com)
{
  u16 value=0;
  if(com==1)       value=uart1_device->revtlen;
  else if (com==2) value=uart2_device->revtlen;
  else if (com==3) value=uart3_device->revtlen;
  return value;
}



bool  init_uart(void)
{
  return TRUE;
}


bool   open_uart(u16 oflag)
{
   return TRUE;
}

hw_size_t Uart_Write_Data(u8 com, void* buffer, hw_size_t size)
{
  if(!size) return 0;
  if(com==1)
  { //DMA发送
    while (DMA_GetCurrDataCounter(DMA1_Channel4));
    if(buffer) memcpy(uart1_device->sendadder, buffer,uart1_device->dmasize>size?size:uart1_device->dmasize);
    if( uart1_device->uart_485_cs)  uart1_device->uart_485_cs(1);
    //DMA发送数据-要先关 设置发送长度 开启DMA
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA1_Channel4->CNDTR = size;
    //DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, DISABLE);
    DMA_Cmd(DMA1_Channel4, ENABLE);  
  }else if(com==2)
  {
    while (DMA_GetCurrDataCounter(DMA1_Channel7));
    if(buffer) memcpy(uart2_device->sendadder, buffer,uart2_device->dmasize>size?size:uart2_device->dmasize);
    if( uart2_device->uart_485_cs)  uart2_device->uart_485_cs(1);
    //DMA发送数据-要先关 设置发送长度 开启DMA
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA1_Channel7->CNDTR = size;
    DMA_Cmd(DMA1_Channel7, ENABLE);  
  }else if(com==3)
  {
    while (DMA_GetCurrDataCounter(DMA1_Channel2));
    if(buffer) memcpy(uart3_device->sendadder, buffer,uart3_device->dmasize>size?size:uart3_device->dmasize);
    if( uart3_device->uart_485_cs)  uart3_device->uart_485_cs(1);
    //DMA发送数据-要先关 设置发送长度 开启DMA
    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA1_Channel2->CNDTR = size;
    DMA_Cmd(DMA1_Channel2, ENABLE);  
  }
  
  return size;
}

//取 DMA数据 指针
u8 *Uart_Get_Data(u8 com,  hw_size_t *size)
{
u8 *ptr;
	*size = Uart_Get_Len(com);	
	if(com==1)       ptr=uart1_device->revtadder;
	else if (com==2) ptr=uart2_device->revtadder;
	else if (com==3) ptr=uart3_device->revtadder;
	return ptr;
}


void Uatr1_Back_IRQHandler()
{
  u8 tem;
//  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//  {  
//    uart1_device.revtadder[uart1_device.revtlen++]=USART_ReceiveData(USART1) & 0xFF; 
//    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//  }
  if(USART_GetITStatus(USART1,USART_IT_IDLE)!= RESET)
  {
    tem=USART1->SR;//先读SR，然后读DR才能清除
    tem=USART1->DR;
    tem=tem;
    Uart_Set_Event(1,E_uart_idle);
    uart1_device->revtlen=uart1_device->dmasize-DMA1_Channel5->CNDTR;
    USART_ClearITPendingBit(USART1, USART_IT_IDLE);
  } 
  
  if(USART_GetITStatus(USART1,USART_IT_TC)!= RESET)
  {
    USART_ClearITPendingBit(USART1, USART_IT_TC);
    if( uart1_device->uart_485_cs)  uart1_device->uart_485_cs(0);

    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA1_Channel4->CNDTR=0;
    Uart_Set_Event(1,E_uart_tc);
  } 
}

void Uatr2_Back_IRQHandler()
{
  u8 tem;
//  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
//  {  
//    uart2_device->revtadder[uart2_device->revtlen++]=USART_ReceiveData(USART2) & 0xFF; 
//    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//  }
  if(USART_GetITStatus(USART2,USART_IT_IDLE)!= RESET)
  {
    tem=USART2->SR;//先读SR，然后读DR才能清除
    tem=USART2->DR;
    tem=tem;
    Uart_Set_Event(2,E_uart_idle);
    uart2_device->revtlen=uart2_device->dmasize-DMA1_Channel6->CNDTR;
    USART_ClearITPendingBit(USART2, USART_IT_IDLE);
  } 
  
  if(USART_GetITStatus(USART2,USART_IT_TC)!= RESET)
  {
    if( uart2_device->uart_485_cs)  uart2_device->uart_485_cs(0);
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA1_Channel7->CNDTR=0;
    Uart_Set_Event(2,E_uart_tc);
    USART_ClearITPendingBit(USART2, USART_IT_TC);
  } 
}

void Uatr3_Back_IRQHandler()
{
  u8 tem;
  if(USART_GetITStatus(USART3,USART_IT_IDLE)!= RESET)
  {
    tem=USART3->SR;//先读SR，然后读DR才能清除
    tem=USART3->DR;
    tem=tem;
    Uart_Set_Event(3,E_uart_idle);
    uart3_device->revtlen=uart3_device->dmasize-DMA1_Channel3->CNDTR;
    USART_ClearITPendingBit(USART3, USART_IT_IDLE);
  } 
  
  if(USART_GetITStatus(USART3,USART_IT_TC)!= RESET)
  {
    if( uart3_device->uart_485_cs)  uart3_device->uart_485_cs(0);
    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA1_Channel2->CNDTR=0;
    Uart_Set_Event(3,E_uart_tc);
    USART_ClearITPendingBit(USART3, USART_IT_TC);
  } 
}

/*******************************************************************************
* Description    : 串口配置初始化
* Input          : 波特率
* Output         : 
* Return         :
*******************************************************************************/
void UART1_Config_Init(u32 baudrate,u8 remap)
{
  _uart1_gpio_init(remap);  
  //Uart1_back_app_Init();
  uart1_device->uart_device_parent.init();
  _uart_setbaudrate(USART1,baudrate);
  _uart1_dma_configuration();
 // Uart_Write_Data(1,"The is a uart1 DMA...!\r\n",24);
}



/*******************************************************************************
* Description    : 串口配置初始化
* Input          : 波特率
* Output         : 
* Return         :
*******************************************************************************/
void UART2_Config_Init(u32 baudrate,u8 remap)
{
  _uart2_gpio_init(remap);
  // Uart2_back_app_Init();
  uart2_device->uart_device_parent.init();
  _uart_setbaudrate(USART2,baudrate);
  _uart2_dma_configuration();
}


/*******************************************************************************
* Description    : 串口配置初始化
* Input          : 波特率
* Output         : 
* Return         :
*******************************************************************************/
void UART3_Config_Init(u32 baudrate,u8 remap)
{
  _uart3_gpio_init(remap);
  uart3_device->uart_device_parent.init();
  _uart_setbaudrate(USART3,baudrate);
  _uart3_dma_configuration();
 //
  //Uart_Write_Data(3,"The is a uart3 DMA...!\r\n",24);
}












