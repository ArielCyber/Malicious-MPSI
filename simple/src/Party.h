#ifndef __PARTY_H
#define __PARTY_H

#include "Functions.h"
#include "kos.h"

using namespace osuCrypto;

class Party
{
	
protected:

       //play as a sender
       struct sender s;
       int* func_sender;
       vector<int>* arr_indexes;
       int* sub_group_sender;
       int Nr_sender;	
       int* R_sender;
       block r_sender;	
       std::vector<std::array<block, 2>>* strings; 
       BitVector* bitvector;
       block seed_sub_group;
       crypto* crypt;
	   
       //play as a receiver
       struct receiver recv;
       int* func_recv;	
       int* new_indexes;
       std::vector<block>* strings_choices;	
       block r_recv;	
       int Nc_recv;	  
       int* sub_group_recv;
       int Nr_recv;
       std::vector <int>* R_recv;
	   
public:
	
       //common attributes
       static int Nbf;
       static int Nc_sender; 
       static Bitstring* BF; 
       static std::vector<int>* items;
       static int Not;
       static int maxOnes;
       static int items_size;
       static int seeds_num;

       //c'tor       
       Party(int numOfOnes,uint32_t m_nSecParam,uint8_t* constSeed );
       
       //setter	
       static void set(vector<int>* items,int items_size,int Nbf,int n,int Nc,int string_length,int maxOnes,int seeds_num);

       //getters
       int getNot() const;
       int getNbf() const;
       int getSeedsNum() const;
       crypto* getCrypto(); 
       BitVector* getChoices() const;
       std::vector<block>& getStringsReceiver();
       std::vector<std::array<block, 2>>& getStrings();

       //openning sockets with the other players
       static void init_sender(struct sender* s, int port);
       static void init_receiver(struct receiver* recv,const char* ip,int port);
       void init_sender(int port); 
       void init_receiver(const char* ip,int port); 

       //writing/reading to/from the other player
       unsigned long write_as_a_receiver(void* msg,unsigned long size);
       unsigned long read_as_a_receiver(void* msg,unsigned long size); 
       unsigned long write_as_a_sender(void* msg,unsigned long size);
       unsigned long read_as_a_sender(void* msg,unsigned long size);  

       //writing/reading to/from a socket
       static unsigned long write_as_a_sender(sender* snd,void* msg,unsigned long size);
       static unsigned long write_as_a_receiver(receiver* recv,void* msg,unsigned long size);
       static unsigned long read_as_a_receiver(receiver* recv,void* msg,unsigned long size);
       static unsigned long read_as_a_sender(sender* snd,void* msg,unsigned long size);
	
       //setting/sending/receiving the sub group
       unsigned long send_sub_group();  
       void set_sub_group();  
       unsigned long recv_C(); 

       //computing/sending/receiving R/r
       void compute_R_r();
       unsigned long send_R_r();
       unsigned long recv_R();  

       //creating BF
       static void create_BF_threads(std::set<int>* h_kokhav,unsigned int* hash_seeds, block seed);
	
       //creating the function
       void get_zeros_ones();
       void arrange_the_indexes();
	   
       //sending/receiving the function
       unsigned long recv_func();
       unsigned long send_func(); 
	
       //xoring the RBF with the synchGBF
       void xor_RBF_sender(synchGBF* r); 
       void xor_RBF_receiver(synchGBF* r);
	
       //freeing memory
       static void delete_bf();
       void delete_rbf_func();
	
       //security check
       int check_if_to_abort();   
       int check_if_to_abort1();
       int check_if_to_abort2(); 
	
       //d'tor
       virtual ~Party();
	   
};
	

#endif
