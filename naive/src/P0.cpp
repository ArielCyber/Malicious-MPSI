#include "P0.h"

//$$$ 
P0::P0(int port,int numOfOnes,uint32_t m_nSecParam,uint8_t* constSeed )
//$$$
{
     sub_group_recv=nullptr;
     R_sender=nullptr;
     R_recv=nullptr;
     func_sender=nullptr;
     func_recv=nullptr;
     RBF_recv=nullptr;
     RBF_sender=nullptr;
     cout<<endl<<"Not: "<<Not<<" Nbf: "<<Nbf<<" Ncc: "<<Nc_sender<<" maxOnes: "<<maxOnes<<endl<<endl;
     strings_choices=new std::vector<block>(Not);
     init_sender(port);
     //$$$
     crypt = new crypto(m_nSecParam, constSeed);
     cout<<"creating choices"<<endl;
     bitvector=new BitVector(Not);
     cout<<"Num of ones= "<<numOfOnes<<endl;
     Bitstring* b1=new Bitstring(Not,numOfOnes,crypt);
     for (int i=0;i<Not;i++)
                (*bitvector)[i]=b1->get_bit(i);
     delete b1;

     std::cout<<"end c'tor"<<std::endl;
}
crypto* P0::getCrypto(){
   return crypt;
}


int P0::Nbf=0;
int P0::seeds_num=0;
Bitstring* P0::BF=nullptr;
int P0::items_size=0;
std::set<int>* P0::h_kokhav=nullptr;
int P0::Not=0;
int P0::Nc_sender=0; 
int P0::string_length=0;
int P0::maxOnes=0;
std::vector<int>* P0::items=nullptr; 



void P0::set_items(std::vector<int>* items,int items_size){
      P0::items=items;
      P0::items_size=items_size;
}
void P0::set_Nbf(int Nbf){
      P0::Nbf=Nbf;
}

void P0::set_BF(Bitstring* BF){
      P0::BF=BF;
}

void P0::set_Not(int n){
      Not=n;
}

void P0::set_Nc_sender(int Nc){
      P0::Nc_sender=Nc;
}

void P0::set_string_length(int string_length){
      P0::string_length=string_length;
}

void P0::set_maxOnes(int maxOnes){
      P0::maxOnes=maxOnes;
}

void P0::set(vector<int>* items,int items_size,int Nbf,int n,int Nc,int string_length,int maxOnes,int seeds_num){
	
    cout<<"setting the items"<<endl;
    set_items(items,items_size);
	 
    cout<<"setting Nbf"<<endl;
    set_Nbf(Nbf);
	 
    cout<<"setting BF"<<endl;
    BF=new Bitstring(Nbf);
	 
    cout<<"setting Not"<<endl;
    set_Not(n);
	 
    cout<<"setting Nc"<<endl;
    set_Nc_sender(Nc);
	 
    cout<<"setting k"<<endl;
    set_string_length(string_length);
	 
    cout<<"setting maxOnes"<<endl;
    set_maxOnes(maxOnes);
	 
    cout<<"setting seeds num"<<endl;
    P0::seeds_num=seeds_num;

	
}

void P0::delete_bf(){
	if (BF!=nullptr) delete BF;
}


void P0::init_sender( int port){
     init_sender(&s,port);
}
	
void P0::init_sender(struct sender* s, int port){
  
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
      
     printf("Server is alive and waiting for socket connection from client.\n");
     listen(s->sock, 1); 

     s->len = sizeof(s->serv_name);
     s->connect_sock = accept(s->sock, (struct sockaddr *)&s->serv_name, &s->len);

}

void P0::init_receiver(const char* ip,int port){ 
     init_receiver(&recv,ip,port);
}

