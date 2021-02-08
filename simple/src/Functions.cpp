#include "Party.h"
#include "P0.h"
#include "Pi.h"
#include "Functions.h"

//TEST
//#include <mutex>          // std::mutex
//std::mutex mtx;           // mutex for critical section


/**
 * replacing between 2 integers
 *
 * @param i first item
 * @param j second item
*/

void swap_(int& i,int& j){
     int temp =i;
     i=j;
     j=temp;
}


/**
 * shuffling the indexes
 *
 * @param arr_indexes vector of indexes
 * @param rand array of random vals
*/


void shuffling(vector<int>* arr_indexes,unsigned int* rand){

     int i=((*arr_indexes).size()-1);

     //exchanging the first element with random element
     swap_((*arr_indexes)[0],(*arr_indexes)[(rand[i]%i)]);

     //going over the vector from the last item to the second item
     for (;i>0;i--){
         //exchanging the currnt item with a random items at places [0..current)
         swap_((*arr_indexes)[i],(*arr_indexes)[(rand[i]%i)]);
      }

     delete [] rand;
}


/**
 * placing the indexes of 0's and 1's into 2 vectors, and shuffling them
 * 
 * @param bitvector vector of bits
 * @param Not size of the bitvector
 * @param Nc the sub group size 
 * @param arr_indexes array for the 2 vectors- one for the 0's indexes and one one for the 1's indexes
 * @param crypt crypthographic var 
*/

void functions::get_zeros_ones(BitVector* bitvector, int Not, int Nc, vector<int>*& arr_indexes,crypto *crypt){

     //establishing 2 vectors of the 0's/1's indexes	
     arr_indexes=new vector<int>[2];
     for (int i=0;i<(Not-Nc);i++)
         if ((*bitvector)[i]==0) arr_indexes[0].push_back(i);
         else arr_indexes[1].push_back(i);
     delete bitvector;
	
     //getting random vals
     unsigned int* rand0=new unsigned int [arr_indexes[0].size()];  
     unsigned int* rand1=new unsigned int [arr_indexes[1].size()];  
 
     crypt->gen_rnd((BYTE*)rand0,4*arr_indexes[0].size());
     crypt->gen_rnd((BYTE*)rand1,4*arr_indexes[1].size());

     //shuffling the vectors
     thread* t1=new thread(shuffling,&arr_indexes[0],rand0);
     thread* t2=new thread(shuffling,&arr_indexes[1],rand1);  
     t1->join();
     t2->join();
     delete t1;
     delete t2;
}


/**
 * placing the indexes of 0's and 1's into 2 vectors, and shuffling them
 * 
 * @param BF Bloom filter
 * @param Nbf Bloom filter's size
 * @param new_indexes array for the new indexes
 * @param current_indexes array of the current indexes
 * @param one_indexes vector of the 1's indexes
 * @param zero_indexes vector of the 0's indexes
 * @param flag_starting_from 0-for starting from the beginning 1-for starting from the ending
 * @param place_0 the place of the current index int the 0's vector
 * @param place_1 the place of the current index int the 1's vector 
 */


void functions::arrange_indexes_thread(const Bitstring* BF,int Nbf,int* new_indexes,int* current_indexes,vector<int>* one_indexes,vector<int>* zero_indexes,int flag_starting_from,int* place_0,int* place_1){

     //starting from the beginning of the BF/vectors
     if (flag_starting_from==0){
	     
	//going over the first half of the BF
        for (int i=0;i<(Nbf/2);i++){
             try{
                 //getting the current bit and fetching the index which will represent it
                 if ((*BF).get_bit(i)==0)
                    new_indexes[i]=current_indexes[(*zero_indexes)[(*place_0)++]];
                 else
                    new_indexes[i]=current_indexes[(*one_indexes)[(*place_1)++]];
             }
             catch (const std::out_of_range& oor) {
		 cout<<"Not enough "<<(*BF).get_bit(i)<<"s - problem!"<<endl;
                 exit(1);
             }  
        }
        
     }
	
     //starting from the ending of the BF/vectors    
     else{
	   
	//going over the second half of the BF
        for (int i=(Nbf/2);i<Nbf;i++){
             try{   
		 //getting the current bit and fetching the index which will represent it   
                 if ((*BF).get_bit(i)==0)
                    new_indexes[i]=current_indexes[(*zero_indexes)[(*place_0)--]];       
                 else
                    new_indexes[i]=current_indexes[(*one_indexes)[(*place_1)--]];
             }
             catch(const std::out_of_range& oor) {
                 cout<<"Not enough "<<(*BF).get_bit(i)<<"s - problem!"<<endl;    
                 exit(1); 
             }   
        }
     }
}



/**
 * Ordering the strings indexes in this way-
 * if in the Bloom filter at place '0' there is the bit '1',
 * so, in the indexes array , at place '0', will be the index of some string which has bit '1'.
 *
 * @param BF Bloom filter
 * @param Nbf Bloom filter's size
 * @param arr_indexes vectors for the 0's indexes and for the 1's indexes
 * @param indexes Array of the indexes of the strings
 * @param Not the size of the bitset and the indexes
 * @param Ncc size of group C
 * @param crypt cryptographic var
 */

	
void functions::arrange_the_indexes(const Bitstring& BF,int Nbf,vector<int>* arr_indexes, int** indexes,int Not, int Ncc,crypto *crypt){
 
    //array for the new indexes
    int* indexes1=new int[Nbf];
 
    //the starting place for going over the vectors
    int place_0_begin=0;
    int place_0_end=arr_indexes[0].size()-1;
    int place_1_begin=0;
    int place_1_end=arr_indexes[1].size()-1;

    //building shuffled vectors of 0's and 1's inedexes
    thread* t1=new thread(functions::arrange_indexes_thread,&BF,Nbf,indexes1,*indexes,&arr_indexes[1],&arr_indexes[0],0,&place_0_begin,&place_1_begin);
    thread* t2=new thread(functions::arrange_indexes_thread,&BF,Nbf,indexes1,*indexes,&arr_indexes[1],&arr_indexes[0],1,&place_0_end,&place_1_end);

    t1->join();
    t2->join();
    delete t1;
    delete t2;

    //checking that there was enougth indexes for the 0's/1's
    if (((place_0_begin-1)>=(place_0_end+1)) || ((place_1_begin-1)>=(place_1_end+1))){
       cout<<"Not enough o's/1's - problem!"<<endl;
       exit(1);
   }

    delete [] arr_indexes;	
    delete [] (*indexes);	

    (*indexes)=indexes1;	

    //cout<<"arrange_the_indexes complete!"<<endl;
}


