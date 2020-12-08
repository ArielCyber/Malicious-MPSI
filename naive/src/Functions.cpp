#include "Party.h"
#include "P0.h"
#include "Pi.h"
#include "Functions.h"

//TEST
//#include <mutex>          // std::mutex
//std::mutex mtx;           // mutex for critical section

//#######
/**
 * Ordering the strings indexes in this way-
 * if in the Bloom filter at place '0' there is the bit '1',
 * so, in the indexes array , at place '0', will be the index of some string which has bit '1'.
 *
 * @param BF Bloom filter
 * @param Nbf Bloom filter's size
 * @param b bitset of the bits for every string's index
 * @param indexes Array of the indexes of the strings
 * @param Not the size of the bitset and the indexes
 * @param Ncc size of group C
 * @param crypt cryptographic var
 */
	
	
void functions::arrange_the_indexes(const Bitstring& BF,int Nbf,BitVector* b, int* indexes,int Not, int Ncc,crypto *crypt){
   
   vector<int> zeros_to_set;
   vector<int> ones_to_set;
   vector<int> zeros;
   vector<int> ones;
	
   //searching for the indexes of 0's and 1's in the BF
   for (int i = 0; i <Nbf; i++) {
       if (BF.get_bit(i)==0) 
           zeros_to_set.push_back(i);
       else
           ones_to_set.push_back(i);
   }
	 
   //searching for the 0's and 1's in the Bitstring
   for (int i = 0; i <(Not-Ncc); i++) {
       if ((*b)[i]==0)
	   zeros.push_back(i);
       else
	   ones.push_back(i);
   }
   
    //there are not enough 0's or 1's in the range 0..Not-Ncc	  
    if ((ones.size()<ones_to_set.size()) || (zeros.size()<zeros_to_set.size())) {
	cout<<"There are not enougth 1's or 0's in 0..Not-Ncc-1"<<endl;
    cout<<"Number of ones = "<<ones.size()<<", required number ="<<ones_to_set.size()<<endl;
    cout<<"Number of zeros = "<<zeros.size()<<", required number ="<<zeros_to_set.size()<<endl;
    if (ones.size()<ones_to_set.size())
    {
            cout<<"Not enough ones - big problem!"<<endl;
            exit(1);
    }
    if (zeros.size()<zeros_to_set.size()) 
    {
            cout<<"Not enough zeros - annoying problem!"<<endl;
    }
        
    //in order that will be intersection between the two group and the other size will abort
    indexes[Nbf-1]=indexes[Not-1];
    //return;
    }

    //get random numbers 
    unsigned int* r=new unsigned int[zeros_to_set.size()];
    crypt->gen_rnd((BYTE*)r,4*zeros_to_set.size());

    //arranging the indexes
    int* indexes1=new int[Nbf];
    int current=0;
    int size =zeros.size();
		    
    for (long unsigned int i=0;i<zeros_to_set.size();i++){

       //get random number in range [0,..,number of zeros]   
       int rand0=r[i]%size;
	   
       //placing the chosen index in the correct place according to the bloom filter
       indexes1[zeros_to_set[current]]=indexes[zeros[rand0]];
       
       //putting the index in the last place in order to not choose it again
       int temp=zeros[size-1];
       zeros[size-1]=zeros[rand0];
       zeros[rand0]=temp;
		   
       current++;
       size--;
    }
	
    delete [] r;
	
    //get random numbers 
    unsigned int* r1=new unsigned int[ones_to_set.size()];
    crypt->gen_rnd((BYTE*)r1,4*ones_to_set.size());

    //arranging the indexes and creating the function

    current=0;
    size =ones.size();
	   
    for (long unsigned int i=0;i<ones_to_set.size();i++){
       int rand1=r1[i]%size;

       indexes1[ones_to_set[current]]=indexes[ones[rand1]];

       int temp=ones[size-1];
       ones[size-1]=ones[rand1];
       ones[rand1]=temp;
		   
       current++;
       size--;
    }	
	
    delete [] r1;
	
    cout<<endl;
    for (int i=0;i<Nbf;i++)
       indexes[i]=indexes1[i];	

    delete [] indexes1;
	
    cout<<"arrange_the_indexes complete!"<<endl;
}

//#######

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

//#######

