#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

#include <condition_variable>
# include <fstream>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <queue>
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
#include <stack>
#include <cryptoTools/Common/BitVector.h>
#include <cryptoTools/Common/block.h>
#include <cryptoTools/Crypto/AES.h>
#include <cryptoTools/Crypto/Commit.h>


class semaphore
{
private:
    std::mutex mutex_;
    std::condition_variable condition_;
    unsigned long count_ = 0;

public:
    void notify() {
        std::lock_guard<decltype(mutex_)> lock(mutex_);
        ++count_;
        condition_.notify_one();
    }

    void wait() {
        std::unique_lock<decltype(mutex_)> lock(mutex_);
        if (count_<=0) 
            condition_.wait(lock);
        --count_;
    }
};


class Gbf_seg;

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
    int GBF_length;
    std::mutex* mtxArr;
    int mtxArr_length;
    int chunk_size;
    int num_of_chunks;
    int lastChunkStart;
    int last_chunk;
    int segments;
    std::mutex* turn;    
    public:

    block* GBF;   

    synchGBF(int Nbf,int chunks,int segments):GBF_length(Nbf),segments(segments){
        chunk_size=ceil(Nbf/segments);
        GBF=new block[GBF_length];
        num_of_chunks=segments;
        mtxArr_length= num_of_chunks;
        last_chunk=num_of_chunks-1;
        lastChunkStart=(num_of_chunks-1)*chunk_size;
        mtxArr= new mutex[mtxArr_length];    
        }

    int get_segments(){
        return segments;
    }

    synchGBF(int Nbf,int chunks):GBF_length(Nbf),chunk_size(chunks){
        GBF=new block[GBF_length];
        num_of_chunks=GBF_length/chunk_size+1;
        mtxArr_length= num_of_chunks;
        last_chunk=num_of_chunks-1;
        lastChunkStart=(num_of_chunks-1)*chunk_size;
        mtxArr= new mutex[mtxArr_length];

    }

   
    inline int getChunkSize(){return chunk_size;}
    inline block* getGBF(){return GBF;}

    void XORsimple(block* toXOR)
    {
        for (int chunk_num=0;chunk_num<num_of_chunks-1;chunk_num++)
        {
            mtxArr[chunk_num].lock();
            for (int j=0;j<chunk_size;j++){
                GBF[chunk_num*chunk_size+j]=GBF[chunk_num*chunk_size+j]^toXOR[chunk_num*chunk_size+j];
            }
            mtxArr[chunk_num].unlock();
        }

        if (lastChunkStart<GBF_length)
        {
            mtxArr[last_chunk].lock();
            for (int j=lastChunkStart;j<GBF_length;j++){
                GBF[j]=GBF[j]^toXOR[j];
            }
            mtxArr[last_chunk].unlock();
        }


    }


    void XORchunk(block* toXOR, int chunk_num)
    {
        mtxArr[chunk_num].lock();
        for (int j=0;j<chunk_size;j++){
         GBF[chunk_num*chunk_size+j]=GBF[chunk_num*chunk_size+j]^toXOR[j];
        }
        mtxArr[chunk_num].unlock();
    }

    void XORchunk_limited(block* toXOR, int chunk_num, int limit)
    {
        mtxArr[chunk_num].lock();
        for (int j=0;j<limit;j++){
         GBF[(chunk_num)*chunk_size+j]=GBF[(chunk_num)*chunk_size+j]^toXOR[j];
        }
        mtxArr[chunk_num].unlock();
    }

    void XORordered(std::vector<block>* toXOR, int* order)
    {
        for (int chunk_num=0;chunk_num<num_of_chunks-1;chunk_num++)
        {
            mtxArr[chunk_num].lock();
            for (int j=0;j<chunk_size;j++){
                GBF[chunk_num*chunk_size+j]=GBF[chunk_num*chunk_size+j]^(*toXOR)[order[chunk_num*chunk_size+j]];
            }
            mtxArr[chunk_num].unlock();
        }
        
        if (lastChunkStart<GBF_length)
        {
            mtxArr[last_chunk].lock();
            for (int j=lastChunkStart;j<GBF_length;j++){
                GBF[j]=GBF[j]^(*toXOR)[order[j]];
            }
            mtxArr[last_chunk].unlock();
        }
    }

    void XORorderedBF(std::vector<std::array<block, 2>>* toXOR, int* order, Bitstring* BF)
    {
        for (int chunk_num=0;chunk_num<num_of_chunks-1;chunk_num++)
        {
            mtxArr[chunk_num].lock();
            for (int j=0;j<chunk_size;j++){
                GBF[chunk_num*chunk_size+j]=GBF[chunk_num*chunk_size+j]^(*toXOR)[order[chunk_num*chunk_size+j]][BF->get_bit(chunk_num*chunk_size+j)];
            }
            mtxArr[chunk_num].unlock();
        }

        if (lastChunkStart<GBF_length)
        {
            mtxArr[last_chunk].lock();
            for (int j=lastChunkStart;j<GBF_length;j++){
                GBF[j]=GBF[j]^(*toXOR)[order[j]][BF->get_bit(j)];
            }
            mtxArr[last_chunk].unlock();
        }
    }
    
    ~synchGBF(){
        delete[] GBF;
        delete[] mtxArr;
        }
    
};


class Gbf_seg{
public:
      block* chunkArr;
      int start;
      int size;
      synchGBF* test;

      Gbf_seg(block* chunkArr,int start,int size,synchGBF* test):
             chunkArr(chunkArr),start(start),size(size),test(test){}

