#if defined(WIN32) || defined(WIN16) || defined(WINDOWS)
#ifndef MSDOS
#define MSDOS
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#ifndef MSDOS
#include <unistd.h>
#else
#include <io.h>
#endif
#include <string.h>
#include "des.h"
int hert_des_ecb_crypt(unsigned char *cbc_in,unsigned char *key_data,int size,int type)
{
	int i,j;
	des_key_schedule ks;
	int num;
	int rel_len=size; 	
  des_cblock * rel_in=(des_cblock * )cbc_in;
	if ((j=des_key_sched((des_cblock  *)(key_data),ks)) != 0)
		{
			printf("Key error %d\n",j);
			
			return -1;
		}

	if(size%8){
		printf("in crypto error %d\n",size);
		return -1;
	}
   des_cblock * rel_out=(des_cblock * )malloc(rel_len);
	bzero(rel_out,rel_len);


	for(i=0;i<rel_len/8;i++)
	{
		if(type){
		des_ecb_encrypt(rel_in+i,rel_out+i,ks,DES_ENCRYPT);
		}
		else{	
		des_ecb_encrypt(rel_in+i,rel_out+i,ks,DES_DECRYPT);
		}
	}
	bzero(rel_in,rel_len);
	memcpy(rel_in,rel_out,rel_len);
	free(rel_out);
	return 0;
}