/**
 * Creating a bloom filter and setting group of indexes to each item
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
 

void functions::create_BF(std::set<int>* h_kokhav,std::vector <int>& items, int len, Bitstring& BF, int Nbf,int seeds_num, unsigned int* seeds, block seed){
    
        
    //setting as key the common seed, and as the messages xor of the item and 0,..,len/2
    auto start = std::chrono::steady_clock::now();
    
    details::AES<details::AESTypes::NI> aes;
    aes.setKey(seed);
	
    int encNum=seeds_num/2+(seeds_num%2);
    block* plaintexts = new block[encNum];
    block* ciphertexts= new block[encNum];
	
    auto med = std::chrono::steady_clock::now();
    std::cout<<"BF consruction setup time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(med-start).count()<<std::endl;
        
    //for each item
    for (int i=0;i<len;i++){
         //setting to the messages xor of the item and 0,..,len/2
         for (int j=0;j<encNum;j++) 
	      plaintexts[j]=toBlock(items[i],0)^toBlock(0,j);

         //activating aes
         aes.ecbEncBlocks(plaintexts, encNum, ciphertexts);

         for (int j=0;j<seeds_num/2;j++){
             //activate modulo Nbf on the AES result and putting 1 at this index in the bloom filter
             uint64_t h1=(ciphertexts[j].as<std::uint64_t>()[0])%((uint64_t)Nbf);
             uint64_t h2=(ciphertexts[j].as<std::uint64_t>()[1])%((uint64_t)Nbf);

             BF.set_bit(h1,1);
             BF.set_bit(h2,1);
             //adding each index to the indexes' set of the item
             h_kokhav[i].insert(h1);
             h_kokhav[i].insert(h2);
         }
         if ((seeds_num%2)==1){
	     uint64_t h1=(ciphertexts[seeds_num/2].as<std::uint64_t>()[0])%((uint64_t)Nbf);
	     BF.set_bit(h1,1);
             h_kokhav[i].insert(h1);
         }

    }

     auto end = std::chrono::steady_clock::now();
     std::cout<<"BF construction online time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()<<std::endl;
     
     delete [] ciphertexts;
     delete [] plaintexts;

    
}

//#######
	   
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
	
	std::cout<<"creating rands vals"<<std::endl;
	int size=(int)((small_group_size/2)*1.1);
	if ((small_group_size%2)==1) size++;
	std::cout<<"size: "<<size;
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
         std::cout<<"getting random vals"<<std::endl; 
		 
         std::set<unsigned int>* s=new set<unsigned int>();

	 int i=0; 
         int j=0;  
		 
         while((int)(*s).size()!=small_group_size){
                   (*s).insert((unsigned int)(ciphertexts[j].as<std::uint64_t>()[i])%big_group_size);
                   i^=1;
                   if (i==0) j++;
                   if (j==size) std::cout<<"out of randoms"<<std::endl;      
         }                                                                                                                                                                                                                                                                                                                                                                                 delete [] ciphertexts;

                  
		 
         std::cout<<"setting the vals to the sub group"<<std::endl;   
         i=0;
	 for (auto item:(*s)){
		   sub_group[i++]=item;    
	 }
		
         delete s;  		 

}

//#######

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


//#######

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
    cout<<"Size of R = "<<R.size()<<endl;
    cout <<"Ncc - R ="<<Ncc-R.size()<<endl;
}

//#######

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



// ##########
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

    auto start = std::chrono::steady_clock::now();  

    //creating Y's array
    Y=new block[items.size()];

    auto med = std::chrono::steady_clock::now();    
    std::cout<<"Get_Y time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(med-start).count()<<std::endl;        

    //setting 0's to Y's array
    for (int i=0;i<(int)items.size();i++)
        Y[i]=toBlock(0, 0);

   //setting 0's to GBF's array
    for (int i=0;i<Nbf;i++)            
        GBF[i]=toBlock(0,0);

    block block0=toBlock(0,0);

    //going over all the items in the DB and filling the GBF
    std::cout<<"rerandomizing"<<std::endl; 
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

    auto med2 = std::chrono::steady_clock::now();
    std::cout<<"GBF data construction time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(med2-med).count()<<std::endl;

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

    auto end=std::chrono::steady_clock::now();
    std::cout<<"GBF putting random strings time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-med).count()<<std::endl;

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

    auto start = std::chrono::steady_clock::now();
    
    //setting the Y's array
    std::cout<<"geting_Y"<<std::endl;
    Y=get_Y(n, pi , h_kokhav, Y, test);

    auto med = std::chrono::steady_clock::now();
    std::cout<<"Get_Y time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(med-start).count()<<std::endl;
    
    //setting 0's to GBF's array
    for (int i=0;i<Nbf;i++) 
         GBF[i]=toBlock(0,0);

    block block0=toBlock(0,0);
	
    //going over all the items in the DB and filling the GBF
    std::cout<<"rerandomizing"<<std::endl;
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

    auto med2 = std::chrono::steady_clock::now();
    std::cout<<"GBF data construction time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(med2-med).count()<<std::endl;
    
	
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
    auto end = std::chrono::steady_clock::now();
    std::cout<<"GBF putting random strings time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-med).count()<<std::endl;
    
	
    delete [] Y;
    return GBF;
}

//#######

/**
 * checking if the item is in the item's list
 *
 * @param h_kokhav h*(item x)
 * @param GBF
**/