/**
 * Calculating hash seeds using AES function and our common seed as key
 *
 * @param seeds_num number of seed to be calculated
 * @param seed the commom seed which will be the key of the AES function
 * @return array of hash seeds
 */

unsigned int* functions::get_seeds(int seeds_num, block seed){
	
    unsigned int* seeds=new unsigned int[seeds_num];
    details::AES<details::AESTypes::NI> aes;
    aes.setKey(seed);
	
    block* plaintexts = new block[seeds_num];
    for (int i=0;i<seeds_num;i++)
    plaintexts[i]=toBlock(0,i);

    block* ciphertexts= new block[seeds_num];
	
    aes.ecbEncBlocks(plaintexts, seeds_num, ciphertexts);
	
    for (int i=0;i<seeds_num;i++)
	seeds[i]=(unsigned int)ciphertexts[i].as<std::uint64_t>()[0];
	
    delete [] ciphertexts;
    delete [] plaintexts;
    return seeds;
	
}

/**
 * the function get range of items, and put the result of activating the hush function in the BF
 *
 * @param seeds_num number of seeds
 * @param h_kokhav array of groups of indexes which represents the result of the hush functions for each item 
 * @param BF the bloom filter
 * @param AES the hush function
 * @param start the index of the first item
 * @param end the index of the last item
 * @param encNum half of the number of seeds
 * @param items array of items
 * @param Nbf the number of bits in the bloom filter

 */


void functions::bf_thread(int seeds_num, std::set<int>* h_kokhav,Bitstring* BF,details::AES<details::AESTypes::NI>* aes,int start,int end,int encNum,std::vector <int>* items, int Nbf){
    
    //storage for the random numbers	
    block* plaintexts = new block[encNum];
    block* ciphertexts= new block[encNum];

    //getting random numbers for each item and putting 1's in the BF
    for (int i=start;i<end;i++){
	 //getting random numbers
         for (int j=0;j<encNum;j++) plaintexts[j]=toBlock((*items)[i],0)^toBlock(0,j);
         (*aes).ecbEncBlocks(plaintexts, encNum, ciphertexts);
         //putting 1's in the right places in the BF
         for (int j=0;j<seeds_num/2;j++){
             uint64_t h1=(ciphertexts[j].as<std::uint64_t>()[0])%((uint64_t)Nbf);
             uint64_t h2=(ciphertexts[j].as<std::uint64_t>()[1])%((uint64_t)Nbf);
             BF->set_bit(h1,1);
             BF->set_bit(h2,1);
             h_kokhav[i].insert(h1);
             h_kokhav[i].insert(h2);
         }
         if ((seeds_num%2)==1){
	     uint64_t h1=(ciphertexts[seeds_num/2].as<std::uint64_t>()[0])%((uint64_t)Nbf);
	     BF->set_bit(h1,1);
             h_kokhav[i].insert(h1);
         }
     }

     delete [] plaintexts;
     delete [] ciphertexts;
}

/**
 * Creating a bloom filter by sending each range of items to a different thread
 *
 * @param h_kokhav array of groups of indexes which represents the result of the hush functions for each item 
 * @param items array of items
 * @param len length of the items
 * @param BF the bloom filter
 * @param Nbf the number of bits in the bloom filter
 * @param seeds_num number of seeds
 * @param seeds seeds
 * @param seed common_seed
 */

void functions::create_BF_threads(std::set<int>* h_kokhav,std::vector <int>* items, int len, Bitstring* BF, int Nbf,int seeds_num, unsigned int* seeds, block seed){
    //auto start = std::chrono::steady_clock::now();

    //creating AES object
    details::AES<details::AESTypes::NI>* aes=new details::AES<details::AESTypes::NI>();
    aes->setKey(seed);

    //calculating number of half seeds
    int encNum=seeds_num/2+(seeds_num%2);

    //auto med = std::chrono::steady_clock::now();
    //std::cout<<"BF consruction setup time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(med-start).count()<<std::endl;

    //separating the items to 36 blocks, and sending each block to a thread which will create the BF
    int begin=0;
    int jump=len/36;

    vector<thread*> threads;
    for (int i=0;i<36;i++){
        threads.push_back(new thread(functions::bf_thread,seeds_num,h_kokhav,BF,aes,begin,(begin+jump),encNum,items,Nbf));
        begin+=jump;
    }

    threads.push_back(new thread(functions::bf_thread,seeds_num,h_kokhav,BF,aes,begin,len,encNum,items,Nbf));

    for (auto& t:threads) t->join();
    for (auto& t:threads) delete t;
    threads.clear();

    delete aes;

    //auto end = std::chrono::steady_clock::now();
    //std::cout<<"BF construction online time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()<<std::endl;

}

	   
/**
 * Creating a sub group of indexes
 *
 * @param sub_group A pinter to the sub group where the random numbers will be stored
 * @param big_group_size The range of the random numbers we can get [0,..,big_group_size)
 * @param small_group_size The number of random vals we need
 * @param seed key for using the AES function
 */
	   

