/*************************************************
  Copyright (C),   
  File name:       App_sfz.c
  Author:          
  Version:         V1.0 
  Date: 		   2016-10-19
  Description:    sfz阅读DEMO
  Function List:  
    			   
  History:         				  
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/

#include "App_sfzCOM.h"
#include "stm32f10x.h"
#include "hal_uart.h"
#include "usbhid.h"


enum {
	SFZ_RESET=0,
	SFZ_FIND_CARD,
	SFZ_READ_WRITE,
};





u8 const sam_cmd[3][11]={{10,0xAA,0xAA,0xAA,0x96,0x69,0x00,0x03,0x20,0x01,0x22},
					   		{10,0xAA,0xAA,0xAA,0x96,0x69,0x00,0x03,0x20,0x02,0x21},
					   		{10,0xAA,0xAA,0xAA,0x96,0x69,0x00,0x03,0x30,0x01,0x32},
							};
static u8 sfz_communication(u8 *src,u8 dataType,u8 *dst,u32 *dstLen);
static void sfzdelay(void);
static void sfzSDAT_dir(u8 dir);
static void sfzSCLK_dir(u8 dir);
static u16 sfz_reardSam(u8 *dst);
void sfz_writeSam(u8 *src,u32 len);
static void sfz_delay(u16 delayTime);
static void sfzdelayL(void);
#define TWI_NOP sfzdelay()
#define TWI_NOPL sfzdelayL()

#if 0
enum {
	COM_OK = 0,
	ERROR_STX	 = 0x10,
	ERROR_ETX	 = 0x11,
	ERROR_CLA = 0x12,
	ERROR_INS = 0x13,
	ERROR_LEN = 0x14,
	ERROR_PACK_NUMBER = 0x15,
	ERROR_PACK_LEN = 0x16,
	ERROR_LRC = 0x17,
	ERROR_SAM = 0x20,	// SAM交互出错
};
#else
enum {
	COM_OK = 0x9000,
	ERROR_STX	 = 0x10,
	ERROR_ETX	 = 0x11,
	ERROR_CLA = 0x12,
	ERROR_INS = 0x13,
	ERROR_LEN = 0x14,
	ERROR_PACK_NUMBER = 0x15,
	ERROR_PACK_LEN = 0x6A18,
	ERROR_LRC = 0x17,
	ERROR_SAM = 0x6A20,	// SAM交互出错
	ERROR_MAX_RETRY = 0x6A86,	
	ERROR_REMAIN_TIMES = 0x63C0,
};

#endif

enum {
	COM_STX	 = 0x02,
	COM_ETX	 = 0x03,
	COM_CLA = 0x00,
	COM_INS_1 = 0x01,		// 传输指令
	COM_INS_2 = 0x02, 		// 保存file信息
	COM_INS_3 = 0x03,		// 执行保存的file信息
	COM_INS_4 = 0x04,		// 特殊透传，接收到此指令并执行后直接使用file信息
};

enum {
	COM_C1	 = 0xC1,	//认证指令
	COM_C2	 = 0xC2,	//解密指令
	COM_C3	 = 0xC3, //取数据
};

enum {
	SAM_ERROR = 0x00,
	SAM_UART = 0x01,
	SAM_I2C = 0x02,
};

#define MAX_PACK_NUMBER	35
#define MAC_DATA_LEN		128
#define MAC_RETURN_LEN		2800
#if 0
typedef struct	 
{
	u8 cla; 
	u8 ins; 
	u8 packNumber;
	u8 packSerial;
	u8 packBuf[MAX_PACK_NUMBER][MAC_DATA_LEN];
	u8 returnBufLen;
	u8 returnBuf[MAC_RETURN_LEN];
}sfz_com_Def;
#else
typedef struct	 
{
	u8 cmd; 
	u8 p1;
	u8 p2;
	u8 len;
	u8 packNumber;
	u8 packSerial;
	u8 packBuf[MAX_PACK_NUMBER][MAC_DATA_LEN];
	u8 sw1;
	u8 sw2;
	u8 returnBufLen;
	u8 returnBuf[MAC_RETURN_LEN];
}sfz_com_Def;

#endif

sfz_com_Def sfz_buf;

#define SFZ_DELAY_MAX_COUNT		5
#define SFZ_DELAY_TIME				60000			// 60 ms
#define SFZ_DELAY_RX_FRAME_H		58000			// 58 ms
typedef struct	 
{
	u8 flag;		// 0 不需要重新发送   1 需要重新发放 
	u8 count;		// 发送的次数 
	u32 delayTime;	// 延时时间 u 为单位
}sfz_deleySend_Def;
sfz_deleySend_Def sfz_deleySend;


static u8 sfz_LRC(u8 *data,u32 len);
static void sfz_sendERRORPackage(u8 *scr,u32 scrLen,u8 result,u8 *dst,u32 *dstLen);
static void sfz_sendPackage(u8 *scr,u32 scrLen,u8 result,u8 returnType,u8 *dst,u32 *dstLen);
static void sfz_sendTRUE(u8 *src,u32 len,u8 returnType);
static void sfz_sendERROR(u8 state);
static u8 sfz_pars1(u8 *rcv,u32 len);


static u8 sfz_againSAM(u8 *src,u8 *dst,u32 *dstLen);

static u8 sfz_sendSAM[160];
static u8 saveFile[3096]={0x06,0x2f,0x14,0x0b,0x32,0xa9,0x3d,0x14,0xa5,0x62,0xef,0x9a,0x90,0x00,0x00,0x03,0x90,0x00,0x00};

u8 tmp1[]={0x0A,0xAA,0xAA,0xAA,0x96,0x69,0x00,0x03,0x20,0x01,0x22,0x0E,0x50,0x00,0x00,0x00,0x00,0xD1,0x03,0x86,0x07,0x00,0x80,0x90,0x00,0x00,0x0A,0xAA,0xAA,0xAA,0x96,0x69,0x00,0x03,0x20,0x02,0x21,0x02,0x08,0x00,0x0A,0xAA,0xAA,0xAA,0x96,0x69,0x00,0x03,0x30,0x01,0x32,0x03,0x90,0x00,0x00,0x23,0x00,0x01,0x41,0x48,0x01,0x07,0x01,0x17,0x00,0x00,0x00,0x00,0x00,0x09,0x23,0x6e,0x88,0xa8,0x69,0xc8,0x33,0x2a,0x93,0x80,0x3b,0x92,0x8f,0xf2,0xdb,0xda,0xe8,0xcd,0x90,0x00,0x00};
u8 tmp2[3] = {0x00,0x88,0x00};	// 0f 00 88 00 52 0a f0 00 8f 2f 56 69 68 92 a8 41 外部认证

u8 tmp3[] ={0x0b,0x32,0xa9,0x3d,0x14,0xa5,0x62,0xef,0x9a,0x90,0x00,0x00,0x0b,0x12,0x41,0x69,0x00,0xfd,0x47,0x74,0x35,0x90,0x00,0x00};
u8 tmp4[3] = {0x00,0x82,0x00};	// 0f 00 82 00 52 0a f0 01	9b da 75 7a ca f9 be fc 内部认证

u8 tmp5[] ={0x0b,0x32,0xa9,0x3d,0x14,0xa5,0x62,0xef,0x9a,0x90,0x00,0x00,0x03,0x90,0x00,0x00};

u8 tmp6[] ={0x0b,0x32,0xa9,0x3d,0x14,0xa5,0x62,0xef,0x9a,0x90,0x00,0x00};


u8  sfz_plain_text[2048] = {0x00};
u32    sfz_plain_text_len =0;
u32	remain_len=0;	
u32 ttt;
u8 ti = 100;



void sfz_send_to_hid(u8 *sendBuf,u32 sendLen)
{
	u32 ptr=0;
	u8 send_times=0;
	u8 tmp_buffer[MAC_RETURN_LEN]={0x00};

	


	tmp_buffer[ptr++] = sfz_buf.sw1;
       tmp_buffer[ptr++] = sfz_buf.sw2;
	tmp_buffer[ptr++] = sendLen;
	memcpy(&tmp_buffer[ptr],sendBuf,sendLen);
	ptr+=sendLen;

	if(ptr>0xff)
	{
		send_times = ptr/0xff +1;
	   	for(u8 i=0;i<send_times;i++)
	   	{
			USB_hid_send(tmp_buffer+i*0xff,0xff);	
	   	}
	}
	else
	{
		USB_hid_send(tmp_buffer,0xff);	
	}

}


u16 sfz_send_to_sam(u8* buffer, u32 buffer_len ,u8 *rcv,u32* rcv_len)
{


	u8 tmp[2048]={0x00};
	u32 len =0;
	u8 packLen = 0;
	u32 ptr =0;
	u8 dataType;
	u8 iret=0;
	u8 rcvState = 0;
	u32 dataLen=0;
	

	// 0、初始化
	RX_FRAME_L;

		//拼包	
		for (u8 i = 0;i<sfz_buf.packNumber;i++)
		{

			packLen = buffer[ptr];				// 包长度
			if (packLen > MAC_DATA_LEN-1)
			{
				sfz_buf.returnBufLen = 1;
				sfz_buf.returnBuf[0] = i;
				return ERROR_PACK_LEN;
			}
			memcpy(sfz_buf.packBuf[i],&buffer[ptr],packLen+1);
			ptr = ptr+packLen+1;
			if (ptr > buffer_len)
			{	
				sfz_buf.returnBufLen = 1;
				sfz_buf.returnBuf[0] = i;
				return ERROR_PACK_LEN;
			}
		}


		if(sfz_buf.cmd==COM_C1&&sfz_buf.p1 ==0x02)
		{	// 在 透传指令后增加 ram中的file信息

			// 1 已经存储的交互包
			u8  i = sfz_buf.packNumber;
			// 2 取出ram 中的 file信息
			ptr = 0;
			dataLen = saveFile[ptr]*256 + saveFile[ptr+1];
			ptr += 2;
			sfz_buf.packNumber += saveFile[ptr++];	// 总包数
			if (sfz_buf.packNumber > MAX_PACK_NUMBER)
				return ERROR_PACK_NUMBER;
		    // 3 在透传数据包后增加ram  中的 file信息
			for (;i<sfz_buf.packNumber;i++)
			{
				packLen = saveFile[ptr];				// 包长度
				if (packLen > MAC_DATA_LEN-1)
				{
					sfz_buf.returnBufLen = 1;
					sfz_buf.returnBuf[0] = i;
					return ERROR_PACK_LEN;
				}
				memcpy(sfz_buf.packBuf[i],&saveFile[ptr],packLen+1);
				ptr = ptr+packLen+1;
				if (ptr > dataLen+2)
				{	
					sfz_buf.returnBufLen = 1;
					sfz_buf.returnBuf[0] = i;
					return ERROR_PACK_LEN;
				}
			}
		}



		
		//跟SAM 通信
	      	Uart_Clr_Event(SFZ_UART,E_uart_idle);	// 同时清理SAM串口上返回的数据
	      	Uart_Dma_Clr(SFZ_UART);
			

		//******************************************************	
		for (sfz_buf.packSerial = 0;sfz_buf.packSerial<sfz_buf.packNumber;)//sfz_buf.packSerial++)
		{	
			u8 i = sfz_buf.packSerial;
			dataType = 0;
			if ((sfz_buf.packBuf[i][0] == 0x0a)&&(sfz_buf.packBuf[i][1] == 0xaa))
			{
				if (memcmp(&sfz_buf.packBuf[i][0],(u8 *)sam_cmd[0],8) == 0)
					dataType = 0;	// 串口命令
				else 	
					dataType = 1;	// 是I2C命令直接执行
			}
			else
				dataType = 1;	// 是I2C命令直接执行

			rcvState = sfz_communication(&sfz_buf.packBuf[i][0],dataType,tmp,&len);
				if(sfz_buf.packNumber==0x15 && sfz_buf.packSerial==0)
				{
					sfz_buf.packNumber=0x15;
				}

			if (rcvState == SAM_ERROR)
			{
				return ERROR_SAM;
			}
			else if (rcvState == SAM_I2C)
			{

				memcpy(rcv,tmp,len);
				*rcv_len = len;
				return COM_OK;
			}
			else if (rcvState == SAM_UART)
			{
				if ((tmp[7] != 0x00)||(tmp[8] != 0x00)||((tmp[9] != 0x9f)&&(tmp[9] != 0x90)))
				{
					memcpy(rcv,tmp,len);
					*rcv_len = len;
					return COM_OK;
				}
				else if (sfz_buf.packSerial >= sfz_buf.packNumber)
				{
					memcpy(rcv,tmp,len);
					*rcv_len = len;
					return COM_OK;
				}
			}
		}

}







/********************************************
输入:rcv UART过来的数据，len长度
返回:true  数据正确，其它错误类型
********************************************/
static u8 sfz_pars1(u8 *rcv,u32 len)
{
	u32 ptr = 0;
	u32 index =0;
	u16 rcv_state =0;
	u8 packLen = 0;
	u32 dataLen=0;

	

	unsigned char send_buffer[2048]={0x00};
	unsigned char receive_buffer[2048]={0x00};

	if(sfz_deleySend.flag !=1 )					
		sfz_deleySend.count = 0;


	sfz_buf.cmd= rcv[ptr++];
	sfz_buf.p1 =rcv[ptr++];
	sfz_buf.p2 =rcv[ptr++];
	sfz_buf.len=rcv[ptr++];
	
	switch(sfz_buf.cmd)
	{


		case COM_C1:
			{
				if(sfz_buf.p1== 0x00)
				{

					sfz_deleySend.flag = 0;				//重新开始读卡流程重试次数清0
					sfz_deleySend.count =0;
					sfz_buf.packNumber = 0x07;	// 总包数
					//获得数据
					memset(tmp1+56,0x00,0x20);
					memcpy(tmp1+56,&rcv[ptr],0x20);
					memcpy(send_buffer,tmp1,sizeof(tmp1));
					rcv_state = sfz_send_to_sam(send_buffer,sizeof(tmp1),receive_buffer,&dataLen);
					if(COM_OK==rcv_state)
					{
						if ((memcmp(receive_buffer,tmp2,3) == 0))
						{
							sfz_buf.sw1 = rcv_state>>8; 
							sfz_buf.sw2 = rcv_state&0x00ff; 
							sfz_buf.returnBufLen = 0x08;
							memcpy(sfz_buf.returnBuf,receive_buffer+7,sfz_buf.returnBufLen);
						}
	
					}
					else
					{
						sfz_buf.sw1 = rcv_state>>8; 
						sfz_buf.sw2 = rcv_state&0x00ff; 
						sfz_buf.returnBufLen =0;
					
					}
				}
				else if(sfz_buf.p1== 0x01)
				{
					sfz_buf.packNumber = 02;							// 总包数
					memcpy(send_buffer,tmp3,24);
					memcpy(send_buffer+1,&rcv[ptr],0x08);				//外部认证密文
					memcpy(send_buffer+13,&rcv[ptr+8],0x08);			//随机数

					rcv_state = sfz_send_to_sam(send_buffer,sizeof(tmp1),receive_buffer,&dataLen);
					if(COM_OK==rcv_state&&(memcmp(receive_buffer,tmp4,3) == 0))
					{
							sfz_buf.sw1 = rcv_state>>8; 
							sfz_buf.sw2 = rcv_state&0x00ff; 
							sfz_buf.returnBufLen = 0x08;
							memcpy(sfz_buf.returnBuf,receive_buffer+7,sfz_buf.returnBufLen);
							
							sfz_deleySend.flag = 0;				//取得内部认证密文成功，重试次数清0
							sfz_deleySend.count =0;
				
	
					}
					else//外部验证失败重发随机数
					{

						sfz_deleySend.flag = 1;
						sfz_deleySend.count ++;

						if(sfz_deleySend.count>SFZ_DELAY_MAX_COUNT-1)
						{
								sfz_buf.sw1 = ERROR_MAX_RETRY>>8; 
								sfz_buf.sw2 = ERROR_MAX_RETRY&0x00ff; 
								sfz_buf.returnBufLen = 0x00;
								sfz_deleySend.flag = 0;
								sfz_deleySend.count =0;
								break;
						}


						sfz_buf.packNumber = 0x07;	// 总包数
						//获得数据
						memcpy(send_buffer,tmp1,sizeof(tmp1));
						rcv_state = sfz_send_to_sam(send_buffer,sizeof(tmp1),receive_buffer,&dataLen);
						if(COM_OK==rcv_state)
						{
							if ((memcmp(receive_buffer,tmp2,3) == 0))
							{

								sfz_buf.sw1 = ERROR_REMAIN_TIMES>>8; 
								sfz_buf.sw2 =( ERROR_REMAIN_TIMES&0x00ff)|(SFZ_DELAY_MAX_COUNT-sfz_deleySend.count); 
								sfz_buf.returnBufLen = 0x08;
								memcpy(sfz_buf.returnBuf,receive_buffer+7,sfz_buf.returnBufLen);
								
							}
		
						}
						else
						{
							sfz_buf.sw1 = rcv_state>>8; 
							sfz_buf.sw2 = rcv_state&0x00ff; 
							sfz_buf.returnBufLen =0;
						
						}
					
					}
		
				}
				else if(sfz_buf.p1== 0x02)
				{
						sfz_buf.packNumber =0x01;	// 总包数
						memcpy(send_buffer,tmp6,sizeof(tmp6));
						memcpy(send_buffer+1,&rcv[ptr],sfz_buf.len);

					rcv_state = sfz_send_to_sam(send_buffer,send_buffer[0]+1,sfz_plain_text,&sfz_plain_text_len);
					if(COM_OK==rcv_state &&(sfz_plain_text[9] ==0x90))
					{	


							sfz_buf.sw1 = rcv_state>>8; 
							sfz_buf.sw2 = rcv_state&0x00ff; 
							sfz_buf.returnBufLen = 0x00;	
							remain_len = sfz_plain_text_len;
					}
					else		//外部验证失败重发随机数
					{
						sfz_deleySend.flag = 1;
						sfz_deleySend.count ++;

						if(sfz_deleySend.count>SFZ_DELAY_MAX_COUNT-1)
						{
								sfz_buf.sw1 = ERROR_MAX_RETRY>>8; 
								sfz_buf.sw2 = ERROR_MAX_RETRY&0x00ff; 
								sfz_buf.returnBufLen = 0x00;
								sfz_deleySend.flag = 0;
								sfz_deleySend.count =0;
								break;
						}

						sfz_buf.p1 = 01;//修改为了不把save的数据拷到tmp1的后面	
						sfz_buf.packNumber = 0x07;	// 总包数
						//获得数据
						memcpy(send_buffer,tmp1,sizeof(tmp1));
						rcv_state = sfz_send_to_sam(send_buffer,sizeof(tmp1),receive_buffer,&dataLen);
						if(COM_OK==rcv_state)
						{
							if ((memcmp(receive_buffer,tmp2,3) == 0))
							{

								sfz_buf.sw1 = ERROR_REMAIN_TIMES>>8; 
								sfz_buf.sw2 =( ERROR_REMAIN_TIMES&0x00ff)|(SFZ_DELAY_MAX_COUNT-sfz_deleySend.count); 
								sfz_buf.returnBufLen = 0x08;
								memcpy(sfz_buf.returnBuf,receive_buffer+7,sfz_buf.returnBufLen);

							}
		
						}
						else
						{
							sfz_buf.sw1 = rcv_state>>8; 
							sfz_buf.sw2 = rcv_state&0x00ff; 
							sfz_buf.returnBufLen =0;
						
						}
					
					}						
		
				}


			}
			break;

		case COM_C2:
			{
					
				if(sfz_buf.p1== 0x00)
				{
					memcpy( saveFile+3+0x0B+1+0x03+1+sfz_buf.p2*251,&rcv[ptr],sfz_buf.len);
					sfz_buf.sw1 = COM_OK>>8; 
					sfz_buf.sw2 = COM_OK&0x00ff; 
					sfz_buf.returnBufLen =0;

				}
				else if(sfz_buf.p1== 0x80)
				{
					memcpy( saveFile+3+0x0B+1+0x03+1+sfz_buf.p2*251,&rcv[ptr],sfz_buf.len);
					if(saveFile[240]==0x04)
						saveFile[240]=0x00;
					sfz_buf.packNumber = 0x07;	// 总包数
					//获得数据
					memcpy(send_buffer,tmp1,sizeof(tmp1));
					rcv_state = sfz_send_to_sam(send_buffer,sizeof(tmp1),receive_buffer,&dataLen);
					if(COM_OK==rcv_state)
					{
						if ((memcmp(receive_buffer,tmp2,3) == 0))
						{
							sfz_buf.sw1 = rcv_state>>8; 
							sfz_buf.sw2 = rcv_state&0x00ff; 
							sfz_buf.returnBufLen = 0x08;
							memcpy(sfz_buf.returnBuf,receive_buffer+7,sfz_buf.returnBufLen);
						}
	
					}
					else
					{
						sfz_buf.sw1 = rcv_state>>8; 
						sfz_buf.sw2 = rcv_state&0x00ff; 
						sfz_buf.returnBufLen =0;
					
					}

				}
				
			}
			break;

		case COM_C3:
			{
				if(sfz_buf.p2<sfz_plain_text_len/252)
				{
					memcpy(sfz_buf.returnBuf,sfz_plain_text+sfz_buf.p2*252,252);
					sfz_buf.sw1 =0x61 ; 
					sfz_buf.sw2 = (sfz_plain_text_len/252+1)-(sfz_buf.p2+1); 
					remain_len -=252;	
					sfz_buf.returnBufLen =252;	
				}
				else
				{
					memcpy(sfz_buf.returnBuf,sfz_plain_text+sfz_buf.p2*252,remain_len);
					sfz_buf.returnBufLen =remain_len;
					sfz_buf.sw1 =0x90; 
					sfz_buf.sw2 =0x00; 	
				}
				
			}
			break;

		default:
			break;
	}

	sfz_send_to_hid(sfz_buf.returnBuf,sfz_buf.returnBufLen);
	return TRUE;
}