block* functions::check_the_item(set<int>& h_kokhav,block* GBF){
	
    block* y=new block(0,0);

    //going over all the indexes in h* and xoring them
    set<int>::iterator begin=h_kokhav.begin();
    set<int>::iterator end=h_kokhav.end();
    for (;begin!=end;begin++){
	    (*y)=(*y)^GBF[*begin];
    }
	
    return y;
}

//############

/**
 * performing offline apport as a receiver
 *
 * @param Party pointer to the party object
 * @param ips vector of the parties' ips
 * @param ports vector of the parties' ports
 * @param player the player's index
 * @param other_player the other player's index
**/

void functions::offline_apport_receiver(Party* party,std::vector<std::string>* ips, int** ports,int player, int other_player){

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

   }

   else {
       std::cout<<"aborting"<<endl;exit(1);
   }

}

//############

/**
 * performing offline apport as a sender
 *
 * @param Party pointer to the party object
 * @param nsndvals parameter for the OT protocol
 * @param stype parameter for the OT protocol
 * @param rtype parameter for the OT protocol
 * @param crypt_ parameter for the OT protocol
**/

void functions::offline_apport_sender(Party* party,std::vector<std::string>* ips, int** ports,int player,int other_player){

    //std::cout<<"starting offline apport sender"<<std::endl;
    string str1=(*ips)[player];
    cout<<str1;
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

//############

/**
 * performing online apport as a receiver- 
 * creating function according to the BF bits and sending it to the other party
 *
 * @param Party pointer to the party object
**/

void functions::online_apport_receiver(Party* party, synchGBF* test){

    //party arranging the indexes of the initial bitstring accordingly to the bloom filter bits
    party->arrange_the_indexes();

    //party sends the injective func
    party->send_func();

}

//############

/**
 * performing online apport as a sender-
 * receiving the function from the other party
 *
 * @param Party pointer to the party object
**/

void functions::online_apport_sender(Party* party, synchGBF* test){
	
    //party receives the injective func
    party->recv_func();

}

//#############
/**
 * running online apport as a sender and as a receiver
 *
 * @param Party pointer to the party object
**/

void functions::run_online_apport(Party* p0, synchGBF* test){

    //online apport receiver
    thread* t1=new thread(functions::online_apport_receiver,p0, test);
    //online apport sender
    thread* t2=new thread(functions::online_apport_sender,p0, test);	
	
    t1->join();
    t2->join();
	
    delete t1;
    delete t2;
	
}

//#############
/**
 * running offline apport as a sender and as a receiver
 *
 * @param Party pointer to the party object
 * @param ips vector to the ips of all the parties
 * @param ports vector to the ports of all the parties
 * @param player index of the current player
 * @param other_player index to the player we are going to play against
**/


void functions::run_offline_apport(Party* p0,std::vector <std::string>* ips,int** ports,int player,int other_player){
			
    //running offline apport receiver    
    thread* t1=new thread(functions::offline_apport_receiver,p0,ips,ports,player,other_player);
    //running offline apport sender
    thread* t2=new thread(functions::offline_apport_sender,p0,ips,ports,player,other_player);
	
    t1->join();
    t2->join();
	
    delete t1;
    delete t2;

}

//#############

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

//#############

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
    cout<<"parties:"<<*parties<<endl;
    for (int i=0;i<*parties;i++){
	cout<<ips[i]<<endl;
    }	
}

//#############

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
             cout << name << ": " << values[counter-1] << '\n';
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

