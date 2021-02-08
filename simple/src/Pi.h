#ifndef __PI_H
#define __PI_H

#include <string.h>
#include<iostream>
#include <arpa/inet.h>

#include "Party.h"

class Pi: public Party{
	
public:

       //sockets for communicating with the other parties (not with P0)
       struct sender* senders;      	    
       struct receiver* receivers;   
	   
       //number of player
       int player;
	
       //number of parties
       int parties;

       Pi(int numOfOnes,uint32_t m_nSecParam, uint8_t* constSeed,int parties,int ** ports, vector<string>& ips,int player);

       //communicating with the other players
       unsigned long write_to_player(int other_player,void* msg,unsigned long size);
       unsigned long read_from_player(int other_player,void* msg,unsigned long size);

       ~Pi();
	
};


#endif





