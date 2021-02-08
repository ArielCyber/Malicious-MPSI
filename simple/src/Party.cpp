#include "Party.h"

//static attributes
int Party::Nbf=0;
int Party::Nc_sender=0;
Bitstring* Party::BF=nullptr; 
std::vector<int>* Party::items=nullptr; 
int Party::Not=0;
int Party::seeds_num=0;
int Party::items_size=0;
int Party::maxOnes=0; 


//c'tor
Party::Party(int numOfOnes,uint32_t m_nSecParam,uint8_t* constSeed ){
	
     //initialization of the attributes
     sub_group_sender=nullptr;
     sub_group_recv=nullptr;
     R_sender=nullptr;
     R_recv=nullptr;
     func_sender=nullptr;
     func_recv=nullptr;	
	 
     //setting vectors for the receiver/sender's strings 
     strings_choices=new std::vector<block>(Not);
     strings=new std::vector<std::array<block, 2>>(Not);
	 
     //setting cryptographic attribute
     crypt = new crypto(m_nSecParam, constSeed);
	
     //setting bitstring with the chosen number of 1's
     bitvector=new BitVector(Not);

     Bitstring* b1=new Bitstring(Not,numOfOnes,crypt);
     for (int i=0;i<Not;i++){
	(*bitvector)[i]=b1->get_bit(i);
     }		
     delete b1;
}


//setting common attributes
void Party::set(vector<int>* items,int items_size,int Nbf,int n,int Nc,int string_length,int maxOnes,int seeds_num){
	
    //cout<<"setting the items"<<endl;
    Party::items=items;
    Party::items_size=items_size;
	 
    //cout<<"setting Nbf"<<endl;
    Party::Nbf=Nbf;
	 
    //cout<<"setting BF"<<endl;
    BF=new Bitstring(Nbf);
	 
    //cout<<"setting Not"<<endl;
    Party::Not=n;
	 
    //cout<<"setting Nc"<<endl;
    Party::Nc_sender=Nc;
	 
    //cout<<"setting maxOnes"<<endl;
    Party::maxOnes=maxOnes;
	 
    //cout<<"setting seeds num"<<endl;
    Party::seeds_num=seeds_num;	
}


/**
     getters
**/

crypto* Party::getCrypto(){
   return crypt;
}

BitVector* Party::getChoices() const{
    return bitvector;
}

std::vector<block>& Party::getStringsReceiver(){
    return *strings_choices;
}

std::vector<std::array<block, 2>>& Party::getStrings(){
    //cout<<"strings:"<<strings->size();
    return *strings;
}

int Party::getNot() const{
	return Not;
}

int Party::getSeedsNum() const{
    return seeds_num;
}


//initialization of the sender
void Party::init_sender( int port){
     init_sender(&s,port);
}


//initialization of the receiver
void Party::init_receiver(const char* ip,int port){ 
     init_receiver(&recv,ip,port);
}


//initialization of a server
void Party::init_sender(struct sender* s, int port){
  
     s->sock = socket(AF_INET, SOCK_STREAM, 0);  
     if (s->sock < 0){ 
         perror ("Error oPiening channel");
     }
  
     memset((char *)&s->serv_name,0, sizeof(s->serv_name)); 
     s->serv_name.sin_family = AF_INET;                                   
     s->serv_name.sin_port = htons(port); 

     if (bind(s->sock, (struct sockaddr *)&s->serv_name, sizeof(s->serv_name)) < 0){
         perror ("Error naming channel");
     }
      
     //printf("Server is alive and waiting for socket connection from client.\n");
     listen(s->sock, 1); 

     s->len = sizeof(s->serv_name);
     s->connect_sock = accept(s->sock, (struct sockaddr *)&s->serv_name, &s->len);

}


//initialization of a client
void Party::init_receiver(struct receiver* recv,const char* ip,int port){ 
	
     //printf("Client is alive and establishing socket connection.\n");
     recv->sock = socket(AF_INET, SOCK_STREAM, 0);
	
     if (recv->sock < 0){ 
        perror ("Error oPiening channel");
        close(recv->sock);
        exit(1);
     }
      
     memset((char *)&recv->cli_name,0, sizeof(recv->cli_name)); 
     recv->cli_name.sin_family = AF_INET; 
     recv->cli_name.sin_addr.s_addr = inet_addr(ip); 
     recv->cli_name.sin_port = htons(port);

     int flag=1;
     while (flag)
     if (connect(recv->sock, (struct sockaddr *)&recv->cli_name, sizeof(recv->cli_name)) >= 0){

           flag=0;	
     }
}	


/**
  writing/reading as a sender/reciever
**/

unsigned long Party::write_as_a_sender(void* msg,unsigned long size){
    unsigned long left=size;
    unsigned long n;
    unsigned long sum=0;
    while (left){
           n=write(s.connect_sock, &((char*)msg)[size-left],left);
           left-=n;
           sum+=n;
    }
    return sum;
}

unsigned long Party::read_as_a_sender(void* msg,unsigned long size){
    unsigned long left=size;
    unsigned long n;
    unsigned long sum=0;
    memset(msg,0,(unsigned long)size);
    while (left){
           n=read(s.connect_sock, &((char*)msg)[size-left],left);
           left-=n;
           sum+=n;
    }
    return sum;	 
}