//##############

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
**/	


void functions::seeds_agreement_pi(Pi* pi,int parties,Commit& commit,block& seed,int player){
	
    //receiveng and sending seed and commit to each party
    Commit* commits_recv=new Commit[parties-1];
    block* seeds_recv=new block[parties-1];	
	
    vector<thread*> threads;
	
    for (int p=0;p<parties;p++){
	if (p==player) continue;
	int place=p;
	if (player<p) place=p-1;
    	threads.push_back(new thread(functions::seeds_pi_write,pi,&commit,&seed,p));	
        threads.push_back(new thread(functions::seeds_pi_read,pi,&commits_recv[place],&seeds_recv[place],p));		
    }
	
    for (auto& t:threads) t->join();
    for (auto& t:threads) delete t;
    threads.clear();
			
    int flag=1;
    int j=0;
			
    //checking if the received pair of seed-commit from each party is OK
    while (flag && (j<(parties-1))){
	if (commits_recv[j]!=Commit(seeds_recv[j])) flag=0;
        j++;
    }				
			
    if (flag==0) cout<<"commit is not ok"<<endl;

    //calculating the common seed
    for (int p=0;p<parties-1;p++){
	seed=seed^seeds_recv[p];
    }
			
    cout<<"common seed:"<<seed<<endl;
			
    delete [] commits_recv;
    delete [] seeds_recv;
}

//helper function
void functions::seeds_pi_write(Pi* pi,Commit* commit,block* seed,int party){
    pi->write_to_player(party,commit,sizeof(Commit));
    pi->write_to_player(party,seed,sizeof(block));
}

//helper function
void functions::seeds_pi_read(Pi* pi,Commit* commit,block* seed,int party){
    pi->read_from_player(party,commit,sizeof(Commit));
    pi->read_from_player(party,seed,sizeof(block));
}

//#############

/**
 * agreement phase of p0 - sending and receiving commit+key , checking if the received objects are ok , ans than computing common seed
 *
 * @param P0_s vector of pointers to the P0's objects 
 * @param commit the commitment
 * @param seed thes seed
**/	


void functions::seeds_agreement_p0(std::vector<P0*>& P0_s,Commit& commit,block& seed){

    //arrays for the commits and seeds which will be received
    Commit* commits_recv=new Commit[P0_s.size()];
    block* seeds_recv=new block[P0_s.size()];
			
    vector<thread*> threads;
	
    //sending P0's seed and commit, and receiving it from the other parties
    for (int p=0;p<(int)P0_s.size();p++){
         threads.push_back(new thread(functions::seeds_p0_write,P0_s[p],&commit,&seed));
	 threads.push_back(new thread(functions::seeds_p0_read,P0_s[p],&commits_recv[p],&seeds_recv[p]));
    }
	
    for (auto& t:threads) t->join();
    for (auto& t:threads) delete t;
    threads.clear();
	
    //checking that the commits and seeds which has been received are OK
    int flag=1;
    int j=0;
    while (flag && (j<(int)P0_s.size())){
	if (commits_recv[j]!=Commit(seeds_recv[j])) flag=0;
	j++;
    }
	
    if (flag==0) cout<<"commit is not ok"<<endl;

    //computing the common seed
    for (int i=0;i<(int)P0_s.size();i++){
	seed=seed^seeds_recv[i];
    }
			
    cout<<"common seed:"<<seed<<endl;
			
    delete [] commits_recv;
    delete [] seeds_recv;

}

//helper function for sending the seed an commit to the other party
void functions::seeds_p0_write(P0* p0,Commit* commit,block* seed){
    p0->write_as_a_sender(commit,sizeof(Commit));
    p0->write_as_a_sender(seed,sizeof(block));
	
}

//helper function for receiving the seed and commit from the other party
void functions::seeds_p0_read(P0* p0,Commit* commit,block* seed){
    p0->read_as_a_sender(commit,sizeof(Commit));
    p0->read_as_a_sender(seed,sizeof(block));
}

//##############

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
**/


void functions::secret_sharing_seed_pi(Pi* pi,int parties,crypto* crypt,int player,block* keys,block* keys_recv){
		
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
        for (int p=0;p<parties;p++){
	    if (p==player) continue;
	    int place=p;
	    if (p>player) place=p-1;
	    threads.push_back(new thread(functions::secret_read,pi,p,&keys_recv[place]));
	    threads.push_back(new thread(functions::secret_write,pi,p,&keys[place]));					
	}
	
        for (auto& t:threads) t->join();
	for (auto& t:threads) delete t;
	threads.clear();
	
}

