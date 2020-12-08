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
private:
       //sender
       struct sender s;
       int* sub_group_sender;	   
       int Nr_sender;	
       int* R_sender;
       block r_sender;	 	   
       int* func_sender;	 
       //char buffer_sender[100]; 
       block* RBF_sender;
       std::vector<std::array<block, 2>>* strings; 
       //int connection_status_sender;
       BitVector* bitvector;
	   block seed_sub_group;
       //$$$
       crypto* crypt;
       //$$$
   
       //receiver
       struct receiver recv;
       int* sub_group_recv;
       int Nr_recv;
       std::vector <int>* R_recv;	
       block r_recv;	
       int Nc_recv;	 
       int* func_recv;	 
       //char buffer_recv[100]; 	
       block* RBF_recv;	 
       std::vector<block>* strings_choices;	   
       //int connection_status_recv;
       block* GBF1_STAR;
       
public:	   

       static Bitstring* BF; 
       static int Not;
       static int Nbf;
       static int Nc_sender; 
       static int string_length;
       static int maxOnes;
       static std::vector<int>* items;
       static int items_size;
       static std::set<int>* h_kokhav;
       static int seeds_num;


       P0(int port,int numOfOnes,uint32_t m_nSecParam, uint8_t* constSeed );

       static void delete_bf();
       static void set(vector<int>* items,int items_size,int Nbf,int n,int Nc,int string_length,int maxOnes,int seeds_num);
       static void set_items(std::vector<int>* items,int items_size);
       static void set_Nbf(int Nbf);
       static void set_BF(Bitstring* BF);
       static void set_Not(int n);
       static void set_Nc_sender(int Nc);
       static void set_string_length(int string_length);
       static void set_maxOnes(int maxOnes);

       //$$$
       crypto* getCrypto();  
       //$$$

       void init_sender(int port);
       static void init_sender(struct sender* s, int port);
       void init_receiver(const char* ip,int port);
       static void init_receiver(struct receiver* recv,const char* ip,int port);
	   
       void send_sub_group();  
       void set_sub_group();  
       void recv_C();
	   
       void compute_R_r();
       void send_R_r();
       void recv_R();
	   
       void create_RBF_sender(synchGBF* r); 
       void create_RBF_receiver(synchGBF* r);	   
	   
       void recv_func();
       void send_func(); 	   

       int check_if_to_abort();	   
       int check_if_to_abort1();
       int check_if_to_abort2();   
       
	   
       void write_as_a_receiver(void* msg,unsigned long size);
       int read_as_a_receiver(void* msg,unsigned long size); 
       void write_as_a_sender(void* msg,unsigned long size);
       int read_as_a_sender(void* msg,unsigned long size); 
	   
       static void write_as_a_sender(sender* snd,void* msg,unsigned long size);
       static void write_as_a_receiver(receiver* recv,void* msg,unsigned long size);
       static int read_as_a_receiver(receiver* recv,void* msg,unsigned long size);
       static int read_as_a_sender(sender* snd,void* msg,unsigned long size);
   
       static void create_BF(std::set<int>* h_kokhav,unsigned int* hash_seeds, block seed);
	
       void arrange_the_indexes();
	   
       void set_GBF1_STAR(block* GBF1_STAR);
       block* get_GBF1_STAR();
       static void set_h_kokhav(std::set<int>* h_kokhav);
	
       BitVector* getChoices() const;
       std::vector<block>& getStringsReceiver();
       std::vector<std::array<block, 2>>& getStrings();
       int getK();
       block* get_RBF_recv() const;
       block* get_RBF_sender() const;	
       int getNot() const;
       int getSeedsNum() const;
	   
       static std::set<int>* get_h_kokhav();
	   
       ~P0();
	   
};

#endif