void functions::get_sub_group(int sub_group[],int big_group_size,int small_group_size,block& seed){
	
        //getting random vals using our seed as key, and numbers from 0 to small_group_size(Nc)/2 as messages and activating on it the AES function
        //we use numbers from 0 to small_group_size/2, because each block object which we are getting from the AES function includes 2 numbers of size 64 bit - which is big enougth
	
	//std::cout<<"creating rands vals"<<std::endl;
	int size=(int)((small_group_size/2)*1.1);
	if ((small_group_size%2)==1) size++;
	//std::cout<<"size: "<<size;
   	block* ciphertexts=new block[size];
	block* plaintexts=new block[size];

	for ( int i=0;i<size;i++){
	     plaintexts[i]=toBlock(0,(unsigned int)i);
	}

	details::AES<details::AESTypes::NI> *aes=new details::AES<details::AESTypes::NI>();	
	aes->setKey(seed);
	aes->ecbEncBlocks(plaintexts, size, ciphertexts);
	
	delete [] plaintexts;
	delete aes;

        //we need now to get numbers which are smaller than big_group_size(Not)
	//Accordingly, we will activate modulo Not on each number we got from the AES function
        //std::cout<<"getting random vals"<<std::endl; 
		 
        std::set<unsigned int>* s=new set<unsigned int>();

	int i=0; 
        int j=0;  
		 
        while((int)(*s).size()!=small_group_size){
                   (*s).insert((unsigned int)(ciphertexts[j].as<std::uint64_t>()[i])%big_group_size);
                   i^=1;
                   if (i==0) j++;
                   if (j==size) std::cout<<"out of randoms"<<std::endl;      
        }                                                                                                                                                                                                                                                                                                                                                                                 delete [] ciphertexts;
       	 
        //std::cout<<"setting the vals to the sub group"<<std::endl;   
        i=0;
	for (auto item:(*s)){
		   sub_group[i++]=item;    
	}
		
        delete s;  		 

}


/**
 * computing r*
 *
 * @param r xor of all the strings in the places of the received indexes
 * @param strings The string's array
 * @param indexes Array of indexes of the 0'S bit strings in group C
 * @param indexes_size The size of the index's array
 */

void functions::compute_r(block& r,std::vector<std::array<block, 2>>& strings,int* indexes,int indexes_size){
    r=toBlock(0,0);
    r=strings[indexes[0]][0];
	
    for (int i=1;i<indexes_size;i++){
	 r=r^strings[indexes[i]][0];		
    }	
}


/**
 * Finding the indexes of the '0' bit string in the chosen sub group and xoring these strings
 *
 * @param sub_group Sub_group of group {0,..,Not}
 * @param indexes Group of indexes {0,..,Not} indexes[0]=0, etc ..
 * @param b The bit string which is corresponding to the indexes array . 
 *          Every change in indexes array will cause to the same change in the bits array.
 * @param Ncc the size of the sub group
 * @param Not the numOTs
 * @param strings An array of the strings
 * @param r pointer to the result of tho 0's strings xor
 * @param R the vector for the indexes of the 0's strings 
**/

bool compare_func(int x,int y){
     return (x>y);
}

void functions::compute_R_r(  int* sub_group,int* indexes,BitVector* b,int Ncc,int Not, std::vector<block>& strings, block& r, std::vector <int> &R){	
   //setting 0 to r
   r=toBlock(0,0);
   int ind=Not-1;
   
   //sorting sub_group from big to small
   
   //because in the next step,
   //we will move all the indexes of the sub group to the end of the indexes array
   //we will move the biggest index first and so on-
   //to ensure that the index and its place are the same
   
   //by removing the indexes to the end, we will know not to use them when we will build the BF
   
   std::sort(sub_group,sub_group+Ncc,compare_func); 
     
   // go over the sub goup
   for (int i=0;i<Ncc;i++){
       //getting the index
       int target=sub_group[i];
       //removing the sub group indexes to the end of the index array (and changing the bits array accordingly)			
       int val=(*b)[target];
       int temp=indexes[target];
       indexes[target]=indexes[ind];
       indexes[ind]=temp;
       temp=(*b)[target];
       (*b)[target]=(*b)[ind];
       (*b)[ind]=temp;
       ind-=1;
		
       //searching for the indexes of the 0's strings
       if (val==0){
          //adding to R group
          R.push_back(sub_group[i]);
          //xoring to r
	  r=r^strings[sub_group[i]];
        }
    }
    //cout<<"Size of R = "<<R.size()<<endl;
    //cout <<"Ncc - R ="<<Ncc-R.size()<<endl;
}


/**
 * checking if the 2 sets are disjoint
 *
 * @param cc Group 1 
 * @param Ncc Size of group 1
 * @param indexes Group 2
 * @param Nbf Size of group 2
 * @param Not number of OT's
 * @return 0 if the groups are disjoint, 1 otherwise.
**/
 
int functions::if_disjoint_sets(int* cc,int Ncc, int* indexes,int Nbf,int Not){

     Bitstring* counter=new Bitstring(Not);

     //putting 1 in the places of the elements of group 1
     for (int i=0; i<Ncc; i++)
	(*counter).set_bit(cc[i],1);

     //checking if one of the elements in group 2 is in group 1
     for (int i=0; i<Nbf; i++)
	if ((*counter).get_bit(indexes[i])==1){
	   delete counter;
	   return 0;
	}

     delete counter;
     return 1;
}


/**
 * filling the GBF
 *
 * @param items the items
 * @param Y pointer to array of Y
 * @param n number of items
 * @param h_kokhav h_kokhav of all the items
 * @param crypt crypto var to generate random strings
 * @param pi instance of pi
 * @param GBF array in the size of Nbf
 * @param test
**/