void sfz_com(void)
{
	u8 rcvState = 0;


	if(USB_Received_Flag)
	{

		//-----------------------------------------------
		rcvState = sfz_pars1(USB_Receive_Buffer,USB_Received_Len);	// 数据处理
		{

		}
		USB_Received_Flag =0;
		USB_Received_Len = 0;	
		memset(USB_Receive_Buffer,0x00,256);		
	}

		
}






static void sfz_sendERROR(u8 state)
{
	u8 sendBuf[MAC_RETURN_LEN]={0x00};
	u32 sendLen = 0;

	if (sfz_buf.returnBufLen > 1)
		sfz_buf.returnBufLen = 0;
	sfz_sendERRORPackage(sfz_buf.returnBuf,sfz_buf.returnBufLen,state,sendBuf,&sendLen);	
   // Uart_Clr_Event(SFZCOM_UART,E_uart_idle);
    //Uart_Dma_Clr(SFZCOM_UART);

//	for (u32 i=0;i<sendLen;i++)
//	{
//		if (i%2)
//			sendBuf[i] ^= 0x83;	
//		else
//			sendBuf[i] ^= 0x51;	
//	}
	USB_hid_send(sendBuf,0xFF);
	//Uart_Write_Data(SFZCOM_UART,sendBuf,sendLen);
}






