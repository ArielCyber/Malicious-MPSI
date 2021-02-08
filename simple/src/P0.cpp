#include "P0.h"

P0::P0(int port,int numOfOnes,uint32_t m_nSecParam,uint8_t* constSeed ):Party(numOfOnes,m_nSecParam,constSeed)
{
     //initializing the socket as a sender
     init_sender(port);
}
