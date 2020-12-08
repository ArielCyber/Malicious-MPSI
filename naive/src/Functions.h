#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

# include <fstream>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <bits/stdc++.h> 
#include <algorithm>
#include <time.h>
#include <cmath>
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
# include <chrono>
#include <future>
#include <set>

#include <cryptoTools/Common/BitVector.h>
#include <cryptoTools/Common/block.h>
#include <cryptoTools/Crypto/AES.h>
#include <cryptoTools/Crypto/Commit.h>

struct sender{
   int sock; 
   int connect_sock; 
   struct sockaddr_in serv_name; 
   socklen_t len; 
};

struct receiver{
   int sock; 
   struct sockaddr_in cli_name; 
};


#include <ENCRYPTO_utils/crypto/crypto.h>

typedef unsigned char BYTE;
typedef unsigned int uint;

#include "Bitstring.h"
#include "MurmurHash.h"

class Party;
class P0;
class Pi;

using namespace osuCrypto;
using namespace std;
class synchGBF{
    block* GBF;
    int GBF_length;
    std::mutex* mtxArr;
    int mtxArr_length;
    int chunk_size;
    int num_of_chunks;
    int lastChunkStart;
    int last_chunk;
    
    public:
    synchGBF(int Nbf):GBF_length(Nbf),chunk_size(256){
        std::cout<<"Chunk size ="<<chunk_size<<endl;
        GBF=new block[GBF_length];
        std::cout<<"GBF_length= "<<GBF_length<<std::endl;
        num_of_chunks=GBF_length/chunk_size+1;
        std::cout<<"num_of_chunks= "<<num_of_chunks<<std::endl;
        mtxArr_length= num_of_chunks;
        last_chunk=num_of_chunks-1;
        lastChunkStart=(num_of_chunks-1)*chunk_size;
        mtxArr= new mutex[mtxArr_length];    
    }

    inline int getChunkSize(){return chunk_size;}
    inline block* getGBF(){return GBF;}

    void XORsimple(block* toXOR)
    {
        //std::cout<<"XOR simple"<<std::endl;
        for (int chunk_num=0;chunk_num<num_of_chunks-1;chunk_num++)
        {
            mtxArr[chunk_num].lock();
            for (int j=0;j<chunk_size;j++){
                GBF[chunk_num*chunk_size+j]=GBF[chunk_num*chunk_size+j]^toXOR[chunk_num*chunk_size+j];
            }
            mtxArr[chunk_num].unlock();
        }
        //int lastChunkStart=(num_of_chunks-1)*chunk_size;
        if (lastChunkStart<GBF_length)
        {
            mtxArr[last_chunk].lock();
            for (int j=lastChunkStart;j<GBF_length;j++){
                GBF[j]=GBF[j]^toXOR[j];
            }
            mtxArr[last_chunk].unlock();
        }
        //std::cout<<"end XOR simple"<<std::endl;

    }

    void XORchunk(block* toXOR, int chunk_num)
    {
        // std::cout<<"XOR chunk: "<<chunk_num<<std::endl;
        mtxArr[chunk_num].lock();
        for (int j=0;j<chunk_size;j++){
         GBF[chunk_num*chunk_size+j]=GBF[chunk_num*chunk_size+j]^toXOR[j];
        }
        mtxArr[chunk_num].unlock();
    }

    void XORchunk_limited(block* toXOR, int chunk_num, int limit)
    {
        //std::cout<<"XOR last chunk: "<<chunk_num<<std::endl;
        mtxArr[chunk_num].lock();
        for (int j=0;j<limit;j++){
         GBF[(chunk_num)*chunk_size+j]=GBF[(chunk_num)*chunk_size+j]^toXOR[j];
        }
        mtxArr[chunk_num].unlock();
    }

    void XORordered(std::vector<block>* toXOR, int* order)
    {
        //std::cout<<"XOR ordered"<<std::endl;
        for (int chunk_num=0;chunk_num<num_of_chunks-1;chunk_num++)
        {
            mtxArr[chunk_num].lock();
            for (int j=0;j<chunk_size;j++){
                //GBF[chunk_num*chunk_size+j]=GBF[chunk_num*chunk_size+j]^toXOR[chunk_num*chunk_size+j];
                GBF[chunk_num*chunk_size+j]=GBF[chunk_num*chunk_size+j]^(*toXOR)[order[chunk_num*chunk_size+j]];
            }
            mtxArr[chunk_num].unlock();
        }
        
        //std::cout<<"lastChunkStart= "<<lastChunkStart<<std::endl;
        if (lastChunkStart<GBF_length)
        {
            mtxArr[last_chunk].lock();
            for (int j=lastChunkStart;j<GBF_length;j++){
                //std::cout<<"j is equal to "<<j<<std::endl;
                GBF[j]=GBF[j]^(*toXOR)[order[j]];
            }
            mtxArr[last_chunk].unlock();
        }
        //std::cout<<"end XOR ordered"<<std::endl;
        //for (int i=0;i<GBF_length;i++){
        // GBF[i]=GBF[i]^(*toXOR)[order[i]][BF->get_bit(i)];
        //}
    }