static void sfz_sendTRUE(u8 *src,u32 len,u8 returnType)
{
	u8 sendBuf[MAC_RETURN_LEN]={0x00};
	u32 sendLen = 0;
	u8 send_times=0;
	u8 tmp1[3] = {0x00,0x88,0x00};	// 0f 00 88 00 52 0a f0 00 8f 2f 56 69 68 92 a8 41 外部认证
	u8 tmp2[3] = {0x00,0x82,0x00};	// 0f 00 82 00 52 0a f0 01	9b da 75 7a ca f9 be fc 内部认证
	
	sfz_buf.sw1 =0x90;
	sfz_buf.sw2 =0x00;
	if ((memcmp(src,tmp1,3) == 0))
	{
		sfz_buf.sw1 =0x63;
		sfz_buf.sw2 =0xC0|(SFZ_DELAY_MAX_COUNT-sfz_deleySend.count);
		sfz_buf.returnBufLen = 0x08;
		memcpy(sfz_buf.returnBuf,src+7,sfz_buf.returnBufLen);
	}
	else if ((memcmp(src,tmp2,3) == 0))
	{
		sfz_buf.sw1 =0x90;
		sfz_buf.sw2 =0x00;
		sfz_buf.returnBufLen = 0x08;
		memcpy(sfz_buf.returnBuf,src+7,sfz_buf.returnBufLen);
	}
	


	memset(sendBuf,0x00,sizeof(sendBuf));
	sfz_sendPackage(sfz_buf.returnBuf,sfz_buf.returnBufLen,COM_OK,returnType,sendBuf,&sendLen);
	if(sendLen>0xff)
	{
		send_times = sendLen/0xff +1;
	   	for(u8 i=0;i<send_times;i++)
	   	{
			USB_hid_send(sendBuf+i*0xff,0xff);	
	   	}
	}
	else
	{
		USB_hid_send(sendBuf,0xff);	
	}


	//Uart_Write_Data(SFZCOM_UART,sendBuf,sendLen);	
}

