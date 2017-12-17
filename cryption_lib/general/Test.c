/*************************************************
  Copyright (C),   
  File name:       test.c
  Author:          
  Version:         V1.0 
  Date: 		   
  Description:     
  Function List:  
    			   
  History:         				  
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include "sha1.h"
#include "sha256.h"
void Test_Sha1(void)
{
 	char result[20];
    	uint32_t  j;
	
	   char *buf[] = {
    "",
    "da39a3ee5e6b4b0d3255bfef95601890afd80709",

    "abc",
    "a9993e364706816aba3e25717850c26c9cd0d89d",

    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    "84983e441c3bd26ebaae4aa1f95129e5e54670f1",

    "The quick brown fox jumps over the lazy dog",
    "2FD4E1C67A2D28FCED849EE1BB76E7391B93EB12",

    "The quick brown fox jumps over the lazy cog", /* avalanche effect test */
    "DE9F2C7FD25E1B3AFAD3E85A0BD17D9B100DB4B3",

    "bhn5bjmoniertqea40wro2upyflkydsibsk8ylkmgbvwi420t44cq034eou1szc1k0mk46oeb7ktzmlxqkbte2sy",
    "456DBB71852804BF540F2893429FF2BAE7D8206E"
};

	for (j = 0; j < (sizeof(buf)/sizeof(buf[0])); j += 2)
	{
 		SHA1( buf[j], strlen(buf[j]),result);
	}
}

void Test_Sha256(void)
{	
    uint8_t result[32];
    uint32_t  j;
    char *buf[] = {
    "",
    "e3b0c442 98fc1c14 9afbf4c8 996fb924 27ae41e4 649b934c a495991b 7852b855",

    "abc",
    "ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 96177a9c b410ff61 f20015ad",

    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    "248d6a61 d20638b8 e5c02693 0c3e6039 a33ce459 64ff2167 f6ecedd4 19db06c1",

    "The quick brown fox jumps over the lazy dog",
    "d7a8fbb3 07d78094 69ca9abc b0082e4f 8d5651e4 6d3cdb76 2d02d0bf 37c9e592",

    "The quick brown fox jumps over the lazy cog", /* avalanche effect test */
    "e4c4d8f3 bf76b692 de791a17 3e053211 50f7a345 b46484fe 427f6acc 7ecc81be",

    "bhn5bjmoniertqea40wro2upyflkydsibsk8ylkmgbvwi420t44cq034eou1szc1k0mk46oeb7ktzmlxqkbte2sy",
    "9085df2f 02e0cc45 5928d0f5 1b27b4bf 1d9cd260 a66ed1fd a11b0a3f f5756d99"
};


    for (j = 0; j < (sizeof(buf)/sizeof(buf[0])); j += 2)
    {
	HAL_SHA256_Once((uint8_t *)buf[j], (uint32_t)strlen(buf[j]),result);

    }

    return ;
}

void Test_MD5(void)
{	
    uint8_t result[16];
    uint32_t  j;
    char *buf[] = {
    "",
    "D41D8CD98F00B204E9800998ECF8427E",

    "abc",
    "900150983CD24FB0D6963F7D28E17F72",

    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    "8215EF0796A20BCAAAE116D3876C664A",

    "The quick brown fox jumps over the lazy dog",
    "9E107D9D372BB6826BD81D3542A419D6",

    "The quick brown fox jumps over the lazy cog", /* avalanche effect test */
    "1055D3E698D289F2AF8663725127BD4B",

    "bhn5bjmoniertqea40wro2upyflkydsibsk8ylkmgbvwi420t44cq034eou1szc1k0mk46oeb7ktzmlxqkbte2sy",
    "D3BE5469A8395B9EFF169D5C120F2B95"
};


    for (j = 0; j < (sizeof(buf)/sizeof(buf[0])); j += 2)
    {
	MD5_Calculation((uint8_t *)buf[j], (uint32_t)strlen(buf[j]),result);

    }

    return ;
}