block* functions::re_randomization_zero(vector <int>& items, block* Y, int n , int Nbf, int bytes , set<int>* h_kokhav ,crypto* crypt, Pi* pi, block* GBF, synchGBF* test){
    //auto start = std::chrono::steady_clock::now();  

    //creating Y's array
    Y=new block[items.size()];

    //auto med = std::chrono::steady_clock::now();    
    //std::cout<<"Get_Y time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(med-start).count()<<std::endl;         

    //setting 0's to Y's array
    for (int i=0;i<(int)items.size();i++)
        Y[i]=toBlock(0, 0);

   //setting 0's to GBF's array
    for (int i=0;i<Nbf;i++)            
        GBF[i]=toBlock(0,0);

    block block0=toBlock(0,0);

    //going over all the items in the DB and filling the GBF
    //std::cout<<"rerandomizing"<<std::endl; 
    for (int i=0;i<n;i++){
        int finalInd=-1;
        block finalShare=Y[i];
        set<int>::iterator begin=h_kokhav[i].begin();
        set<int>::iterator end=h_kokhav[i].end();

	//going over the indexes of h* of item i
        for (;begin!=end;begin++){ 
            int j=(*begin);
            //if the cell is empty
            if (eq(GBF[j],block0)){
                if (finalInd==-1)  
                    finalInd=j;
                else{
                    //setting random val
                    unsigned long* x1=new unsigned long;
                    unsigned long* x2=new unsigned long;
                    crypt->gen_rnd((BYTE*)x1,8);
                    crypt->gen_rnd((BYTE*)x2,8);
                    GBF[j]=block(*x1,*x2);
                    finalShare=finalShare^GBF[j];
                    delete x1;
                    delete x2;
                }
           }
           //otherwise
           else{
                finalShare=finalShare^GBF[j];
           }
        }
        if  (finalInd==-1){
           cout<<finalInd<<" is out of bound";
        }
        else{
           GBF[finalInd]=finalShare;
        }
    }

    //auto med2 = std::chrono::steady_clock::now();
    //std::cout<<"GBF data construction time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(med2-med).count()<<std::endl;

    //filling the cells which remain empty
    for (int i=0; i<Nbf;i++)
       if (eq(GBF[i],block0)){
           unsigned long* x1=new unsigned long;
           unsigned long* x2=new unsigned long;
           crypt->gen_rnd((BYTE*)x1,8);
           crypt->gen_rnd((BYTE*)x2,8);
           GBF[i]=block(*x1,*x2);
           delete x1;
           delete x2;
       }

    //auto end=std::chrono::steady_clock::now();
    //std::cout<<"GBF putting random strings time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-med).count()<<std::endl;

    delete [] Y;
    return GBF;
}


/**
 * filling the GBF
 *
 * @param items the items
 * @param Y pointer to array of Y
 * @param n number of items
 * @param h_kokhav h_kokhav of all the items
 * @param crypt crypto var to generate random strings
 * @param pi instance of pi
 * @param GBF array in the size of Nbf with nullptr's
 * @param test
**/

block* functions::re_randomization(vector <int>& items, block* Y, int n , int Nbf, int bytes , set<int>* h_kokhav ,crypto* crypt, Pi* pi, block* GBF, synchGBF* test){
    //auto start = std::chrono::steady_clock::now();
    
    //setting the Y's array
    //std::cout<<"geting_Y"<<std::endl;
    Y=get_Y(n, pi , h_kokhav, Y, test);

    //auto med = std::chrono::steady_clock::now();
    //std::cout<<"Get_Y time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(med-start).count()<<std::endl;
    
    //setting 0's to GBF's array
    for (int i=0;i<Nbf;i++) 
         GBF[i]=toBlock(0,0);

    block block0=toBlock(0,0);
	
    //going over all the items in the DB and filling the GBF
    //std::cout<<"rerandomizing"<<std::endl;
    for (int i=0;i<n;i++){
		
        int finalInd=-1;
        block finalShare=Y[i];

        set<int>::iterator begin=h_kokhav[i].begin();
        set<int>::iterator end=h_kokhav[i].end();
	    
	//going over the indexes of h* of item i
        for (;begin!=end;begin++){		
	    int j=(*begin);
		
            //if the cell is empty
            if (eq(GBF[j],block0)){
	        if (finalInd==-1) 
	            finalInd=j;
	        else{
                    //setting random val

	            unsigned long* x1=new unsigned long;
	            unsigned long* x2=new unsigned long;
	            crypt->gen_rnd((BYTE*)x1,8); 
	            crypt->gen_rnd((BYTE*)x2,8);
	            GBF[j]=block(*x1,*x2);
	            finalShare=finalShare^GBF[j];
		    delete x1;
		    delete x2;
	        }
	    }
			 
	    else{
	        finalShare=finalShare^GBF[j];
	    }
        }

        if  (finalInd==-1){
	    cout<<finalInd<<" is out of bound";
        }
        else{
            GBF[finalInd]=finalShare;
        } 
    }

    //auto med2 = std::chrono::steady_clock::now();
    //std::cout<<"GBF data construction time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(med2-med).count()<<std::endl;
    
	
    //filling the cells which remain empty
    for (int i=0; i<Nbf;i++)
	if (eq(GBF[i],block0)){
	    unsigned long* x1=new unsigned long;
	    unsigned long* x2=new unsigned long;
	    crypt->gen_rnd((BYTE*)x1,8);
	    crypt->gen_rnd((BYTE*)x2,8);
	    GBF[i]=block(*x1,*x2);
	    delete x1;
	    delete x2;
	}
    //auto end = std::chrono::steady_clock::now();
    //std::cout<<"GBF putting random strings time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-med).count()<<std::endl;
	
    delete [] Y;
    return GBF;
}


/**
 * checking if the item is in the item's list
 *
 * @param h_kokhav h*(item x)
 * @param GBF
**/

block* functions::check_the_item(set<int>& h_kokhav,int bytes, int Nbf,block* GBF){
	
    block* y=new block(0,0);

    //going over all the indexes in h* and xoring them
    set<int>::iterator begin=h_kokhav.begin();
    set<int>::iterator end=h_kokhav.end();
    for (;begin!=end;begin++){
	    (*y)=(*y)^GBF[*begin];
    }
	
    return y;
}


/**
 * performing offline apport as a receiver
 *
 * @param Party pointer to the party object
 * @param ips vector of the parties' ips
 * @param ports vector of the parties' ports
 * @param player the player's index
 * @param other_player the other player's index
**/

void functions::offline_apport_receiver(Party* party,std::vector<std::string>* ips, int** ports,int player, int other_player,fstream* fout,std::mutex* mu){

   //std::cout<<"starting offline apport receiver"<<std::endl;	
   
   string str=(*ips)[other_player];
   str.append(":");
   if (player==0)
       str.append( std::to_string(7000+other_player*2));
   else
       str.append( std::to_string(7000+player*2+1));

   //performing OT's
   TwoChooseOne(Role::Receiver,1, str,party);

   if (player==0)
       party->init_receiver((*ips)[other_player].c_str(),ports[other_player][1]);

   //receiving the sub group
   party->recv_C();

   //checking if the size of the sub group is OK
   if (party->check_if_to_abort()==0){

       //computing the R group and r* and sending them
       party->compute_R_r();
       party->send_R_r();
       party->get_zeros_ones();

   }

   else {
       std::cout<<"aborting"<<endl;exit(1);
   }

}


