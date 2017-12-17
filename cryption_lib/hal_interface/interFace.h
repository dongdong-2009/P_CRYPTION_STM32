
#ifndef	__DEV_INTERFACE_h__
#define	__DEV_INTERFACE_h__



#define  hw_size_t  u32
#define NULL          _NULL

enum rt_device_class_type
{
	HW_Device_Class_Char = 0,						/* character device								*/
	HW_Device_Class_Block,							/* block device 								*/
	HW_Device_Class_NetIf,							/* net interface 								*/
	HW_Device_Class_MTD,							/* memory device 								*/
	HW_Device_Class_CAN,							/* CAN device 									*/
	HW_Device_Class_RTC,							/* RTC device 									*/
	HW_Device_Class_Sound,							/* Sound device 								*/
	HW_Device_Class_Unknown							/* unknown device 								*/
};

/*
 * Device related structure
 */
struct hw_device
{
	/* device type */
	enum rt_device_class_type type;
	/* device flag and device open flag*/
	u16 flag, open_flag;

	/* common device interface */
	bool  (*init)	(void);
	bool  (*open)	(u16 oflag);
	bool  (*close)	(void);
	hw_size_t (*read)	(u32 pos, void* buffer, hw_size_t size);
	hw_size_t (*write)	(u32 pos, const void* buffer, hw_size_t size);
	/* device private data */
	void* private;
};

/******************************************************************* 
函数功能:   接口注册
入口参数: 无
返 回 值: 
相关调用:
备    注: 
修改信息:         
********************************************************************/
extern void Dev_InterFace_Init(void);



#endif

