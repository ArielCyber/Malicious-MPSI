#ifndef __P0_H
#define __P0_H

#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include<iostream>
#include <arpa/inet.h>

#include "Party.h"

class P0 : public Party
{
		         
public:	   
       P0(int port,int numOfOnes,uint32_t m_nSecParam, uint8_t* constSeed );      
};

#endif