/**
 * performing offline apport as a sender
 *
 * @param Party pointer to the party object
 * @param nsndvals parameter for the OT protocol
 * @param stype parameter for the OT protocol
 * @param rtype parameter for the OT protocol
 * @param crypt_ parameter for the OT protocol
**/

void functions::offline_apport_sender(Party* party,std::vector<std::string>* ips, int** ports,int player,int other_player, fstream* fout,std::mutex* mu){

    //std::cout<<"starting offline apport sender"<<std::endl;
	
    string str1=(*ips)[player];
    //cout<<str1;
    str1.append(":");
    if (player==0)
	str1.append( std::to_string(7000+other_player*2+1));
    else
	str1.append( std::to_string(7000+player*2));
	
    //performing OT's
    TwoChooseOne(Role::Sender,1, str1,party);

    if (player!=0) party->init_sender(ports[other_player][0]);

    //party sets sub group
    party->set_sub_group();

    //party sends the sub group
    party->send_sub_group();

    //party receives r* and R
    party->recv_R();

    //party checks if to abort because its r* and the r* of the othe party don't match
    if (party->check_if_to_abort1()!=0) {std::cout<<"Aborting now";exit(1);}//Changed

}


/**
 * performing online apport as a receiver- 
 * creating function according to the BF bits and sending it to the other party
 *
 * @param Party pointer to the party object
**/

void functions::online_apport_receiver(Party* party, synchGBF* test, fstream* fout,std::mutex* mu){

    //party arranging the indexes of the initial bitstring accordingly to the bloom filter bits
    party->arrange_the_indexes();

    //party sends the injective func
    party->send_func();

    //xoring the GBF
    party->xor_RBF_receiver(test);
}


/**
 * performing online apport as a sender-
 * receiving the function from the other party
 *
 * @param Party pointer to the party object
**/

void functions::online_apport_sender(Party* party, synchGBF* test,fstream* fout,std::mutex* mu){
	
    //party receives the injective func and buildin
    party->recv_func();
	
    //xoring the GBF
    party->xor_RBF_sender(test);
}


/**
 * running online apport as a sender and as a receiver
 *
 * @param Party pointer to the party object
**/

void functions::run_online_apport(Party* p0, synchGBF* test,fstream* fout,std::mutex* mu ){

    //online apport receiver
    thread* t1=new thread(functions::online_apport_receiver,p0, test,fout,mu);
    //online apport sender
    thread* t2=new thread(functions::online_apport_sender,p0, test,fout,mu);	
	
    t1->join();
    t2->join();
	
    delete t1;
    delete t2;	
}


/**
 * running offline apport as a sender and as a receiver
 *
 * @param Party pointer to the party object
 * @param ips vector to the ips of all the parties
 * @param ports vector to the ports of all the parties
 * @param player index of the current player
 * @param other_player index to the player we are going to play against
**/


void functions::run_offline_apport(Party* p0,std::vector <std::string>* ips,int** ports,int player,int other_player,fstream* fout,std::mutex* mu){			

    //running offline apport receiver    
    thread* t1=new thread(functions::offline_apport_receiver,p0,ips,ports,player,other_player,fout,mu);
    //running offline apport sender
    thread* t2=new thread(functions::offline_apport_sender,p0,ips,ports,player,other_player,fout,mu);
	
    t1->join();
    t2->join();
	
    delete t1;
    delete t2;

}


/**
 * setting ports to connect with the other players
 *
 * @param ports pointer to the ports matrix
 * @param player the player number
 * @param parties the number of parties
**/	

void functions::set_ports(int** ports,int player,int parties){
    for (int i=0;i<parties;i++){
	ports[i]=new int[2];
	ports[i][0]=8000 + i*parties + player;
        ports[i][1]=8000 + player*parties + i;
    }
}


/**
 * getting the ip's of all the players
 *
 * @param ips vector of the ips
 * @param file the name of the ips text file
 * @param parties pointer to the number of parties
**/	

void functions::set_ips(std::vector <string>& ips,string file, int* parties){
    string line;
    ifstream myfile(file);
       
    if (myfile.is_open()){
        while (getline(myfile,line)){
	    line.resize(line.size()-1);
            ips.push_back(line);
        }
    }

    *parties=ips.size();
    //cout<<"parties:"<<*parties<<endl;
    /*for (int i=0;i<*parties;i++){
	cout<<ips[i]<<endl;
    }*/	
}


/**
 * getting the protocol's parameterd from configuration file
 *
 * @param cfg_file the name of the cfg file
 * @param pointer to the seeds number of seeds
 * @param Nbf pointer to the size of the BF
 * @param Not pointer to the number of OT's
 * @param numOFOnes pointer to the number of ones in the bitsring
 * @param Nc pointer to the size of the sub group
 * @param Nr pointer to the maximum number of 1's in the sub group
 * @param data name of the DB file
 * @param data name of the ip file 
**/	

void functions::read_protocol_parameters(const char* cfg_file,uint32_t* seeds,uint32_t* Nbf,uint64_t* Not,uint32_t* numOFOnes,uint32_t* Nc,uint32_t* Nr,string& data,string& ip){

    ifstream cFile (cfg_file);
    string values[8];
    int counter=0;
    if (cFile.is_open()){
	 string line;
	 while(getline(cFile, line)){
             auto delimiterPos = line.find("=");
             auto name = line.substr(0, delimiterPos);
             values[counter++] = line.substr(delimiterPos + 1);
             //cout << name << ": " << values[counter-1] << '\n';
        }			
    }

    *seeds=stoi(values[0]);
    *Nbf=stoi(values[1]);
    *Not=stoi(values[2]);
    *numOFOnes=stoi(values[3]);
    *Nc=stoi(values[4]);
    *Nr=stoi(values[5]);
    data=values[6];
    ip=values[7];
	
}