void P0::init_receiver(struct receiver* recv,const char* ip,int port){ 
	
     printf("Client is alive and establishing socket connection.\n");
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

void P0::compute_keys(std::set<int>* h_kokhav){ 
    /*keys=new block[items_size];
    for (int i=0;i<items_size;i++){
		keys[i]=toBlock(0,0);

	    Bitstring* was_or_not=new Bitstring(Nbf);
        std::set<int>::iterator begin=h_kokhav[i].begin();
        std::set<int>::iterator end=h_kokhav[i].end();
        for (;begin!=end;begin++){
            
            int index=(*begin);		
		    if ((*was_or_not).get_bit(index)==0){
			    (*was_or_not).set_bit(index,1);
		        keys[i]=keys[i]^((*strings_choices)[func_recv[index]]);
							
        	}
		}
		//if (i==0) cout<<i<<") "<<keys[i]<<endl;
		delete was_or_not;
	}
   if (strings_choices!=nullptr) delete strings_choices; 
        std::cout<<"keys were computed"<<std::endl;*/
}


void P0::recv_keys(block* keys_recv){
	read_as_a_receiver(keys_recv,sizeof(block)*items_size);
	cout<<0<<") "<<keys_recv[0]<<endl;
}

void P0::set_sub_group(){
}

void P0::send_sub_group(){
}



void P0::recv_R(){
    read_as_a_sender(&Nr_sender,sizeof(int));
    cout<<"From P0: Nr: "<<Nr_sender<<endl<<endl;
    R_sender=new int[Nr_sender];
    read_as_a_sender(R_sender,sizeof(int)*Nr_sender);
    read_as_a_sender(&r_sender,sizeof(block));
    cout<<"From P0: r* (P0): "<<r_sender;
    cout<<endl;
    cout<<"R and r were received"<<endl;
}

//Changed
int P0::check_if_to_abort1(){
}



void P0::recv_func(){
     func_sender=new int[Nbf];
     read_as_a_sender(func_sender,sizeof(int)*Nbf);
     //for (int i=0;i<Nbf;i++) cout<<i<<")"<<func_sender[i]<<" ";
     cout<<"func was received"<<endl;
}

int P0::check_if_to_abort2(){
}

void P0::create_BF(std::set<int>* h_kokhav,unsigned int* hash_seeds, block seed){
    functions::create_BF( h_kokhav,*items, items_size, (*BF),Nbf,seeds_num,hash_seeds,seed);
    cout<<"BF was created"<<endl;
}	
	
void P0::create_RBF_sender(synchGBF* test){

}	


void P0::write_as_a_sender(void* msg,unsigned long size){
    unsigned long left=size;
    unsigned long n;
    cout<<left;
    while (left){
           n=write(s.connect_sock, &((char*)msg)[size-left],left);
           left-=n;
           cout<<left;
    }
}

void P0::write_as_a_sender(sender* snd,void* msg,unsigned long size){
    unsigned long left=size;
    unsigned long n;
    cout<<left;
	
    while (left){
           n=write(snd->connect_sock, &((char*)msg)[size-left],left);
           left-=n;
	   cout<<left;
    }
	 
}

int P0::read_as_a_sender(void* msg,unsigned long size){
    unsigned long left=size;
    unsigned long n;

    memset(msg,0,(unsigned long)size);
	
    while (left){
           n=read(s.connect_sock, &((char*)msg)[size-left],left);
           left-=n;
    }
	
    return (size-left);	 
}

int P0::read_as_a_sender(sender* snd,void* msg,unsigned long size){
    unsigned long left=size;
    unsigned long n;
    cout<<0;
    memset(msg,0,(unsigned long)size);
	 
    while (left){
           cout<<0;
           n=read(snd->connect_sock, &((char*)msg)[size-left],left);
           left-=n;
    }
	
    return (size-left);	 
}
	 
void P0::write_as_a_receiver(void* msg,unsigned long size){
    unsigned long left=size;
    unsigned long n;
    while (left){

           n=write(recv.sock, &((char*)msg)[size-left],left);
           left-=n;
	 
    }
}

void P0::write_as_a_receiver(receiver* recv,void* msg,unsigned long size){
    unsigned long left=size;
    unsigned long n;
	
    while (left){

           n=write(recv->sock, &((char*)msg)[size-left],left);
           left-=n;
    }
}

int P0::read_as_a_receiver(void* msg,unsigned long size){
    unsigned long left=size;
    unsigned long n;
    memset(msg,0,(unsigned long)size);

    while (left){

           n=read(recv.sock, &((char*)msg)[size-left],left);
           left-=n;
    }
	
    return (size-left);	 
}

int P0::read_as_a_receiver(receiver* recv,void* msg,unsigned long size){
    unsigned long left=size;
    unsigned long n;
    memset(msg,0,(unsigned long)size);
	 
    while (left){

           n=read(recv->sock, &((char*)msg)[size-left],left);
           left-=n;
	 
    }
	
    return (size-left);	 
}

void P0::recv_C(){
    read_as_a_receiver(&Nc_recv,sizeof(int));
    cout<<"From P0: Nc: "<<Nc_recv<<endl<<endl;
    sub_group_recv=new int[Nc_recv];
    block seed_recv;
    read_as_a_receiver( &seed_recv, sizeof(block));
    functions::get_sub_group(sub_group_recv,Not,Nc_recv,seed_recv);

}


//Changed
int P0::check_if_to_abort(){

    if (Nc_recv>Not-Nbf){
       cout<<"Nc>Not-Nbf: session has been stopped"<<endl;
       close(recv.sock);
       return 1;
    }
    else{
       cout<<"Nc<=Not-Nbf: session continues"<<endl;
       return 0;
    }
}


void P0::compute_R_r(){
    func_recv=new int[Not];
    for (int i=0;i<Not;i++) func_recv[i]=i;
    R_recv=new std::vector<int>;
    functions::compute_R_r(sub_group_recv,func_recv,bitvector,Nc_recv,Not,*strings_choices,r_recv,*R_recv);  
    if (sub_group_recv!=nullptr) delete [] sub_group_recv;
    Nr_recv=(*R_recv).size();
    cout<<"Nr: "<<Nr_recv<<endl<<endl;
    cout<<"r* (P0): "<<r_recv;
    cout<<endl<<endl;
    cout<<"r and R were computed"<<endl;
}

void P0::send_R_r(){
    write_as_a_receiver(&Nr_recv,sizeof(int));		
    int arr[Nr_recv];
    for (int i=0;i<Nr_recv;i++)
    arr[i]=(*R_recv)[i];		
    if (R_recv!=nullptr) delete R_recv;
    write_as_a_receiver(arr,sizeof(int)*Nr_recv);
    write_as_a_receiver(&r_recv,sizeof(block));
    cout<<"r and R were sent"<<endl;
}


void P0::arrange_the_indexes(){
   functions::arrange_the_indexes(*BF,Nbf,bitvector,func_recv,Not,Nc_recv,crypt); 
   delete bitvector;
   cout<<"indexes were arranged"<<endl;   
}

void P0::create_RBF_receiver(synchGBF* test){

   test->XORordered(strings_choices,func_recv);
   cout<<"rbf was created"<<endl;
   if (func_recv!=nullptr) delete [] func_recv;
   if (strings_choices!=nullptr) delete strings_choices;
   
}


void P0::send_func(){

   write_as_a_receiver(func_recv,sizeof(int)*Nbf);

   cout<<"func was sent"<<endl;	
}
	

P0::~P0(){

   close(recv.sock);
   close(s.connect_sock);
   close(s.sock);
   delete crypt;
}


BitVector* P0::getChoices() const{
    return bitvector;
}

std::vector<block>& P0::getStringsReceiver(){
    return *strings_choices;
}

std::vector<std::array<block, 2>>& P0::getStrings(){
    return *strings;
}

int P0::getK() {
    return string_length;
}

block* P0:: get_RBF_recv() const{
    return RBF_recv;
}

int P0::getNot() const{
    return Not;
}

block* P0:: get_RBF_sender() const{
    return RBF_sender;
}


void P0::set_GBF1_STAR(block* GBF1_STAR){
    this->GBF1_STAR=GBF1_STAR;
}

void P0::set_h_kokhav(std::set<int>* h_kokhav){
    P0::h_kokhav=h_kokhav;
}

std::set<int>* P0::get_h_kokhav(){
    return h_kokhav;
}

block* P0::get_GBF1_STAR(){
    return GBF1_STAR;
}

int P0::getSeedsNum() const{
    return seeds_num;
}

int* P0::get_func_recv(){
    return func_recv;
}