       ~Gbf_seg(){
           delete [] chunkArr;
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

    void arrange_indexes_thread(const Bitstring* BF,int Nbf,int* new_indexes,int* current_indexes,vector<int>* one_indexes,vector<int>* zero_indexes,int flag_starting_from,int* place_0,int* place_1);
    void arrange_the_indexes(const Bitstring& BF,int Nbf,vector<int>* arr_indexes, int** indexes);
    void get_zeros_ones(BitVector* b, int Not, int Nc, vector<int>*& arr_indexes,crypto *crypt);
    void get_sub_group(int* ,int,int,block&);
    void compute_r(block& r,std::vector<std::array<block, 2>>& strings,int* indexes,int indexes_size); 
    void compute_R_r( int* sub_group,int* indexes,BitVector* b,int Ncc,int Not, std::vector<block>& strings, block& r, std::vector <int> &R);
    int if_disjoint_sets(int *,int, int *,int, int);
    unsigned int* get_seeds(int seeds_num, block seed);	
    block* re_randomization(vector <int>& items, block* Y, int n , int Nbf, int bytes , set<int>* h_kokhav ,crypto* crypt, Pi* pi, block* GBF, synchGBF* r);
    block* re_randomization_zero(vector <int>& items, block* Y, int n , int Nbf, int bytes , set<int>* h_kokhav ,crypto* crypt, Pi* pi, block* GBF, synchGBF* test);

    void offline_apport_receiver(Party* party,std::vector<std::string>* ips, int** ports,int player, int other_player,fstream* fout,std::mutex* mu);
    void offline_apport_sender(Party* party,std::vector<std::string>* ips, int** ports,int player,int other_player, fstream* fout,std::mutex* mu);
    void online_apport_sender(Party* party, synchGBF* r, fstream* fout,std::mutex* mu);
    void online_apport_receiver(Party* party, synchGBF* r, fstream* fout,std::mutex* mu);
    void run_online_apport(Party* p0, synchGBF* test,fstream* fout,std::mutex* mu );
    void run_offline_apport(Party* p0,std::vector <std::string>* ips,int** ports,int player,int other_player,fstream* fout,std::mutex* mu) ;

    void set_ports(int** ports,int player,int parties);
    void set_ips(std::vector <string>& ips,string file, int* parties);
    void read_protocol_parameters(const char* cfg_file,uint32_t* seeds,uint32_t* Nbf,uint64_t* Not,uint32_t* numOFOnes,uint32_t* Nc,uint32_t* Nr,string& data,string& ip);
    void get_values(string values_file,vector <int>& values);
	
    void seeds_agreement_pi(Pi* pi,int parties,Commit& commit,block& seed,int player,fstream* fout);  
    void seeds_pi_write(Pi* pi,Commit* commit,block* seed,int party,unsigned long* data,std::mutex* mu);
    void seeds_pi_read(Pi* pi,Commit* commit,block* seed,int party,unsigned long* data,std::mutex* mu);

    void seeds_agreement_p0(std::vector<P0*>& P0_s,Commit& commit,block& seed,fstream* fout);
    void seeds_p0_write(P0* p0,Commit* commit,block* seed,unsigned long* sum,std::mutex* mu);
    void seeds_p0_read(P0* p0,Commit* commit,block* seed,unsigned long* sum,std::mutex* mu);

    void set_seed_commit(Commit& commit, block& seed,crypto* crypt);

    void secret_sharing_seed_pi(Pi* pi,int parties,crypto* crypt,int player,block* keys,block* keys_recv,fstream* fout);
    void secret_sharing_aes(int parties,uint32_t Nbf,synchGBF* test,block* keys,block* keys_recv);
    void secret_write(Pi* pi,int other_player,block* keys,unsigned long* sum,std::mutex* mu);
    void secret_read(Pi* pi,int other_player,block* keys,unsigned long* sum,std::mutex* mu);

    void secret_sharing_seed_p0(vector <P0*>& p0_s,int parties,block* keys,block* keys_recv,fstream* fout); 
    void secret_write_p0(P0* p0,block* keys,unsigned long* sum,std::mutex* mu);
    void secret_read_p0(P0* p0,block* keys,unsigned long* sum,std::mutex* mu);
	
    std::chrono::steady_clock::time_point get_duration(std::chrono::steady_clock::time_point& start,fstream& fout);
	
    void get_intersection( std::set<int>* h_kokhav,block* GBF);
    block* get_Y(int items_size,Pi* pi,std::set<int>* h_kokhav, block* Y, synchGBF* test);
    block* check_the_item(set<int>& h_kokhav,block* GBF);
	
    block* compute_gbf(std::vector<P0*>& P0_s,int Nbf, synchGBF* test,fstream* fout);  	
    void get_gbf(P0* p0, synchGBF* test,unsigned long* data,int player);
    void comulative_gbf_pi(Pi* pi,int Nbf,block* GBF1,fstream* fout);
 	
    void creating_p0_instance(int party,int port,std::vector<P0*>* P0_s,uint32_t numOfOnes,uint32_t m_nSecParam, uint8_t* constSeed );

    void bf_thread(int seeds_num,std::set<int>* h_kokhav,Bitstring* BF,details::AES<details::AESTypes::NI>* aes,int start,int end,int encNum,std::vector <int>* items, int Nbf);
    void create_BF_threads(std::set<int>* h_kokhav,std::vector <int>* items, int len, Bitstring* BF, int Nbf,int seeds_num, unsigned int* seeds, block seed);

}

#endif