/**
 * getting the DB of the party from the data file (comma separated)
 *
 * @param values_file the name of the values file
 * @param values vector for the values
**/	

void functions::get_values(string values_file,vector <int>& values){

    ifstream myfile(values_file);
    string intermediate;
	
    while(getline(myfile, intermediate, ',')) 
        values.push_back(stoi(intermediate));   
}


/**
 * agreement phase of pi - sending and receiving commit+key , checking if the received objects are ok , ans than computing common seed
 *
 * @param pi pointer to the player
 * @param parties num of players
 * @param commit the commitment
 * @param seed thes seed
 * @param player the number of the current player
 * @param fout file for writing the amount of transfered data
**/	


void functions::seeds_agreement_pi(Pi* pi,int parties,Commit& commit,block& seed,int player,fstream* fout){
	
    //receiveng and sending seed and commit to each party
    Commit* commits_recv=new Commit[parties-1];
    block* seeds_recv=new block[parties-1];	
	
    vector<thread*> threads;
	
    unsigned long data_recv=0;
    unsigned long data_send=0;
    std::mutex mu_recv;
    std::mutex mu_send;  
    for (int p=0;p<parties;p++){
	if (p==player) continue;
	int place=p;
	if (player<p) place=p-1;
    	threads.push_back(new thread(functions::seeds_pi_write,pi,&commit,&seed,p,&data_send,&mu_send));	
        threads.push_back(new thread(functions::seeds_pi_read,pi,&commits_recv[place],&seeds_recv[place],p,&data_recv,&mu_recv));		
    }
	
    for (auto& t:threads) t->join();
    for (auto& t:threads) delete t;
    threads.clear();

    (*fout)<<"sending seed+commit to the other parties,"<<data_send<<"\n";
    (*fout)<<"receiving seed+commit to the other parties,"<<data_recv<<"\n";       
			
    int flag=1;
    int j=0;
			
    //checking if the received pair of seed-commit from each party is OK
    while (flag && (j<(parties-1))){
	if (commits_recv[j]!=Commit(seeds_recv[j])) flag=0;
        j++;
    }				
			
    if (flag==0) cout<<"commit is not ok"<<endl;
			
    for (int p=0;p<parties-1;p++){
	seed=seed^seeds_recv[p];
    }
			
    //cout<<"common seed:"<<seed<<endl;
			
    delete [] commits_recv;
    delete [] seeds_recv;
}


//helper function
void functions::seeds_pi_write(Pi* pi,Commit* commit,block* seed,int party,unsigned long* data,std::mutex* mu){
    unsigned long sum=pi->write_to_player(party,commit,sizeof(Commit));
    sum+=pi->write_to_player(party,seed,sizeof(block));
    mu->lock();
    (*data)+=sum;
    mu->unlock();
}

//helper function
void functions::seeds_pi_read(Pi* pi,Commit* commit,block* seed,int party,unsigned long* data,std::mutex* mu){
    unsigned long sum=pi->read_from_player(party,commit,sizeof(Commit));
    sum+=pi->read_from_player(party,seed,sizeof(block));
    mu->lock();
    (*data)+=sum; 
    mu->unlock();  
}


/**
 * agreement phase of p0 - sending and receiving commit+key , checking if the received objects are ok , ans than computing common seed
 *
 * @param P0_s vector of pointers to the P0's objects 
 * @param commit the commitment
 * @param seed thes seed
 * @param fout file for writing the amount of transfered data
**/	


void functions::seeds_agreement_p0(std::vector<P0*>& P0_s,Commit& commit,block& seed,fstream* fout){

    //arrays for the commits and seeds which will be received
    Commit* commits_recv=new Commit[P0_s.size()];
    block* seeds_recv=new block[P0_s.size()];
			
    vector<thread*> threads;
	
    //sending P0's seed and commit, and receiving it from the other parties
    unsigned long bytes_rec=0;
    unsigned long bytes_send=0;	
    std::mutex m_rec;
    std::mutex m_send;
    for (int p=0;p<(int)P0_s.size();p++){
         threads.push_back(new thread(functions::seeds_p0_write,P0_s[p],&commit,&seed,&bytes_send,&m_send));
	 threads.push_back(new thread(functions::seeds_p0_read,P0_s[p],&commits_recv[p],&seeds_recv[p],&bytes_rec,&m_rec));
    }
	
    for (auto& t:threads) t->join();
    for (auto& t:threads) delete t;
    threads.clear();

    (*fout)<<"sending seed+commit to the other parties,"<<bytes_send<<"\n";   
    (*fout)<<"receiving seed+commit from the other parties,"<<bytes_rec<<"\n"; 
	
    //checking that the commits and seeds which has been received are OK
    int flag=1;
    int j=0;
    while (flag && (j<(int)P0_s.size())){
	if (commits_recv[j]!=Commit(seeds_recv[j])) flag=0;
	j++;
    }
	
    if (flag==0) cout<<"commit is not ok"<<endl;
			
    for (int i=0;i<(int)P0_s.size();i++){
	seed=seed^seeds_recv[i];
    }
			
    //cout<<"common seed:"<<seed<<endl;
			
    delete [] commits_recv;
    delete [] seeds_recv;

}

//helper function for sending the seed an commit to the other party
void functions::seeds_p0_write(P0* p0,Commit* commit,block* seed,unsigned long* sum,std::mutex* mu){
    unsigned long sum1=p0->write_as_a_sender(commit,sizeof(Commit));
    unsigned long sum2=p0->write_as_a_sender(seed,sizeof(block));
    mu->lock();
    (*sum)=(*sum)+sum1+sum2;
    mu->unlock();
}

//helper function for receiving the seed and commit from the other party
void functions::seeds_p0_read(P0* p0,Commit* commit,block* seed,unsigned long* sum,std::mutex* mu){
    unsigned long sum1=p0->read_as_a_sender(commit,sizeof(Commit));
    unsigned long sum2=p0->read_as_a_sender(seed,sizeof(block));
    mu->lock();
    (*sum)=(*sum)+sum1+sum2;
    mu->unlock();
}