//helper function
void functions::secret_write(Pi* pi,int other_player,block* keys){
    pi->write_to_player(other_player,keys,sizeof(block));
    cout<<"write to "<<other_player<<" "<<*keys<<endl;

}

//helper function
void functions::secret_read(Pi* pi,int other_player,block* keys){
    pi->read_from_player(other_player,keys,sizeof(block));
    cout<<"read from "<<other_player<<" "<<*keys<<endl;
}

//#############

/**
 * secret sharing- p0 generates keys using the crypto var
 * After he has finished to generate the keys, he gets/sends the keys from/to the rest of the parties
 * @param p0_s vector of P0's instances
 * @param parties number of parties
 * @param keys array of keys
 * @param keys_recv array of keys_recv
**/

void functions::secret_sharing_seed_p0(vector <P0*>& p0_s,int parties,block* keys,block* keys_recv){
	  
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
        for (int p=0;p<parties-1;p++){
	    threads.push_back(new thread(functions::secret_read_p0,p0_s[p],&keys_recv[p]));
	    threads.push_back(new thread(functions::secret_write_p0,p0_s[p],&keys[p]));					
	}
	
        for (auto& t:threads) t->join();
	for (auto& t:threads) delete t;
	threads.clear();
	
}

//helper function to send the key
void functions::secret_write_p0(P0* p0,block* keys){
    p0->write_as_a_sender(keys,sizeof(block));
    cout<<"block:"<<(*keys)<<" ";
}

//helper function to receive the key
void functions::secret_read_p0(P0* p0,block* keys){
    p0->read_as_a_sender(keys,sizeof(block));
    cout<<"block:"<<(*keys)<<" ";
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
  
void functions::secret_sharing_aes(int parties,uint32_t Nbf,synchGBF* test,block* keys,block* keys_recv)	{	
	
     auto start = std::chrono::steady_clock::now();
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

    auto med = std::chrono::steady_clock::now();
    std::cout<<"Secret-sharing setup time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(med-start).count()<<std::endl;

    //activating AES and xoring the secrets
    for (int p=0;p<(parties-1);p++) {
	    aes->setKey(keys[p]);
	    aes->ecbEncBlocks(plaintexts, Nbf, ciphertexts);
	    for (unsigned int i=0;i<Nbf;i++) r[i]=r[i]^ciphertexts[i];
	    aes->setKey(keys_recv[p]);
	    aes->ecbEncBlocks(plaintexts, Nbf, ciphertexts);	
	    for (unsigned int i=0;i<Nbf;i++) r[i]=r[i]^ciphertexts[i];
     }	

    auto end = std::chrono::steady_clock::now();
    std::cout<<"Secret-sharing time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-med).count()<<std::endl;	

    //freeing memory
    delete [] keys;
    delete [] keys_recv;
    delete [] plaintexts;
    delete [] ciphertexts;
    delete aes;
	   	
}

//##############

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

//##############

/**
 * receiving GBF1_STAR from the opponent players and xoring the with the shared GBF
 *
 * @param p0 instant of P0
 * @pararm test shared GBF
**/

void functions::get_gbf(P0* p0, synchGBF* test)
{	
    int chunk_size=test->getChunkSize();	
    block* chunkArr=new block[chunk_size];
    int n=P0::Nbf/chunk_size;

    //getting the GBF from Pi and xoring it with the shared GBF
    for (int i=0;i<n;i++)
    {
       p0->read_as_a_receiver(chunkArr,sizeof(block)*chunk_size);
       test->XORchunk(chunkArr,i);
    }
	
    if (n*chunk_size<P0::Nbf)
    {
        p0->read_as_a_receiver(chunkArr,sizeof(block)*(P0::Nbf-n*chunk_size));
        test->XORchunk_limited(chunkArr,n,P0::Nbf-n*chunk_size);
    }	
}

//#######
/**
 * COMPUTING P0's GBF
 *
 * @param P0_s vector of the p0's instances
 * @param Nbf size of the GBF
 * @param test shared GBF
 
**/

block* functions::compute_gbf(std::vector<P0*>& P0_s,int Nbf, synchGBF* test){
	
   //getting GBF from the other parties and xoring them
   vector <thread*> threads;
   for (int i=0;i<(int)P0_s.size();i++){
        threads.push_back(new thread(functions::get_gbf,P0_s[i],test));	
   }		

   for (auto& t:threads) t->join();
   for (auto& t:threads) delete t;
   threads.clear();

   return test->getGBF();		
}

//################

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


//##############
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
  
    block* M1=test->getGBF(); //M1=pi->get_RBF_sender();
		
    for(int i=0;i<items_size;i++)
       for (int j:h_kokhav[i])			
	   Y[i]=Y[i]^M1[j];
		   
    return Y;
}

