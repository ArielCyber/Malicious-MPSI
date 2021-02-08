#include "Pi.h"


Pi::Pi(int numOfOnes,uint32_t m_nSecParam, uint8_t* constSeed,int parties,int ** ports, vector<string>& ips,int player):
Party(numOfOnes,m_nSecParam,constSeed),player(player),parties(parties)
{
	 
        //initializing the socket as a receiver (communicating with P0)
	init_receiver(ips[0].c_str(),ports[0][1]);
	 
	//building sockets to communicate with the other parties
	senders=new struct sender[parties-1-player];
        receivers=new struct receiver[player-1];	
	 
	for (int p=1;p<player;p++)
             Pi::init_receiver(&receivers[p-1], ips[p].c_str(),ports[p][1]);			
		    
        for (int p=player+1;p<parties;p++)
             Pi::init_sender(&senders[p-player-1], ports[p][0]);
}


/**
 * writing/reading to/from the other parties
 * 
 * @param other_player the opponent player 
 * @param msg the message
 * @param size the message's 

*/

unsigned long Pi::write_to_player(int other_player,void* msg,unsigned long size){
    if (other_player==0) return this->write_as_a_receiver(msg,size);
    else if (other_player<player) return this->write_as_a_receiver(&receivers[other_player-1],msg,size);
    else return this->write_as_a_sender(&senders[other_player-player-1],msg,size);
}

unsigned long Pi::read_from_player(int other_player,void* msg,unsigned long size){
    if (other_player==0) return read_as_a_receiver(msg,size);
    else if (other_player<player) return read_as_a_receiver(&receivers[other_player-1],msg,size);
    else return read_as_a_sender(&senders[other_player-player-1],msg,size);
}


Pi::~Pi(){

   for (int i=0;i<(player-1);i++){
       close(receivers[i].sock);
   }
		    
   for (int i=0;i<(parties-1-player);i++){
       close(senders[i].connect_sock);
       close(senders[i].sock);
   }
		
   delete [] senders;
   delete [] receivers;

}