/**
 * creating seed using Crypto var and creating commit using the seed
 *
 * @param commit reference to the commit object
 * @param seed reference to the seed object
 * @param crypt pointer to crypto object
**/	


void functions::set_seed_commit(Commit& commit, block& seed,crypto* crypt){
    //creating seed 
    unsigned long x1;
    unsigned long x2;
    crypt->gen_rnd((BYTE*)&x1,8);
    crypt->gen_rnd((BYTE*)&x2,8);
    seed=toBlock(x1,x2);
    //creating commit
    commit=Commit(seed);
}

//##############

/**
 * secret sharing - each pi sends and receives key from the other parties. The result is xored secret.
 *
 * @param pi pointer to the current player
 * @param parties number of parties
 * @param crypt pointer to crypto object
 * @param player number of the current player
 * @param keys storage for the keys
 * @param keys_recv storage for the keys which will be received
 * @param fout file for writing the amount of transfered data
**/


void functions::secret_sharing_seed_pi(Pi* pi,int parties,crypto* crypt,int player,block* keys,block* keys_recv,fstream* fout){		

	unsigned long x1;
        unsigned long x2;
		
        //generating keys
        for (int i=0;i<(parties-1);i++){
            crypt->gen_rnd((BYTE*)&x1,8);
            crypt->gen_rnd((BYTE*)&x2,8);
            keys[i]=toBlock(x1,x2);
	}
	
	vector<thread*> threads;
	
        //sending and receiving the keys
	
        unsigned long sum_recv=0;
        unsigned long sum_send=0;
        std::mutex mu_recv;
        std::mutex mu_send;     

        for (int p=0;p<parties;p++){
	    if (p==player) continue;
	    int place=p;
	    if (p>player) place=p-1;
	    threads.push_back(new thread(functions::secret_read,pi,p,&keys_recv[place],&sum_recv,&mu_recv));
	    threads.push_back(new thread(functions::secret_write,pi,p,&keys[place],&sum_send,&mu_send));					
	}

  	
        for (auto& t:threads) t->join();
	for (auto& t:threads) delete t;
	threads.clear();

        (*fout)<<"secret sharing: sending keys,"<<sum_send<<"\n";    
        (*fout)<<"secret sharing: receiving keys,"<<sum_recv<<"\n";   		
}


//helper function
void functions::secret_write(Pi* pi,int other_player,block* keys,unsigned long* sum,std::mutex* mu){
    unsigned long sum1=pi->write_to_player(other_player,keys,sizeof(block));
    //cout<<"write to "<<other_player<<" "<<*keys<<" "<<"bytes:"<<sum1<<" "<<endl;
    mu->lock();  
    (*sum)=(*sum)+sum1; 
    mu->unlock();   
}

//helper function
void functions::secret_read(Pi* pi,int other_player,block* keys,unsigned long* sum,std::mutex* mu){
    unsigned long sum1=pi->read_from_player(other_player,keys,sizeof(block));
    //cout<<"read from "<<other_player<<" "<<*keys<<" "<<"bytes:"<<sum1<<" "<<endl;
    mu->lock();  
    (*sum)=(*sum)+sum1;  
    mu->unlock();    
}


/**
 * secret sharing- p0 generates keys using the crypto var
 * After he has finished to generate the keys, he gets/sends the keys from/to the rest of the parties

 * @param p0_s vector of P0's instances
 * @param parties number of parties
 * @param keys array of keys
 * @param keys_recv array of keys_recv
 * @param fout file for writing the amount of transfered data
**/

void functions::secret_sharing_seed_p0(vector <P0*>& p0_s,int parties,block* keys,block* keys_recv,fstream* fout){
	  
        unsigned long x1;
        unsigned long x2;
		
        //generating keys
        for (int i=0;i<(parties-1);i++){

            crypto* crypt=p0_s[i]->getCrypto();
            crypt->gen_rnd((BYTE*)&x1,8);
            crypt->gen_rnd((BYTE*)&x2,8);
            keys[i]=toBlock(x1,x2);
	}
	
	vector<thread*> threads;
	
        //sending and receiving the keys
        unsigned long sum_rec=0;
        unsigned long sum_send=0;
        std::mutex mu_rec;
        std::mutex mu_send;	

        for (int p=0;p<parties-1;p++){
	    threads.push_back(new thread(functions::secret_read_p0,p0_s[p],&keys_recv[p],&sum_rec,&mu_rec));
	    threads.push_back(new thread(functions::secret_write_p0,p0_s[p],&keys[p],&sum_send,&mu_send));					
	}

        for (auto& t:threads) t->join();
	for (auto& t:threads) delete t;
	threads.clear();

        (*fout)<<"secret sharing- sending keys,"<<sum_send<<"\n"<<"secret sharing- receiving keys,"<<sum_rec<<"\n";  	
}

//helper function to send the key
void functions::secret_write_p0(P0* p0,block* keys,unsigned long* sum,std::mutex* mu){
    unsigned long sum1=p0->write_as_a_sender(keys,sizeof(block));
    //cout<<"sending block:"<<(*keys)<<" "<<"bytes:"<<sum1<<" "<<endl;
    mu->lock();
    (*sum)=(*sum)+sum1;
    mu->unlock();
}

//helper function to receive the key
void functions::secret_read_p0(P0* p0,block* keys,unsigned long* sum,std::mutex* mu){
    unsigned long sum1=p0->read_as_a_sender(keys,sizeof(block));
    //cout<<"receiving block:"<<(*keys)<<" "<<"bytes:"<<sum1<<" "<<endl;
    mu->lock(); 
    (*sum)=(*sum)+sum1;
    mu->unlock();
}

//##############

/** p0 calculates the secret using AES and keys from earlier phase
  * for each key- takes the key and each "message" from 0 to Nbf and activates AES function
  * After finishing, all the results are xored
  *
  * @param parties number of parties
  * @param Nbf size of bf
  * @param test pointer to the secret 
  * @param keys p0's keys
  * @param keys_recv the other parties keys
**/
  
