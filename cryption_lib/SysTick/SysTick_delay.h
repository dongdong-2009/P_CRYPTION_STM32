#ifndef __SysTick_DELAY_H
#define __SysTick_DELAY_H 			   
//#include <stm32f10x_lib.h>

//ʹ��SysTick����ͨ����ģʽ���ӳٽ��й���
//����delay_us,delay_ms

#include "stm32f10x.h"

//#ifndef u8
//typedef unsigned char	u8;
//#endif
//
//#ifndef u16
//typedef unsigned short	u16;
//#endif
//
//#ifndef u32
//typedef unsigned int	u32;
//#endif

#define SysTick_Counter_Disable        ((u32)0xFFFFFFFE)
#define SysTick_Counter_Enable         ((u32)0x00000001)
#define SysTick_Counter_Clear          ((u32)0x00000000)
#define CTRL_TICKINT_Set      ((u32)0x00000002)


void Delay_Init(u8 SYSCLK);				// ��ʱ��ʼ��	ϵͳʱ��Ϊ72MHz
void Delay_ms(u16 nms);
void Delay_us(u32 nus);

#endif