//##############
/**
 * sending the GBF of pi to p0
 *
 * @param pi pointer to the player
 * @param Nbf size of GBF
 * @param GBF1 the GBF
**/

void functions::comulative_gbf_pi(Pi* pi,int Nbf,block* GBF1){
	    
    pi->write_as_a_sender(GBF1,sizeof(block)*Nbf);
    delete [] GBF1;
    std::cout<<"GBF1 deleted"<<std::endl;
    delete pi;
}
		


//##############
/**
 * creating P0's instances
 *
 * @param party index of P0's instance
 * @param port port to set
 * @param P0's vector of the P0's instances
 * @param numOfOnes numOfOnes to set
**/
void functions::creating_p0_instance(int party,int port,std::vector<P0*>* P0_s,uint32_t numOfOnes,uint32_t m_nSecParam, uint8_t* constSeed ){
                    cout<<"creating p0 for party "<<(party+1)<<endl; 
                    (*P0_s)[party]=new P0(port,numOfOnes,m_nSecParam,constSeed); 
}  

//##############
/**
 * computing the keys of P0
 *
 * @param keys array for the created keys
 * @param h_kokhav h* of each item
 * @param items_size the size of the items
 * @param p pointer to P0 instance 
**/

void functions::compute_keys_p0(block* keys,std::set<int>* h_kokhav,int items_size,P0* p){
     auto start=std::chrono::steady_clock::now();
	
     //going over all the items
     for (int i=0;i<items_size;i++){
         std::set<int>::iterator begin1=h_kokhav[i].begin();
         std::set<int>::iterator end1=h_kokhav[i].end();
         keys[i]=toBlock(0, 0);
	     
	 //xoring all the strings in h* of the item
         for (;begin1!=end1;begin1++){
             int index=(*begin1);
             keys[i]=keys[i]^(p->getStringsReceiver()[p->get_func_recv()[index]]);
         }
     }  
     if (&(p->getStringsReceiver())!=nullptr) delete &(p->getStringsReceiver()); 
     std::cout<<"keys were computed"<<std::endl; 
     auto end1=std::chrono::steady_clock::now(); 
     cout<<"Duration of computing keys: "<<std::chrono::duration_cast<std::chrono::milliseconds>(end1-start).count()<<endl;
}

//##############
/**
 * computing the keys of P0
 *
 * @param keys array for the created keys
 * @param h_kokhav h* of each item
 * @param items_size the size of the items
 * @param p pointer to Pi instance 
**/

void functions::compute_keys_pi(block* keys,std::set<int>* h_kokhav,int items_size,Pi* p){
     auto start=std::chrono::steady_clock::now(); 
	
     //going over all the items
     for (int i=0;i<items_size;i++){
         std::set<int>::iterator begin1=h_kokhav[i].begin(); 
         std::set<int>::iterator end1=h_kokhav[i].end();
         keys[i]=toBlock(0, 0);
	     
	 //xoring all the strings in h* of the item
         for (;begin1!=end1;begin1++){
             int index=(*begin1);
             keys[i]=keys[i]^(p->getStrings()[p->get_func_sender()[index]][1]);   
         }
     }
     if (&(p->getStrings())!=nullptr) delete &(p->getStrings()); 
     std::cout<<"keys were computed_"<<std::endl;     
     auto end=std::chrono::steady_clock::now();
     cout<<"Duration of computing keys: "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()<<endl; 
}

//##############
/**
 * p0 receiving keys from pi
 *
 * @param p0 instance of P0
 * @param keys array for saving the received keys
**/

void functions::recv_keys(P0* p0,block* keys){
   p0->recv_keys(keys); 
   cout<<"got key"<<endl; 
}