void functions::secret_sharing_aes(int parties,uint32_t Nbf,synchGBF* test,block* keys,block* keys_recv){		
     //auto start = std::chrono::steady_clock::now();
     block* r=test->getGBF();
	
     //creating objects for activate aes
     details::AES<details::AESTypes::NI> *aes=new details::AES<details::AESTypes::NI>();
		
     block* ciphertexts=new block[Nbf];
     block* plaintexts=new block[Nbf];

     //creating the "mesages"	
     for (unsigned int i=0;i<Nbf;i++){
	 plaintexts[i]=toBlock(0,i);
	 r[i]=toBlock(0,0);
     }

    //auto med = std::chrono::steady_clock::now();
    //std::cout<<"Secret-sharing setup time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(med-start).count()<<std::endl;
	
    //activating AES and xoring the secrets
    for (int p=0;p<(parties-1);p++) {
	    aes->setKey(keys[p]);
	    aes->ecbEncBlocks(plaintexts, Nbf, ciphertexts);
	    for (unsigned int i=0;i<Nbf;i++) r[i]=r[i]^ciphertexts[i];
	    aes->setKey(keys_recv[p]);
	    aes->ecbEncBlocks(plaintexts, Nbf, ciphertexts);	
	    for (unsigned int i=0;i<Nbf;i++) r[i]=r[i]^ciphertexts[i];
    }	

    //auto end = std::chrono::steady_clock::now();
    //std::cout<<"Secret-sharing time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-med).count()<<std::endl;	

    //freeing memory
    delete [] keys;
    delete [] keys_recv;
    delete [] plaintexts;
    delete [] ciphertexts;
    delete aes;
	      	
}


/**
 * writing the duration of the step
 *
 * @param start the start point
 * @param fout stream to write to
**/

std::chrono::steady_clock::time_point functions::get_duration(std::chrono::steady_clock::time_point& start,fstream& fout){
    auto end=std::chrono::steady_clock::now();
    fout<< std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()<<",";
    return end;
}


/**
 * receiving GBF1_STAR from the opponent players and xoring the with the shared GBF
 *
 * @param p0 instant of P0
 * @pararm test shared GBF
**/

void functions::get_gbf(P0* p0, synchGBF* test,unsigned long* data,int player) {

    int chunk_size=test->getChunkSize();
    block* chunkArr=new block[chunk_size];
    int n=P0::Nbf/chunk_size;
    unsigned long sum=0;

    //getting the GBF from Pi and xoring it with the shared GBF
    for (int i=0;i<n;i++){
       sum+= p0->read_as_a_receiver(chunkArr,sizeof(block)*chunk_size);
       test->XORchunk(chunkArr,i);
    }
	
    if (n*chunk_size<P0::Nbf){
        sum+=p0->read_as_a_receiver(chunkArr,sizeof(block)*(P0::Nbf-n*chunk_size));
        test->XORchunk_limited(chunkArr,n,P0::Nbf-n*chunk_size);
    }
	
    data[player]=sum;   
}


/**
 * COMPUTING P0's GBF
 *
 * @param P0_s vector of the p0's instances
 * @param Nbf size of the GBF
 * @param test shared GBF
 
**/

block* functions::compute_gbf(std::vector<P0*>& P0_s,int Nbf, synchGBF* test,fstream* fout){
	
   //getting GBF from the other parties and xoring them
   vector <thread*> threads;
   unsigned long* data=new unsigned long[P0_s.size()];  
   
   for (int i=0;i<(int)P0_s.size();i++){
        threads.push_back(new thread(functions::get_gbf,P0_s[i],test,data,i));	
   }
   for (auto& t:threads) t->join();
   for (auto& t:threads) delete t;
   threads.clear();

   for (int i=0;i<(int)P0_s.size();i++){        
        (*fout)<<"receiving gbf from player number "<<(i+1)<<","<<data[i]<<'\n';
   }
   delete [] data;    
   return test->getGBF();
}
	

/**
 * going over the indexes of each h*, xoring them, and checking if the sum is 0. If it is-print the item.
 *
 * @param h* vector of h*
 * @param bytes number of bytes in the string
 * @param GBF the GBF
**/

void functions::get_intersection( std::set<int>* h_kokhav,int bytes,block* GBF){
	
   cout<<"The intersection: ";
   for (int t=0;t<P0::items_size;t++){
	block* y=functions::check_the_item(h_kokhav[t],bytes, P0::Nbf,GBF);
	if ((*y)==toBlock(0,0)) cout<<(*P0::items)[t]<<" ";
	delete y;
   }
   std::cout <<std::endl;
   delete [] GBF;
}


/**
 * computing codewords
 *
 * @param items_size size of the data
 * @param pi pointer to the current player
 * @param h_kokhav
 
**/

block* functions::get_Y(int items_size,Pi* pi,std::set<int>* h_kokhav, block* Y, synchGBF* test){	
				
    for (int i=0;i<items_size;i++)
         Y[i]=toBlock(0, 0);
  
    block* M1=test->getGBF();

    //xoring 
    for(int i=0;i<items_size;i++)
       for (int j:h_kokhav[i])			
	   Y[i]=Y[i]^M1[j];
		   
    return Y;
}


/**
 * sending the GBF of pi to p0
 *
 * @param pi pointer to the player
 * @param Nbf size of GBF
 * @param GBF1 the GBF
 * @param fout file for writing the amount of transfered data
**/

void functions::comulative_gbf_pi(Pi* pi,int Nbf,block* GBF1,fstream* fout){
	    
    (*fout)<<"sending gbf to p0,"<<(pi->write_as_a_sender(GBF1,sizeof(block)*Nbf))<<"\n";
    delete [] GBF1;
    //std::cout<<"GBF1 deleted"<<std::endl;
    delete pi;
}
		

/**
 * creating P0's instances
 *
 * @param party index of P0's instance
 * @param port port to set
 * @param P0's vector of the P0's instances
 * @param numOfOnes numOfOnes to set
**/
void functions::creating_p0_instance(int party,int port,std::vector<P0*>* P0_s,uint32_t numOfOnes,uint32_t m_nSecParam, uint8_t* constSeed ){
     //cout<<"creating p0 for party "<<(party+1)<<endl;
     (*P0_s)[party]=new P0(port,numOfOnes,m_nSecParam,constSeed);
}
