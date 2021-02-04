#include "Pi.h"

Pi::Pi(std::vector <int>& items,int  items_size,int Not,int Nbf,int Nc,int k,int maxOnes,int seeds_num,int numOfOnes,uint32_t m_nSecParam, uint8_t* constSeed,int parties,int ** ports, vector<string>& ips,int player):
Nc_sender(Nc),items(items),Not(Not),Nbf(Nbf),k(k),maxOnes(maxOnes),seeds_num(seeds_num),items_size(items_size),player(player),parties(parties){

	sub_group_sender=nullptr;
	sub_group_recv=nullptr;
	R_sender=nullptr;
	R_recv=nullptr;
	func_sender=nullptr;
	func_recv=nullptr;
	RBF_recv=nullptr;
	RBF_sender=nullptr;
        BF=nullptr;
	b=new BitVector(Not);

        crypt = new crypto(m_nSecParam, constSeed);	
	Bitstring* b1=new Bitstring(Not,numOfOnes,crypt);
        for (int i=0;i<Not;i++){
		(*b)[i]=b1->get_bit(i);
        }		
	delete b1;
	
	strings_choices=new std::vector<block>(Not);
	strings=new std::vector<std::array<block, 2>>(Not);
	 
	init_receiver(ips[0].c_str(),ports[0][1]);
	 
	senders=new struct sender[parties-1-player];
        receivers=new struct receiver[player-1];	
	 
	for (int p=1;p<player;p++)
             Pi::init_receiver(&receivers[p-1], ips[p].c_str(),ports[p][1]);			
		    
        for (int p=player+1;p<parties;p++)
             Pi::init_sender(&senders[p-player-1], ports[p][0]);
}


void Pi::get_zeros_ones(){
     functions::get_zeros_ones(b, Not,arr_indexes,crypt);
}

crypto* Pi::getCrypto(){
   return crypt;
}

void Pi::init_sender( int port){
     init_sender(&s,port);
}
	
void Pi::init_sender(struct sender* s, int port){
  
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

void Pi::init_receiver(const char* ip,int port){ 
     init_receiver(&recv,ip,port);
}

void Pi::init_receiver(struct receiver* recv,const char* ip,int port){ 
	
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

void Pi::set_sub_group(){
	
    //creating seed
    unsigned long x1;
    unsigned long x2;
    crypt->gen_rnd((BYTE*)&x1,8);
    crypt->gen_rnd((BYTE*)&x2,8);
    seed_sub_group=toBlock(x1,x2);
	
    //getting sub group
    sub_group_sender=new int[Nc_sender];
    functions::get_sub_group(sub_group_sender,Not,Nc_sender,seed_sub_group);
    cout<<"sub group was set"<<endl;
}

unsigned long Pi::send_sub_group(){
    unsigned long sum=write_as_a_sender( &Nc_sender,sizeof(int));		   
    sum+=write_as_a_sender( &seed_sub_group,sizeof(block));
    cout<<"sub group was sent"<<endl;
    return sum;
}


unsigned long Pi::recv_R(){
     unsigned long sum=read_as_a_sender(&Nr_sender,sizeof(int));
     cout<<"From Alice: Nr: "<<Nr_sender<<endl<<endl;
     R_sender=new int[Nr_sender];
     sum+=read_as_a_sender(R_sender,sizeof(int)*Nr_sender);
     sum+=read_as_a_sender(&r_sender,sizeof(block));
     cout<<"From Alice: r* (Alice): ";
     cout<<r_sender;
     cout<<endl;
     cout<<"R and r were received"<<endl;
     return sum;
}


int Pi::check_if_to_abort1(){

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
	cout<<endl<<"r*(Alice) :"<<r1;
        if (neq(r1,r_sender))
		    flag=1;
	cout<<endl<<endl;

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
	     cout<<"Ncc-Nr<=Nmaxones && r*(Alice)==r*(Alice): session continues"<<endl;
	     if (R_sender!=nullptr) delete [] R_sender;
	     return 0;
        }
     }
     return 0;
}


unsigned long Pi::recv_func(){
     func_sender=new int[Nbf];
     unsigned long sum=read_as_a_sender(func_sender,sizeof(int)*Nbf);
     cout<<"func was received"<<endl;
     return sum;
}


int Pi::check_if_to_abort2(){
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
	cout<<"the function and the sub group are disjoint: session continues"<<endl;
	i=0;
	write_as_a_sender(&i,sizeof(int));		
        return 0;	
    }
}
	

void Pi::create_BF_threads(std::set<int>* h_kokhav,unsigned int* hash_seeds, block seed){  
    BF=new Bitstring(Nbf); 
    functions::create_BF_threads( h_kokhav,&items, items_size, BF,Nbf,seeds_num,hash_seeds, seed);   
    cout<<"BF was created"<<endl;  
}


void Pi::create_RBF_sender(synchGBF* test){ 
    test->XORorderedBF(strings, func_sender,BF);
    cout<<"RBF was created"<<endl;
	if (func_sender!=nullptr) delete [] func_sender;  
    if (strings!=nullptr) delete strings;
}	