    void XORorderedBF(std::vector<std::array<block, 2>>* toXOR, int* order, Bitstring* BF)
    {
        //std::cout<<"XOR ordered with BF"<<std::endl;
        for (int chunk_num=0;chunk_num<num_of_chunks-1;chunk_num++)
        {
            mtxArr[chunk_num].lock();
            for (int j=0;j<chunk_size;j++){
                //GBF[chunk_num*chunk_size+j]=GBF[chunk_num*chunk_size+j]^toXOR[chunk_num*chunk_size+j];
                GBF[chunk_num*chunk_size+j]=GBF[chunk_num*chunk_size+j]^(*toXOR)[order[chunk_num*chunk_size+j]][BF->get_bit(chunk_num*chunk_size+j)];
            }
            mtxArr[chunk_num].unlock();
        }
        //int lastChunkStart=(num_of_chunks-1)*chunk_size;
        if (lastChunkStart<GBF_length)
        {
            mtxArr[last_chunk].lock();
            for (int j=lastChunkStart;j<GBF_length;j++){
                GBF[j]=GBF[j]^(*toXOR)[order[j]][BF->get_bit(j)];
            }
            mtxArr[last_chunk].unlock();
        }
        //std::cout<<"end XOR ordered with BF"<<std::endl;
        //for (int i=0;i<GBF_length;i++){
        // GBF[i]=GBF[i]^(*toXOR)[order[i]][BF->get_bit(i)];
        //}
    }
    
    ~synchGBF(){
        delete[] GBF;
        delete[] mtxArr;
        }
    
};


class Lock{
	
	std::mutex mu;
	int* turns;
	int current;
	int free_place;
	
public:

   Lock():current(0),free_place(0){}
   
   void set_players(int players){
       turns=new int[players];
   }
   
   void lock(int player){
	   while (turns[current]!=player){}
   }
   
   void unlock(){
	   current++;
   }
   
   bool wait(int player){
	   mu.lock();
	   turns[free_place++]=player;
	   mu.unlock();
	   
	   if (turns[current]==player) return false;
	   std::cout<<"wait"<<std::endl;
	   return true;
   }
	   
   ~Lock(){
	   delete [] turns;
   }
	   
};



namespace functions{

    void arrange_the_indexes(const Bitstring& BF,int Nbf,BitVector* b, int* indexes,int Not,int Ncc, crypto* crypt);
    void create_BF(std::set<int>* h_kokhav,std::vector <int>& items, int len, Bitstring& BF, int Nbf,int seeds_num, unsigned int* seeds, block seed);
    void get_sub_group(int* ,int,int,block&);
    //void compute_keys(vector <int>& items,BYTE** RBF,BYTE** keys,int len_,int Nbf, int k,int seeds_num);
    void compute_r(block& r,std::vector<std::array<block, 2>>& strings,int* indexes,int indexes_size); 
    void compute_R_r( int* sub_group,int* indexes,BitVector* b,int Ncc,int Not, std::vector<block>& strings, block& r, std::vector <int> &R);
    int if_disjoint_sets(int *,int, int *,int, int);
    //void find_intersection(vector <int>& intersec,int ,BYTE** keys1,int keys2_len,BYTE** keys2, vector<int>& ,int);
    unsigned int* get_seeds(int seeds_num, block seed);	
    //void create_h_kokhav( set<int>* h_kokhav,vector <int>& items,int n, int Nbf, int seeds_num, unsigned int* seeds);
    block* re_randomization(vector <int>& items, block* Y, int n , int Nbf, int bytes , set<int>* h_kokhav ,crypto* crypt, Pi* pi, block* GBF, synchGBF* r);
    block* re_randomization_zero(vector <int>& items, block* Y, int n , int Nbf, int bytes , set<int>* h_kokhav ,crypto* crypt, Pi* pi, block* GBF, synchGBF* test);
    block* check_the_item(set<int>& h_kokhav,int bytes, int Nbf,block* GBF);
	
