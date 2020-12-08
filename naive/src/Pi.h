#ifndef __PI_H
#define __PI_H

#include <string.h>
#include<iostream>
#include <arpa/inet.h>

#include "Party.h"

class Pi: public Party{
	
public:

       crypto* crypt;
       //sender
       struct sender* senders;
       struct sender s;
       int* sub_group_sender;	   
       int Nr_sender;	
       int* R_sender;
       block r_sender;	
       int Nc_sender;    
       int* func_sender;	 
       char buffer_sender[100]; 
       block* RBF_sender;
       std::vector<std::array<block, 2>>* strings;
       block seed_sub_group;   
       //int connection_status_sender;
	   
       //receiver
       struct receiver* receivers;
       struct receiver recv;
       int* sub_group_recv;
       int Nr_recv;
       std::vector <int>* R_recv;	
       block r_recv;	
       int Nc_recv;	 
       int* func_recv;	 
       char buffer_recv[100]; 	
       block* RBF_recv;	 
       BitVector* b;
       std::vector<block>* strings_choices;   
       std::vector<int> intersection_recv;
       std::vector <int>& items; 
       Bitstring* BF; 
       //int connection_status_recv;
	   
       int Not;
       int Nbf;
       int k;
       int maxOnes;
       int seeds_num;
       int items_size;
       int player;
       int parties;

       Pi(std::vector <int>& items,int  items_size,int Not,int Nbf,int Nc,int k,int maxOnes,int seeds_num,int numOfOnes,uint32_t m_nSecParam, uint8_t* constSeed ,int parties,int ** ports, vector<string>& ips,int player);
      
       crypto* getCrypto(); 	
       void init_sender(int port);
       static void init_sender(struct sender* s, int port);
       void init_receiver(const char* ip,int port);
       static void init_receiver(struct receiver* recv,const char* ip,int port);
	
       void set_sub_group();
       void send_sub_group();

       void recv_R();
       int check_if_to_abort1();

       void recv_func();
       int check_if_to_abort2();
       void create_BF(std::set<int>* h_kokhav,unsigned int* hash_seeds, block seed);
       void create_RBF_sender(synchGBF* r);
	   
       void write_as_a_receiver(void* msg,unsigned long size);
       int read_as_a_receiver(void* msg,unsigned long size); 
       void write_as_a_sender(void* msg,unsigned long size);
       int read_as_a_sender(void* msg,unsigned long size); 
	   
       static void write_as_a_sender(sender* snd,void* msg,unsigned long size);
       static void write_as_a_receiver(receiver* recv,void* msg,unsigned long size);
       static int read_as_a_receiver(receiver* recv,void* msg,unsigned long size);
       static int read_as_a_sender(sender* snd,void* msg,unsigned long size);
	   
       void write_to_player(int other_player,void* msg,unsigned long size);
       void read_from_player(int other_player,void* msg,unsigned long size);

       void send_keys(block* keys);
       void recv_C();
       int check_if_to_abort();

       void compute_R_r();
       void send_R_r();
       void arrange_the_indexes();
       void create_RBF_receiver(synchGBF* r);

       int getNbf() const;
       void send_func();
       ~Pi();
       int getNot() const;
       BitVector* getChoices() const;
       std::vector<block>& getStringsReceiver();
       std::vector<std::array<block, 2>>& getStrings();
       int getK() ;
       block* get_RBF_recv() const;
       block* get_RBF_sender() const;
       int getSeedsNum() const;
       int* get_func_sender() const;	
};


#endif







