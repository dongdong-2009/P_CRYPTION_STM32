#ifndef _APP_SFZ_H_
#define _APP_SFZ_H_

//#include "Hjdz.h"

//void sfz_test(void);

#if 0
#define TX_FRAME_PORT	GPIOB
#define TX_FRAME_PIN	GPIO_Pin_6

#define RX_FRAME_PORT	GPIOE
#define RX_FRAME_PIN	GPIO_Pin_7
#define SFZ_UART 1

#define SFZCOM_UART 2

#else
#define TX_FRAME_PORT	GPIOC
#define TX_FRAME_PIN	GPIO_Pin_4

#define RX_FRAME_PORT	GPIOC
#define RX_FRAME_PIN	GPIO_Pin_5
#define SFZ_UART 2
#define SFZCOM_UART 1
#endif


#define SDAT_PORT	GPIOB
#define SDAT_PIN	GPIO_Pin_11

#define SCLK_PORT	GPIOB
#define SCLK_PIN	GPIO_Pin_10

#define TX_FRAME	(TX_FRAME_PORT->IDR&TX_FRAME_PIN) //GPIO_ResetBits(TX_FRAME_PORT,TX_FRAME_PIN)
#define TX_FRAME_H  TX_FRAME_PORT->BSRR = TX_FRAME_PIN
#define RX_FRAME_H  RX_FRAME_PORT->BSRR = RX_FRAME_PIN  //(GPIO_SetBits(GPIOC,GPIO_Pin_1))
#define RX_FRAME_L  RX_FRAME_PORT->BRR = RX_FRAME_PIN  //(GPIO_ResetBits(GPIOC,GPIO_Pin_1))

#define SDAT        (SDAT_PORT->IDR&SDAT_PIN) //    GPIO_ResetBits(SDAT_PORT,SDAT_PIN)
#define SDAT_H      SDAT_PORT->BSRR = SDAT_PIN		//(GPIO_SetBits(GPIOC,GPIO_Pin_1))
#define SDAT_L      SDAT_PORT->BRR = SDAT_PIN 		//(GPIO_ResetBits(GPIOC,GPIO_Pin_1))

#define SCLK        (SCLK_PORT->IDR&SCLK_PIN) //   	    GPIO_ResetBits(SCLK_PORT,SCLK_PIN)
#define SCLK_H      SCLK_PORT->BSRR = SCLK_PIN	// (GPIO_SetBits(GPIOC,GPIO_Pin_1))
#define SCLK_L      SCLK_PORT->BRR = SCLK_PIN	//(GPIO_ResetBits(GPIOC,GPIO_Pin_1))

void sfzIntil(void);

#endif 