static void sfz_sendPackage(u8 *scr,u32 scrLen,u8 result,u8 returnType,u8 *dst,u32 *dstLen)
{
	u32 ptr=0;

	dst[ptr++] = sfz_buf.sw1;
       dst[ptr++] = sfz_buf.sw2;
	dst[ptr++] = scrLen;
	memcpy(&dst[ptr],scr,scrLen);
	ptr+=scrLen;
	*dstLen = ptr;




//    len =  scrLen;
//	len++;	// 增加一字节的result
//	len++;	// 增加一字节的reutrnType
//	len++;	// 增加一字节的本身内容的长度
//    
//	dst[ptr++] = COM_STX;
//	dst[ptr++] = sfz_buf.cla + 0x10;
//	dst[ptr++] = sfz_buf.ins;
//	dst[ptr++] = (len>>8)&0xff;
//	dst[ptr++] = len&0xff;
//	dst[ptr++] = result;
//	dst[ptr++] = returnType;
//	dst[ptr++] = scrLen;
//	for (u32 i=0;i<scrLen;i++)
//	{
//		dst[ptr++] = scr[i];
//	}
//	dst[ptr++] = COM_ETX;
//	u8 lrc = sfz_LRC(&dst[1],ptr - 2);
//	dst[ptr++] = lrc;
//	*dstLen = ptr;
}	