unsigned long Pi::write_as_a_sender(void* msg,unsigned long size){
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

unsigned long Pi::write_as_a_sender(sender* snd,void* msg,unsigned long size){
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

unsigned long Pi::read_as_a_sender(void* msg,unsigned long size){
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

unsigned long Pi::read_as_a_sender(sender* snd,void* msg,unsigned long size){
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
	 
unsigned long Pi::write_as_a_receiver(void* msg,unsigned long size){
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

unsigned long Pi::write_as_a_receiver(receiver* recv,void* msg,unsigned long size){
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

unsigned long Pi::read_as_a_receiver(void* msg,unsigned long size){
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

unsigned long Pi::read_as_a_receiver(receiver* recv,void* msg,unsigned long size){
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

unsigned long Pi::write_to_player(int other_player,void* msg,unsigned long size){
    if (other_player==0) return this->write_as_a_receiver(msg,size);
    else if (other_player<player) return this->write_as_a_receiver(&receivers[other_player-1],msg,size);
    else return this->write_as_a_sender(&senders[other_player-player-1],msg,size);
}

unsigned long Pi::read_from_player(int other_player,void* msg,unsigned long size){
    if (other_player==0) return read_as_a_receiver(msg,size);
    else if (other_player<player) return read_as_a_receiver(&receivers[other_player-1],msg,size);
    else return read_as_a_sender(&senders[other_player-player-1],msg,size);
}

unsigned long Pi::recv_C(){
    unsigned long sum=read_as_a_receiver(&Nc_recv,sizeof(int));
    cout<<"From P0: Nc: "<<Nc_recv<<endl<<endl;
    sub_group_recv=new int[Nc_recv];
    block seed_recv;
    sum+=read_as_a_receiver( &seed_recv, sizeof(block));
    functions::get_sub_group(sub_group_recv,Not,Nc_recv,seed_recv);
    return sum;
}


int Pi::check_if_to_abort(){

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

void Pi::compute_R_r(){
    func_recv=new int[Not];
    for (int i=0;i<Not;i++) func_recv[i]=i;
    R_recv=new std::vector<int>;
    functions::compute_R_r(sub_group_recv,func_recv,b,Nc_recv,Not,*strings_choices,r_recv,*R_recv);
    if (sub_group_recv!=nullptr) delete [] sub_group_recv;
    Nr_recv=(*R_recv).size();
    cout<<"Nr: "<<Nr_recv<<endl<<endl;
    cout<<"computed r*: "<<r_recv;
    cout<<endl<<endl;
    cout<<"r and R were computed"<<endl;

}

unsigned long Pi::send_R_r(){
    unsigned long sum=write_as_a_receiver(&Nr_recv,sizeof(int));		
    int arr[Nr_recv];
    for (int i=0;i<Nr_recv;i++)
    arr[i]=(*R_recv)[i];		
    if (R_recv!=nullptr) delete R_recv;
    sum+=write_as_a_receiver(arr,sizeof(int)*Nr_recv);
    sum+=write_as_a_receiver(&r_recv,sizeof(block));
    cout<<"r and R were sent"<<endl;
    return sum;
}


void Pi::arrange_the_indexes(){
   functions::arrange_the_indexes(*BF,Nbf,arr_indexes,&func_recv,Not,Nc_recv,crypt);
   cout<<"indexes were arranged"<<endl;   
}


void Pi::create_RBF_receiver(synchGBF* test){

   test->XORordered(strings_choices,func_recv);

   cout<<"rbf was created"<<endl;
   if (func_recv!=nullptr) delete [] func_recv;
   if (strings_choices!=nullptr) delete strings_choices;
   cout<<"rbf was created"<<endl;
}


unsigned long Pi::send_func(){
    unsigned long sum=write_as_a_receiver(func_recv,sizeof(int)*Nbf);
    cout<<"func was sent"<<endl;
    return sum;	
}

Pi::~Pi(){
   close(recv.sock);
   close(s.connect_sock);
   close(s.sock);

   for (int i=0;i<(player-1);i++){
       close(receivers[i].sock);
   }
		    
   for (int i=0;i<(parties-1-player);i++){
       close(senders[i].connect_sock);
       close(senders[i].sock);
   }

   delete crypt;
		
   delete [] senders;
   delete [] receivers;

   std::cout<<"deleting BF"<<std::endl;	
   if (BF!=nullptr) delete BF;
   std::cout<<"deleting RBF_recv"<<std::endl;
   if (RBF_recv!=nullptr) delete [] RBF_recv;
   std::cout<<"deleting RBF_sender"<<std::endl;	 
   if (RBF_sender!=nullptr) delete [] RBF_sender;

}


int Pi::getNot() const{
    return Not;
}

BitVector* Pi::getChoices() const{
    return b;
}

std::vector<block>& Pi::getStringsReceiver(){

    return (*strings_choices);
}

std::vector<std::array<block, 2>>& Pi::getStrings(){
    return (*strings);
}
int Pi::getK(){
    return k;
}

block* Pi:: get_RBF_recv() const{
    return RBF_recv;
}

block* Pi:: get_RBF_sender() const{
    return RBF_sender;
}

int Pi::getSeedsNum() const{
    return seeds_num;
}
