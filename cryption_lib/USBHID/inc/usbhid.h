#ifndef __USB_HID_H_
#define	__USB_HID_H_

typedef struct
{
	unsigned char ucbHead[2];
	unsigned char ucType;
	unsigned char ucIndex;
	unsigned char ucbLen[2];
	unsigned char ucbCmd[2];
	unsigned int uiLen;
	unsigned char * pucInData;
	unsigned char * pucOutData;
	unsigned char ucXOR;
	unsigned char ucbTail[2];
}USBHID_MESSAGE; 



extern unsigned char frame_buffer[3096];
extern unsigned int frame_len ;
extern unsigned char frame_flag;

extern uint8_t USB_Receive_Buffer[];
extern uint8_t USB_Send_Buffer[];
extern volatile uint8_t USB_Received_Flag;
extern uint8_t USB_Received_Len;

void USB_hid_send(unsigned char *pdata,unsigned short len);//unsigned char *pdata,
void HID_CMD_ANSWER(unsigned char * pucBuf, unsigned char ret);
void HID_CMD(void);

#endif