//##############
/**
 * xoring all the keys of the players from player 0 to parties-1
 *
 * @param parties number of players that participate
 * @param keys array of players which every player has his array of keys
 * @param new_keys array for saving all the possible keys
 * @param item_ind array for saving the corresponding index of item to the key in the same place in the array of keys
 * @param items_size num of items
 * @param counter the place of the new key
 * @param index the index to be placed in the items_ind array
 * @param player the player that we xor his keys
 * @param sum the sum of xored keys
 * 
**/

void functions::xor_2(int parties, block **keys, block* new_keys, int* items_ind,int items_size, int counter, int index, int player,block sum){
   if (player==parties){
      new_keys[counter]=sum;
      items_ind[counter]=index;
   }
   else 
      for (int i=0;i<items_size;i++){
          if (player==0) index=i;
	  xor_2(parties,keys,new_keys,items_ind,items_size,(counter*items_size+i),index,(player+1),(keys[player][i]^sum));
      }
}

//##############
/**
 * xoring all the keys of the players from the chosen player to the number of players-1
 *
 * @param parties number of players
 * @param keys array of players which every player has his array of keys
 * @param new_keys array for saving all the possible keys
 * @param items_size num of items
 * @param counter the place of the new key
 * @param player the player that we xor his keys
 * @param sum the sum of xored keys
 * 
**/

void functions::xor_3(int parties, block **keys, block* new_keys,int items_size, int counter, int player,block sum){
     if (player==parties){
         new_keys[counter]=sum;
     }
     else 
	 for (int i=0;i<items_size;i++){
	      xor_3(parties,keys,new_keys,items_size,(counter*items_size+i),(player+1),(keys[player][i]^sum));
	 }
}

//##############
/**
 * sorting the keys using merge sort
 *
 * @param arr array of keys
 * @param low the lower bound of the array to sort
 * @param high the higher bound of the array to sort
 * 
**/

void functions::merge_sort1(block* arr,int low,int high){
	int n=high-low;
	if (n<=1) return;
	int mid=(low+high)/2;
	merge_sort1(arr,low,mid);
	merge_sort1(arr,mid,high);
	int i=low;
	int j=mid;
	int k=0;
	block* temp=new block[n];
	while(i<mid && j<high){
		if (arr[i]<arr[j]) temp[k++]=arr[i++];
		else temp[k++]=arr[j++];
	}
	while (i<mid) temp[k++]=arr[i++];
	while (j<high) temp[k++]=arr[j++];
	for (int l=0;l<n;l++)
		arr[low+l]=temp[l];
	delete [] temp;
}

//##############
/**
 * sorting the keys using merge sort, and arranging the item's indexes accordingly
 *
 * @param arr array of keys
 * @param low the lower bound of the array to sort
 * @param high the higher bound of the array to sort
 * @param arr_items array of the indexes of the items
 * 
**/

void functions::merge_sort2(block* arr,int low,int high,int* arr_items){
	int n=high-low;
	if (n<=1) return;
	int mid=(low+high)/2;
	merge_sort2(arr,low,mid,arr_items);
	merge_sort2(arr,mid,high,arr_items);
	int i=low;
	int j=mid;
	int k=0;
	block* temp=new block[n];
	int* temp1=new int[n];
	while(i<mid && j<high){
		if (arr[i]<arr[j]){ 
		   temp[k]=arr[i];
		   temp1[k++]=arr_items[i++];
		}
		else {
			temp[k]=arr[j];
			temp1[k++]=arr_items[j++];
		}
	}
	while (i<mid){
		temp[k]=arr[i];
		temp1[k++]=arr_items[i++];
	}
	while (j<high) {
		temp[k]=arr[j];
		temp1[k++]=arr_items[j++];
	}
	for (int l=0;l<n;l++){
		arr[low+l]=temp[l];
		arr_items[low+l]=temp1[l];
	}
	delete [] temp;
	delete [] temp1;
}

//##############
/**
 * searching for a item in the sorted array using binary search
 *
 * @param arr array of keys
 * @param size size of the array
 * @param find key to find
 * 
**/
bool functions::binary_search(block* arr,int size,block find){
	if ((find<arr[0]) || (arr[size-1]<find)) return false;
	int low=0;
	int high=size-1;
	while (low<=high){
		int mid=(low+high)/2;
		if (arr[mid]==find) return true;
		if (find<arr[mid]) high=mid-1;
		else low=mid+1;
	}
	return false;
}


