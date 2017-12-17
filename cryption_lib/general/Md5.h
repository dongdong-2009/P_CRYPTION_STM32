#ifndef  _MD5_H_
#define  _MD5_H_


//---------------------------------------------------------------------
#define R_memset(x, y, z) memset(x, y, z)
#define R_memcpy(x, y, z) memcpy(x, y, z)
#define R_memcmp(x, y, z) memcmp(x, y, z)

typedef unsigned long UINT4;
typedef unsigned char *POINTER;

//----------------------------结构体定义-----------------------------------------
/* MD5 context. */
typedef struct {
   /* state (ABCD) */   
   /*四个32bits数，用于存放最终计算得到的消息摘要。当消息长度〉512bits时，也用于存放每个512bits的中间结果*/ 
   UINT4 state[4];   

   /* number of bits, modulo 2^64 (lsb first) */    
   /*存储原始信息的bits数长度,不包括填充的bits，最长为 2^64 bits，因为2^64是一个64位数的最大值*/
   UINT4 count[2];
  
   /* input buffer */ 
   /*存放输入的信息的缓冲区，512bits*/
   unsigned char buffer[64];  
} MD5_CTX;

//----------------------------外部函数接口-----------------------------------------
extern void MD5Init(MD5_CTX *);
extern void MD5Update(MD5_CTX *, unsigned char *, unsigned int);
extern void MD5Final(unsigned char [16], MD5_CTX *);
extern void MD5_Calculation(unsigned char * input, unsigned long blen, unsigned char * output);
#endif