unsigned long Party::write_as_a_receiver(void* msg,unsigned long size){
    unsigned long left=size;
    unsigned long n;
    unsigned long sum=0;
    while (left){
           n=write(recv.sock, &((char*)msg)[size-left],left);
           left-=n;
           sum+=n;
    }
    return sum;
}

unsigned long Party::read_as_a_receiver(void* msg,unsigned long size){
    unsigned long left=size;
    unsigned long n;
    memset(msg,0,(unsigned long)size);
    unsigned long sum=0;
    while (left){
           n=read(recv.sock, &((char*)msg)[size-left],left);
           left-=n;
           sum+=n;    
    }
	
    return sum;	 
}	 


/**
   writing/reading from/to a socket
**/

unsigned long Party::write_as_a_sender(sender* snd,void* msg,unsigned long size){
    unsigned long left=size;
    unsigned long n;
    unsigned long sum=0;
    while (left){
           n=write(snd->connect_sock, &((char*)msg)[size-left],left);
           left-=n;
           sum+=n;
    }	 
    return sum;
}

unsigned long Party::read_as_a_sender(sender* snd,void* msg,unsigned long size){
     unsigned long left=size;
     unsigned long n;
     unsigned long sum=0;	 
     memset(msg,0,(unsigned long)size);
     while (left){
           n=read(snd->connect_sock, &((char*)msg)[size-left],left);
           left-=n;
           sum+=n;
     }
     return sum;	 
}

unsigned long Party::write_as_a_receiver(receiver* recv,void* msg,unsigned long size){
    unsigned long left=size;
    unsigned long n;
    unsigned long sum=0;
    while (left){
           n=write(recv->sock, &((char*)msg)[size-left],left);
           left-=n;
           sum+=n;
    }
    return sum;
}

unsigned long Party::read_as_a_receiver(receiver* recv,void* msg,unsigned long size){
    unsigned long left=size;
    unsigned long n;
    unsigned long sum=0;
    memset(msg,0,(unsigned long)size);
	 
    while (left){
           n=read(recv->sock, &((char*)msg)[size-left],left);
           left-=n;
           sum+=n;
    }
	
    return sum;	 
}


/**
   setting a random sub group of indexes (size: Nc_sender)
**/

void Party::set_sub_group(){
	
    //creating seed
    unsigned long x1;
    unsigned long x2;
    crypt->gen_rnd((BYTE*)&x1,8);
    crypt->gen_rnd((BYTE*)&x2,8);
    seed_sub_group=toBlock(x1,x2);
	
    //getting sub group
    sub_group_sender=new int[Nc_sender];
    functions::get_sub_group(sub_group_sender,Not,Nc_sender,seed_sub_group);
   
    //cout<<"sub group was set"<<endl;
}


/**
   sending/receiving the sub group
**/

unsigned long Party::send_sub_group(){
    unsigned long sum=write_as_a_sender( &Nc_sender,sizeof(int));		   
    sum+=write_as_a_sender( &seed_sub_group,sizeof(block));
    //cout<<"sub group was sent"<<endl;
    return sum;
}

unsigned long Party::recv_C(){
    unsigned long sum=read_as_a_receiver(&Nc_recv,sizeof(int));
    sub_group_recv=new int[Nc_recv];
    block seed_recv;
    sum+=read_as_a_receiver( &seed_recv, sizeof(block));
    functions::get_sub_group(sub_group_recv,Not,Nc_recv,seed_recv);
    return sum;
}


 /**
   computing R/r accordingly to the received sub group
 **/

void Party::compute_R_r(){
    func_recv=new int[Not];
    for (int i=0;i<Not;i++) func_recv[i]=i;
    R_recv=new std::vector<int>;
    functions::compute_R_r(sub_group_recv,func_recv,bitvector,Nc_recv,Not,*strings_choices,r_recv,*R_recv);  
    if (sub_group_recv!=nullptr) delete [] sub_group_recv;
    Nr_recv=(*R_recv).size();
    //cout<<"Nr: "<<Nr_recv<<endl<<endl;
    //cout<<"r* (P0): "<<r_recv;
    //cout<<endl<<endl;
    //cout<<"r and R were computed"<<endl;
}


 /**
   sending/receiving R/r
 **/

unsigned long Party::send_R_r(){
    unsigned long sum=write_as_a_receiver(&Nr_recv,sizeof(int));		
    int arr[Nr_recv];
    for (int i=0;i<Nr_recv;i++)
    arr[i]=(*R_recv)[i];		
    if (R_recv!=nullptr) delete R_recv;
    sum+=write_as_a_receiver(arr,sizeof(int)*Nr_recv);
    sum+=write_as_a_receiver(&r_recv,sizeof(block));
    //cout<<"r and R were sent"<<endl;
    return sum;
}

