#ifndef __PARTY_H
#define __PARTY_H

#include "Functions.h"
#include "kos.h"

using namespace osuCrypto;

class Party
{
public:

       virtual crypto* getCrypto()=0; 
       virtual void init_sender(int port)=0; 
       virtual void init_receiver(const char* ip,int port)=0; 
	   
       virtual void send_sub_group()=0;  
       virtual void set_sub_group()=0;  
       virtual void recv_C()=0;
	   
       virtual void compute_R_r()=0;
       virtual void send_R_r()=0;
       virtual void recv_R()=0;
	   
       virtual void create_RBF_sender(synchGBF* r)=0; 
       virtual void create_RBF_receiver(synchGBF* r)=0;	   
	   
       virtual void recv_func()=0;
       virtual void send_func()=0; 

       virtual int check_if_to_abort()=0;   
       virtual int check_if_to_abort1()=0;
       virtual int check_if_to_abort2()=0;
       
       virtual void write_as_a_receiver(void* msg,unsigned long size)=0;
       virtual int read_as_a_receiver(void* msg,unsigned long size)=0; 

       virtual void write_as_a_sender(void* msg,unsigned long size)=0;
       virtual int read_as_a_sender(void* msg,unsigned long size)=0; 
	
       virtual void arrange_the_indexes()=0;

       virtual BitVector* getChoices() const=0;
       virtual std::vector<block>& getStringsReceiver()=0;
       virtual  std::vector<std::array<block, 2>>& getStrings()=0;
       virtual int getK()=0;
       virtual block* get_RBF_recv() const=0;
       virtual ~Party(){};
       virtual int getNot() const=0;
	   
};
	

#endif