static void sfz_sendERRORPackage(u8 *scr,u32 scrLen,u8 result,u8 *dst,u32 *dstLen)
{
	u32 ptr=0;

	dst[ptr++] = sfz_buf.sw1;
       dst[ptr++] = sfz_buf.sw2;
	dst[ptr++] = scrLen;
	memcpy(dst[ptr],scr,scrLen);
	ptr+=scrLen;
	*dstLen = ptr;
}

static u8 sfz_LRC(u8 *data,u32 len)
{
	u8 lrc = 0;
	for (u32 i=0;i<len;i++)
		lrc ^= data[i];	

	return lrc;
}

/*******************************************************
输入:rcv UART过来的数据，len长度 
返回:false 错误  1 串口返回数据 2 SAM返回数据
*******************************************************/
static u8 sfz_communication(u8 *src,u8 dataType,u8 *dst,u32 *dstLen)
{
	u8 timeCount = 15;		// sam的I2C通讯60ms足够了，但是最后等待SAM解析回传的要远大于60ms，现在设置成900ms
    Uart_Clr_Event(SFZ_UART,E_uart_idle);
    Uart_Dma_Clr(SFZ_UART);
    // 1、发送串口数据
    if (dataType == 0)
    {        
		Uart_Write_Data(SFZ_UART, &src[1],src[0]);
		sfz_buf.packSerial++;		// 指向下一包数据
    }
	else
	{
		sfz_writeSam(&src[1],src[0]);
		sfz_buf.packSerial++;		// 指向下一包数据
	}
	TIM3->CNT = 0;
	while (timeCount)
	{
		// 1.1 查询是否有串口数据返回
		if ((Uart_Get_Event(SFZ_UART)&E_uart_idle) == E_uart_idle) 
		{
			u32 len;
			u8 *rcv;
			rcv = Uart_Get_Data(SFZ_UART,(hw_size_t *)&len);
	      	Uart_Clr_Event(SFZ_UART,E_uart_idle);
	      	Uart_Dma_Clr(SFZ_UART);
			*dstLen = len;
			memcpy(dst,rcv,len);
			return SAM_UART;
		}
		// 1.2 查询是否有SAM模块数据需要接收
		if (TX_FRAME)
		{
			u16 len,reclen;
			u8 tmp[160];
			
			len = sfz_reardSam(tmp);
			if (len != 0)
			{

				if(sfz_buf.packNumber==0x15 && sfz_buf.packSerial==2)
				{
					sfz_buf.packNumber=0x15;
				}


	
				//sfz_buf.packSerial++;		// 指向下一包数据
				if (sfz_buf.packSerial < sfz_buf.packNumber)
					;
				else
				{
					*dstLen = len;
					memcpy(dst,tmp,len);
					return SAM_I2C;  // 返回数据进行网络操作
				}

				reclen = sfz_buf.packBuf[sfz_buf.packSerial][0];
				memcpy(tmp,&sfz_buf.packBuf[sfz_buf.packSerial][1],reclen);
				sfz_delay(100);
				sfz_writeSam(tmp,reclen);
				sfz_buf.packSerial++;		// 指向下一包数据

	


				sfz_sendSAM[0] =  reclen;
				memcpy(&sfz_sendSAM[1],tmp,reclen);
			}
			TIM3->CNT = 0;					// 发送数据给SAM后，等待SAM的应答超时时间
		}
        if(TIM3->CNT>60000)
    	{
    		if (timeCount)
    			timeCount--;
			TIM3->CNT = 0;
    	}
	}
	return SAM_ERROR;
}