unsigned long Party::recv_R(){
     unsigned long sum=read_as_a_sender(&Nr_sender,sizeof(int));
     //cout<<"From Alice: Nr: "<<Nr_sender<<endl<<endl;
     R_sender=new int[Nr_sender];
     sum+=read_as_a_sender(R_sender,sizeof(int)*Nr_sender);
     sum+=read_as_a_sender(&r_sender,sizeof(block));
     //cout<<"From Alice: r* (Alice): ";
     //cout<<r_sender;
     //cout<<endl;
     //cout<<"R and r were received"<<endl;
     return sum;
}


/**
   creation of the Bloom filter
**/

void Party::create_BF_threads(std::set<int>* h_kokhav,unsigned int* hash_seeds, block seed){  
    BF=new Bitstring(Nbf); 
    functions::create_BF_threads( h_kokhav,items, items_size, BF,Nbf,seeds_num,hash_seeds, seed);   
    //cout<<"BF was created"<<endl;  
}


/**
   creating vectors of the 0's/1's indexes
   The indexes are shuffled
**/

void Party::get_zeros_ones(){
     functions::get_zeros_ones(bitvector, Not, Nc_recv, arr_indexes,crypt);
}


/**
   arranging the indexes accordingly to th BF's bits
**/

void Party::arrange_the_indexes(){
   functions::arrange_the_indexes(*BF,Nbf,arr_indexes,&func_recv);
   //cout<<"indexes were arranged"<<endl;   
}


/**
   sending/receiving the generated function
**/

unsigned long Party::recv_func(){
     func_sender=new int[Nbf];
     unsigned long sum=read_as_a_sender(func_sender,sizeof(int)*Nbf);
     //cout<<"func was received"<<endl;
     return sum;
}

unsigned long Party::send_func(){
    unsigned long sum=write_as_a_receiver(func_recv,sizeof(int)*Nbf);
    //cout<<"func was sent"<<endl;
    return sum;	
}


/**
   xoring the strings of the receiver/sender with the synchGBF object, 
   (which is common to all the P0's instances)
**/

void Party::xor_RBF_receiver(synchGBF* test){
   test->XORordered(strings_choices,func_recv);
   //cout<<"rbf was created"<<endl;
   if (func_recv!=nullptr) delete [] func_recv;
   if (strings_choices!=nullptr) delete strings_choices;
}

void Party::xor_RBF_sender(synchGBF* test){
    test->XORorderedBF(strings, func_sender,BF);
    //cout<<"RBF was created"<<endl;
	if (func_sender!=nullptr) delete [] func_sender;  
    if (strings!=nullptr) delete strings;
}	


/**
   checking if the protocol is secure enougth. 
   if not- the execution will be stopped.
**/

int Party::check_if_to_abort(){

    if (Nc_recv>Not-Nbf){
       cout<<"Nc>Not-Nbf: session has been stopped"<<endl;
       close(recv.sock);
       return 1;
    }
    else{
       //cout<<"Nc<=Not-Nbf: session continues"<<endl;
       return 0;
    }
}

int Party::check_if_to_abort1(){

    if (Nc_sender-Nr_sender>maxOnes){
	cout<<"Ncc-Nr>Nmaxones: session has been stopped"<<endl;
        close(s.connect_sock);
	close(s.sock);
	if (sub_group_sender!=nullptr) delete [] sub_group_sender;
	sub_group_sender=nullptr;
	if (R_sender!=nullptr) delete [] R_sender;
	return 1;
    }
    else{
	int flag=0;
	block r1;
	functions::compute_r(r1,*strings, R_sender,Nr_sender);
	//cout<<endl<<"r*(Alice) :"<<r1;
        if (neq(r1,r_sender))
        flag=1;
	//cout<<endl<<endl;

	if (flag==1){
	     cout<<"r* is not OK: session has been stopped"<<endl;
             close(s.connect_sock);
             close(s.sock);
	     if (sub_group_sender!=nullptr) delete [] sub_group_sender;
	     sub_group_sender=nullptr;
	     if (R_sender!=nullptr) delete [] R_sender;
             return 1;
        }
        else{
	     //cout<<"Ncc-Nr<=Nmaxones && r*(Alice)==r*(Alice): session continues"<<endl;
	     if (R_sender!=nullptr) delete [] R_sender;
	     return 0;
        }
     }
     return 0;
}


int Party::check_if_to_abort2(){
	int i;
    if (functions::if_disjoint_sets(sub_group_sender,Nc_sender,func_sender,Nbf,Not)==0){
	if (sub_group_sender!=nullptr) delete [] sub_group_sender;
	cout<<"part of the function's indexes were in the sub group: session has been stopped"<<endl;
	i=1;
	write_as_a_sender(&i,sizeof(int));
	close(s.connect_sock);
	close (s.sock);
	return 1;
    }
    else{
	if (sub_group_sender!=nullptr) delete [] sub_group_sender;
	//cout<<"the function and the sub group are disjoint: session continues"<<endl;
	i=0;
	write_as_a_sender(&i,sizeof(int));		
        return 0;	
    }
}


//deleting the bloom filter
void Party::delete_bf(){
     if (BF!=nullptr) delete BF;
}


//d'tor
Party::~Party(){
   close(recv.sock);
   close(s.connect_sock);
   close(s.sock);
   delete crypt;
}

