#include "stm32f10x.h"
#include "usbhid.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_conf.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_istr.h"


USBHID_MESSAGE g_usbhid_message;


unsigned char frame_buffer[3096];
unsigned int frame_len = 0;

unsigned int temp_len = 0;
unsigned char frame_flag= 0;

void USB_hid_send(unsigned char *pdata,unsigned short len)//unsigned char *pdata,
{

	unsigned short i=0;

	while(len>REPORT_COUNT)
	{
		UserToPMABufferCopy(pdata+i, ENDP2_TXADDR, REPORT_COUNT);
    		SetEPTxCount(ENDP2, REPORT_COUNT);
   		 SetEPTxValid(ENDP2); 
		 i+=REPORT_COUNT;
		 len-=REPORT_COUNT;
		 while(GetEPTxStatus(ENDP2)!=EP_TX_NAK);
	}
	if(len>0)
	{
		UserToPMABufferCopy(pdata+i, ENDP2_TXADDR, len);
    		SetEPTxCount(ENDP2, len);
   		 SetEPTxValid(ENDP2); 
		 while(GetEPTxStatus(ENDP2)!=EP_TX_NAK);
	}
	
}


unsigned char HID_FRAME_RECEIVE(unsigned char *pbuf,unsigned char *len)
{

	
	//接收长度
	if(pbuf[0]==0x02&&pbuf[1]==0x00)
	{
		frame_len = (pbuf[3]<<8|pbuf[4])+7;
	
	}

	//拼包
	memcpy(frame_buffer+temp_len,pbuf,*len);
	temp_len += *len;

	//FRAME 接收完成
	if(temp_len>frame_len)
	{
		frame_flag =1;
		temp_len =0;	
	
	}

	return 0;
		
}
void HID_CMD(void)
{
	unsigned char iRET;
	unsigned short len;
	if(USB_Received_Flag)
	{
		HID_FRAME_RECEIVE(USB_Receive_Buffer,&USB_Received_Len);		
		USB_Received_Flag=0;
		USB_Received_Len=0;
	}


}