u8 g_delayFlay = 0; 	//0 不延时  1延时
/*******************************************************
输入:rcv UART过来的数据，len长度 
返回:false 错误  1 串口返回数据 2 SAM返回数据 0 超时
*******************************************************/
static u8 sfz_againSAM(u8 *src,u8 *dst,u32 *dstLen)
{

	g_delayFlay = 1;
	sfz_writeSam(&src[1],src[0]);
	g_delayFlay = 0;
  	Uart_Clr_Event(SFZ_UART,E_uart_idle);
  	Uart_Dma_Clr(SFZ_UART);
	TIM3->CNT = 0;
	while (TIM3->CNT < 60000)
	{
		// 1.1 查询是否有串口数据返回
		if ((Uart_Get_Event(SFZ_UART)&E_uart_idle) == E_uart_idle) 
		{
			u32 len;
			u8 *rcv;
			rcv = Uart_Get_Data(SFZ_UART,(hw_size_t *)&len);
	      	Uart_Clr_Event(SFZ_UART,E_uart_idle);
	      	Uart_Dma_Clr(SFZ_UART);
			*dstLen = len;
			memcpy(dst,rcv,len);

			return SAM_UART;
		}
		// 1.2 查询是否有SAM模块数据需要接收
		if (TX_FRAME)
		{
			u16 len;//,reclen;
			u8 tmp[160];
			
			len = sfz_reardSam(tmp);
			if (len != 0)
			{
				return SAM_I2C;
			}
		}
	}
	return SAM_ERROR;	// 超时
}