    void offline_apport_receiver(Party* party,std::vector<std::string>* ips, int** ports,int player, int other_player);
    void offline_apport_sender(Party* party,std::vector<std::string>* ips, int** ports,int player, int other_player);
    void online_apport_sender(Party* party, synchGBF* r);
    void online_apport_receiver(Party* party, synchGBF* r);
    void run_online_apport(Party* p0, synchGBF* r);
    void run_offline_apport(Party* p0,std::vector <std::string>* ips,int** ports,int player,int other_player);

    void set_ports(int** ports,int player,int parties);
    void set_ips(std::vector <string>& ips,string file, int* parties);
    void read_protocol_parameters(const char* cfg_file,uint32_t* seeds,uint32_t* Nbf,uint64_t* Not,uint32_t* numOFOnes,uint32_t* Nc,uint32_t* Nr,string& data,string& ip);
    void get_values(string values_file,vector <int>& values);
	
    void seeds_pi_write(Pi* pi,Commit* commit,block* seed,int party);
    void seeds_pi_read(Pi* pi,Commit* commit,block* seed,int party);
    void seeds_agreement_pi(Pi* pi,int parties,Commit& commit,block& seed,int player);
    void seeds_p0_write(P0* p0,Commit* commit,block* seed);
    void seeds_p0_read(P0* p0,Commit* commit,block* seed);
    void seeds_agreement_p0(std::vector<P0*>& P0_s,Commit& commit,block& seed);
    void set_seed_commit(Commit& commit, block& seed,crypto* crypt);
	

    void secret_write(Pi* pi,int other_player,block* keys);
    void secret_read(Pi* pi,int other_player,block* keys);
    void secret_write_p0(P0* p0,block* keys);
    void secret_read_p0(P0* p0,block* keys);
	
    std::chrono::steady_clock::time_point get_duration(std::chrono::steady_clock::time_point& start,fstream& fout);
	
    void get_intersection( std::set<int>* h_kokhav,int bytes,block* GBF);
    block* get_Y(int items_size,Pi* pi,std::set<int>* h_kokhav, block* Y, synchGBF* test);
    void comulative_gbf_pi(Pi* pi,int Nbf,block* GBF1);
	
    block* compute_gbf(std::vector<P0*>& P0_s,int Nbf, synchGBF* test);

    block* compute_gbf_hyper_cube(int parties,std::vector<P0*>& P0_s, synchGBF* test);
    void get_gbf1(P0* p0,block* GBF,block* M1_STAR,block* M0_KOVA,Lock* lock,int j);
	
    void get_gbf(P0* p0, synchGBF* r);
    //void pi_hyper_cube(int player,int parties,Pi* pi,int Nbf,block* GBF1,block* r);
    void hyper_cube_pi_read(Pi* pi,Lock* lock,block* GBF,int Nbf,int other_player,int j);
	
    void xor_vals0(block* GBF,block GBF1_STAR,block M1_STAR,block M0_KOVA, Lock* lock,int j);
    void xor_vals2(block b,Lock* lock,block* GBF,int j);
	
    void secret_sharing_seed_p0(vector <P0*>& p0_s,int parties,block* keys,block* keys_recv);
    void secret_sharing_seed_pi(Pi* pi,int parties,crypto* crypt,int player,block* keys,block* keys_recv);
    void secret_sharing_aes(int parties,uint32_t Nbf,synchGBF* tesr,block* keys,block* keys_recv);
	
    void compute_keys(P0* p0,std::set<int>* h_kokhav);
    void recv_keys(P0* p0,block* keys);
	
    void xor_2(int parties, block **keys, block* new_keys, int* items_ind,int items_size, int counter, int index, int player,block sum);
    void xor_3(int parties, block **keys, block* new_keys,int items_size, int counter, int player,block sum);
    void merge_sort2(block* arr,int low,int high,int* arr_items);
    void merge_sort1(block* arr,int low,int high);
    bool binary_search(block* arr,int size,block find);
    void creating_p0_instance(int party,int port,std::vector<P0*>* P0_s,uint32_t numOfOnes,uint32_t m_nSecParam, uint8_t* constSeed );

    void compute_keys_p0(block* keys,std::set<int>* h_kokhav,int items_size,P0* p);
    void compute_keys_pi(block* keys,std::set<int>* h_kokhav,int items_size,Pi* p);

}

#endif