u8 isUpSCLK(void)
{
    TIM2->CNT = 0;
    while(TX_FRAME)
    {
        while(SCLK)
        {
            
            if (TIM2->CNT > 8*2)    // 大于8us
                break;
            if (!TX_FRAME) goto retn;
        }
        if (TIM2->CNT > 2*2)
          break;
    }
retn:
    if (TX_FRAME)
        return TRUE;
    else
        return FALSE;
}

u8 isDownSCLK(void)
{
    TIM2->CNT = 0;
    while(TX_FRAME)
    {
        while(!SCLK)
        {
            
            if (TIM2->CNT > 8*2)    // 大于8us
                break;
            if (!TX_FRAME) goto retn;
        }
        if (TIM2->CNT > 2*2)
          break;
    }
retn:
    if (TX_FRAME)
        return TRUE;
    else
        return FALSE;
}

static u16 sfz_reardSam(u8 *dst)
{
	u8 tmp=0;
	u16 len=0;
	 __disable_irq ();
	sfzSDAT_dir(0);
	sfzSCLK_dir(0);
	if (TX_FRAME)
	{
        isUpSCLK();//while(SCLK); 
		while (TX_FRAME)
		{
            tmp = 0;
			for (u8 i=0;i<8;i++)
			{
				tmp <<= 1;
                if (isDownSCLK() == FALSE) goto retn;
				if (SDAT)
				{
					tmp = tmp|0x01;
				}
                if (isUpSCLK() == FALSE) goto retn;
			}

			sfzSDAT_dir(1);
			SDAT_L;
            if (isDownSCLK() == FALSE) goto retn;
			*dst = tmp;
            if (isUpSCLK() == FALSE) goto retn;
			sfzSDAT_dir(0);
			dst++;
			len++;
		}
retn:
	   SDAT_H;
	   SCLK_H;
	__enable_irq();
	   return len;
	}
	__enable_irq();
	return 0;
}

void sfz_writeSam(u8 *src,u32 len)
{
	u8 tmp,i;

	 __disable_irq ();
     SCLK_H;
	 SDAT_H;
	 sfzSDAT_dir(1);
	 sfzSCLK_dir(1);
	 RX_FRAME_H;

	 TWI_NOP;
	 SDAT_L;
	 TWI_NOP;
	 SCLK_L;
	 while (len--)
	 {
	     tmp =*src;
		 for (i=0;i<8;i++)
		{   
			if (tmp & 0x80)
			{
			 	SDAT_H;
			}else{
				SDAT_L;
			}
			tmp <<= 1;
			TWI_NOPL;		
			SCLK_H;
			TWI_NOP;
			SCLK_L;				
		}
		sfzSDAT_dir(0);
		TWI_NOP;
		SCLK_H;
		TWI_NOP;
        SCLK_L;
        src++;
		sfzSDAT_dir(1);
	}
	 TWI_NOP;
	SCLK_H;
	sfz_delay(1);
	SDAT_H;
	sfzSDAT_dir(1);
	sfz_delay(1);
	__enable_irq();

	if(g_delayFlay)
	{
		TIM3->CNT = 0;
		while (TIM3->CNT <= SFZ_DELAY_RX_FRAME_H)
		{
			if ((Uart_Get_Event(SFZCOM_UART)&E_uart_idle) == E_uart_idle) 
				break;
		}
	}
	//sfz_delay(20000);
    //sfz_delay(20000);
    //sfz_delay(18000);
	RX_FRAME_L;
}

static void sfzSDAT_dir(u8 dir)
{	
    if (dir)
    {
        SDAT_PORT->CRH&=( ~(0x0f<<12) );
        SDAT_PORT->CRH |=(7<<12);
    }
    else
    {
        SDAT_PORT->CRH&=( ~(0x0f<<12) );
        SDAT_PORT->CRH |=(1<<14);
    }
}

static void sfzSCLK_dir(u8 dir)
{
    if (dir)
    {
        SCLK_PORT->CRH&=( ~(0x0f<<8) );//clear bit 12,13 ,14.15 
        SCLK_PORT->CRH |=(7<<8);//set for 50MHZ output and pull-up
    }
    else
    {
        SCLK_PORT->CRH&=( ~(0x0f<<8) );//clear bit 8,9 ,10.11 for input
        SCLK_PORT->CRH |=(0x05<<8);//set for GPIO_Mode_IPU
    }
}
static void sfzdelayL(void)
{
	TIM2->CNT = 0; 
    while (TIM2->CNT<8);	// 5u
}	

static void sfzdelay(void)
{
	TIM2->CNT = 0; 
    while (TIM2->CNT<9);	// 5u
}

static void sfz_delay(u16 delayTime)
{
	TIM2->CNT = 0; 
    while (TIM2->CNT<delayTime*2);	// 5u
}


 void sfzIntil(void)
{
	// 1、串口初始化
	UART1_Config_Init(115200,0);	
	UART2_Config_Init(115200,0);	
	// 2、IO初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = TX_FRAME_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(TX_FRAME_PORT, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin = SDAT_PIN|SCLK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(SDAT_PORT, &GPIO_InitStructure);
    
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = RX_FRAME_PIN;
    GPIO_Init(RX_FRAME_PORT, &GPIO_InitStructure);
	// 3、延时初始化
     RCC_APB1PeriphClockCmd( 
                         RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_DeInit(TIM2);
	 /* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 65535;
	//TIM_TimeBaseStructure.TIM_Prescaler = 39;
	TIM_TimeBaseStructure.TIM_Prescaler = 72/2-1;//64;//36*104-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM2, ENABLE);

	// 4、延时初始化
     RCC_APB1PeriphClockCmd( 
                         RCC_APB1Periph_TIM3, ENABLE);
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_DeInit(TIM3);
	 /* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 65535;
	//TIM_TimeBaseStructure.TIM_Prescaler = 39;
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;//64;//36*104-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM3, ENABLE);
}


