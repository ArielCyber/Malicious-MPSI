#include "../src/MurmurHash.h"
#include "../src/kos.h"
#include "../src/Party.h"
#include "../src/Functions.h"
#include "../src/P0.h"
#include "../src/Pi.h"
#include "../src/Bitstring.h"

int main(int argc, char** argv)
{	
  
	//-- step 1-- getting parameters

	//getting our protocol vals

	//cout<<"getting the protocols  vals"<<endl;
	int player=atoi(argv[1]);
	const char* cfg_file=argv[2];
	string ip_file,values_file;
	uint32_t numOfOnes,Nc,maxOnes,Nbf,seeds_num;
	uint64_t numOTs;
        functions::read_protocol_parameters(cfg_file,&seeds_num,&Nbf,&numOTs,&numOfOnes,&Nc,&maxOnes,values_file,ip_file);
	uint32_t k=128;
	uint32_t bytes = int(k/8);
	vector <int> items;
	//cout<<"values:"<<values_file;
	functions::get_values(values_file,items);
	int items_size=items.size();
	//cout<<"data size: "<<items.size()<<endl;

	//getting the ips of the players
	std::vector <string> ips;
	int parties;
        functions::set_ips(ips,ip_file,&parties);

	//setting ports
	int** ports=new int*[parties];
        functions::set_ports(ports,player,parties);

	//crypto var
	//Not supposed to be here. Needs to be an unique input which is given by each player and which the other players don't know.
        const char* m_cConstSeed[] = {"437398417012387813714564100","15657566154164333269053561","345353656745745679457845","45346565757754721232645745","565657776767688668686434",
                                      "232324546565743545454542323","11117812368245852473965742","283728362864268427537523","83467365736745325637547237","766432973541628309724783",
                                      "874563278200376642634859957","43875393272645475958373635","276435474389482746588290","37363534628193847564839029","484389723762187219003948",
                                      "437398417012387813714564100","15657566154164333269053101","345353656745745679457102","45346565757754721232645103","565657776767688668686104",
                                      "232324546565743545454542105","11117812368245852473965106","283728362864268427537107","83467365736745325637547108","766432973541628309724109",
                                      "874563278200376642634859110","43875393272645475958373111","276435474389482746588112","37363534628193847564839113","484389723762187219003114",
                                      "437398417012387813714564115","15657566154164333269053116","345353656745745679457117","45346565757754721232645118","565657776767688668686119",
                                      "232324546565743545454542120","11117812368245852473965121","283728362864268427537122","83467365736745325637547123","766432973541628309724124",
                                      "874563278200376642634859125","43875393272645475958373126","276435474389482746588127","37363534628193847564839128","484389723762187219003129",
                                      "437398417012387813714564130","15657566154164333269053131","345353656745745679457132","45346565757754721232645133","565657776767688668686134",
                                      "232324546565743545454542135","11117812368245852473965136","283728362864268427537137","83467365736745325637547138","766432973541628309724139",
                                      "874563278200376642634859140","43875393272645475958373141","276435474389482746588142","37363534628193847564839143","484389723762187219003144",
                                      "384837476629203847463894045","38297437239823723298329749","483732632839237292383930","39834738473921742023832983","382732873621519128474908"};

        //for P0's instances- in order that for each pi- p0 will generate an unique bitstring
        const char* m_cConstSeed_p0[] ={"837398417082347831734964176","23654364655126333279080012","645789052310784374431129","92126573757754721332691149","345278771167685662186414", 
                                        "232324543655746546454542323","36928942743746286342987426","908264524193774629302826","74363294372394389237329172","398438274236728346823476",
                                        "348734975749574385793474873","43485748374939454567345683","438423485920232302439434","59838743766432764823467824","328984234723847823748347",
                                        "547534894573495739457834572","49549854374382747923492344","548503845834959834585490","83479574957495748574857458","493802938483473874837483",
                                        "129230438498359849584958945","43940394344923483904834430","438493723497347347832749","47384792342364934387643874","389430274234739847834783",
                                        "752145486432546487875421254","43847394729347387483748374","430423948395894898889384","43943094302492384384839484","893473827643746272882737",
                                        "843463746273423546235463544","74637264728343546354627343","983473642376434379483744","56347637482636437229103284","743846384628374736476763",
                                        "192193829388473847387484387","43488293271873236628898374","434878273283728738233827","54859849387372376233627447","438493894839207423478344",  
                                        "125368214578788854542258412","35215487878565656822564125","232843943758247548578475","12198373947384782374837484","483947238973347283478347",  
                                        "349384927374873878573482948","12343943584985498554898999","939849357348728734384777","78312683615436546345364554","343455453656565766767566",  
                                        "530830428374827349738473847","12139204930483489348933029","892837248374837823956234","43940239493485838948372327","438472397489237483743447", 
                                        "439403825829347123023943849","10314839482340283408324303","549504950340584584958945","54504550453954545454545455","438985948058495849584959",  
                                        "434893726392574985748957485","83727362476282910374732283","984398427384723646729122","23298438748374857485748575","434983985478574875487584"}; 
                                        
	uint32_t m_nSecParam = 128;

        //creating timing file 
        ifstream ifile("timing.csv");
	fstream fout;
        fout.open("timing.csv", ios::out);
        auto start = std::chrono::steady_clock::now();
	
        //creating communication file
        ifstream i_file("com.csv");
        fstream fout_com;
        fout_com.open("com.csv", ios::out);

        synchGBF* test=new synchGBF(Nbf,256);
  
  
	//if p0
        if (player==0){

	  //fout<<"Setup, Offline, Online,\n";
          fout<<"Step 2 (setup), Steps 3+4(Offline AppROT), Step 6 (BF), Steps 7+10 ( Online AppROT), Step 11 (XOR), Step 12(output)"<<endl;

		
		
	  //--step 2-- hash seed agreement

          //auto test1 = std::chrono::steady_clock::now();
     
          //cout<<"creating p0"<<endl;
     
	  //init P0 common vars
	  P0::set(&items,items_size,Nbf,numOTs,Nc,bytes,maxOnes,seeds_num);
          //creating vector of P0's instances
          std::vector<P0*> P0_s;
          //resizing it
          P0_s.resize(parties-1);
          //std::cout<<"p0 size:"<<P0_s.size()<<std::endl;
          //auto test2 = std::chrono::steady_clock::now();

          std::vector<std::thread*> threads;
      
          //creating P0's instance for each Pi!=0
	  for (int i=1;i<parties;i++){
                //setting different port and crypto vars
                threads.push_back(new thread(functions::creating_p0_instance,(i-1),ports[i][0],&P0_s,numOfOnes,m_nSecParam,(uint8_t*) m_cConstSeed_p0[i]));
	  }

          for (auto& t:threads) t->join();
          for (auto& t:threads) delete t;  
          threads.clear();
  
          //auto test3 = std::chrono::steady_clock::now();

          //cout<<"sharing secret"<<endl;
      
          //the keys of p0 for each pi
          block* keys=new block[parties-1];
          //the keys of each pi for p0
          block* keys_recv=new block[parties-1];
      
          //p0 generates keys from the crypto var
          //After that, he gets/sends the keys from/to the rest of the parties
          functions::secret_sharing_seed_p0(P0_s,parties,keys,keys_recv,&fout_com);
       
          //auto test4 = std::chrono::steady_clock::now();
         
          //cout<<"setting seed-commit"<<endl;
          //seed and commit of p0
	  block seed;
	  Commit commit;

          //crypto var of p0 for creating seed and commit
          crypto *crypt_ = new crypto(m_nSecParam, (uint8_t*) m_cConstSeed[0]);
          //generating random seed using the crypto var, and from this seed generating Commit object
	  functions::set_seed_commit(commit, seed, crypt_);
       
          //auto test5 = std::chrono::steady_clock::now();
	    
          //agreement
          //cout<<"seeds agreement"<<endl;
          //checking if there is agreement-
          //sending and receiving commit+key to/from each pi , 
          //checking if the received commit is ok, according to the seed which was received
          //and than computing common seed by xoring all the others seeds
          //the common seed is now in the seed object, and we will use it for creating seeds for the hush functions
          functions::seeds_agreement_p0(P0_s,commit,seed,&fout_com);

          //computing seeds for the hush function
          //taking the common seed which was clculated before
          //taking "messages" from 0 to the num of seeds which we need
          //activating on it AES function
          //cout<<"computing seeds"<<endl;
	  unsigned int* hash_seeds=functions::get_seeds(seeds_num,seed);

          //auto test6 = std::chrono::steady_clock::now();

          /*
	  std::cout<<"1-2 = "<<std::chrono::duration_cast<std::chrono::milliseconds>(test2-test1).count()<<std::endl;
          std::cout<<"2-3 = "<<std::chrono::duration_cast<std::chrono::milliseconds>(test3-test2).count()<<std::endl;
          std::cout<<"3-4 = "<<std::chrono::duration_cast<std::chrono::milliseconds>(test4-test3).count()<<std::endl;
          std::cout<<"4-5 = "<<std::chrono::duration_cast<std::chrono::milliseconds>(test5-test4).count()<<std::endl;
          std::cout<<"5-6 = "<<std::chrono::duration_cast<std::chrono::milliseconds>(test6-test5).count()<<std::endl;
	  */
		
          start=functions::get_duration(start,fout);


          //--steps 3+4+5-- offline apport + AES
      
          //secret sharing AES
          //p0 calculates the secret using AES and the keys from the secret sharing phase-
          //for each key- takes the key and each "message" from 0 to Nbf and activates AES function
          //After finishing, all the results are xored

          //cout<<"secret aes"<<endl;

          if(parties>2)
          {
             cout<<"Secret-sharing executed"<<endl;
             threads.push_back(new thread(functions::secret_sharing_aes,parties,Nbf,test,keys,keys_recv));
          }

          //offline apport
          //performin Not OT's with each pi as a receiver and as a sender
		
          //as a sender-	
          //1) getting 2*Not words	
          //2) calculating sub_group of random numbers of size Nc using AES function-
	  //   getting random key using our crypto var, taking messages from 0 to Nc , and activating AES on it
          //   After getting the AES numbers, activating modulo Not on them, in order to get random vals at range 0,..,Not
          //3) sending the sub_group
          //4) receiving R and r*- indexes from the sub_group which has bit 0 at the other player, and their xor calculation
          //5) xoring all the 0's strings in the places of the received indexes
	  //   checking if the xor of this indexes is really what we have received and checking if the number of 0's is enougth
		
          //as a receiver-
	  //1) getting Not words
          //2) getting the sub group
          //3) calculating R and r*- 
	  //   go over the sub group and check which indexes has bit 0, 
	  //   this indexes will be put in the R array, 
          //   r will be the xoring result of the corresponding strings
	  //4) sending R and r*
      
          //cout<<"offline apport"<<endl;
          std::mutex mu;
	  for (int i=0;i<(parties-1);i++){
              int other_player=i+1;
	      threads.push_back(new thread(functions::run_offline_apport,P0_s[i],&ips,ports,player,other_player,&fout_com,&mu));
	  }

          for (auto& t:threads) t->join();
	  for (auto& t:threads) delete t;
	  threads.clear();

	  start=functions::get_duration(start,fout);


		

          //--steps 6-- compute BF

          //cout<<"creating bf"<<endl;

          //creating bloom filter using AES
          //setting the common seed as key,and the item val xored with numbers from 0 to the hush functions num as messages
          //by activating AES on the key and messages, we are getting random numbers 
          //and by activating modulo Nbf we getting indexes in the range of the bloom filter
          //setting to 1 each index in the bloom filter, and adding each index to its h_kokhav group
	  std::set<int>* h_kokhav=new std::set<int>[items_size];
	  P0::create_BF_threads(h_kokhav,hash_seeds, seed);

          delete [] hash_seeds;

          start=functions::get_duration(start,fout);

	  //--steps 7+10-- online apport
		
          //as a sender-	
          //receiving the function
		
          //as a receiver-
          //creating function according to the BF bits and sending it to the other party

          //cout<<"online apport"<<endl;

	  for (int i=0;i<(parties-1);i++){
                threads.push_back(new thread(functions::run_online_apport,P0_s[i],test,&fout_com,&mu));
	  }

          for (auto& t:threads) t->join();
	  for (auto& t:threads) delete t;
	  threads.clear();

          //cout<<"end online appROT P0"<<endl;

          //freeing memory
          P0::delete_bf();

          start=functions::get_duration(start,fout);

	  //--step 11-- comulative gbfs
          //getting the GBFs from the opponent parties and xoring them 
          //cout<<"computing gbf p0"<<endl;
	  block* GBF=functions::compute_gbf1(P0_s,Nbf,test,&fout_com);
          start=functions::get_duration(start,fout);

	  //--step 12-- output
          //going over the indexes of each h*, xoring them, and checking if the sum is 0. If it is-print the item.
          //cout<<"getting the intersection"<<endl;
	  functions::get_intersection( h_kokhav,bytes,GBF);
          //cout<<"End of P0"<<endl;
		
	  start=functions::get_duration(start,fout);

          fout<<endl;
	  fout.close();
          delete crypt_;  

          //std::cout<<"exit";
		
      }

      // if pi
      else{

          //fout<<"Setup, Offline, Online\n";
          fout<<"Step 2, Steps 3+4+5, Step 6 (BF), Steps 7+10 ( Online AppROT), Step 8, Step 9, Step 11"<<endl;

          //init Pi                                                                                                                                                                                                                         Pi *pi=new Pi(items,items_size,numOTs,Nbf,Nc,k,maxOnes,seeds_num,numOfOnes,crypt,parties,ports,ips,player);  
          //setting all its attributes
          Pi *pi=new Pi(items,items_size,numOTs,Nbf,Nc,k,maxOnes,seeds_num,numOfOnes,m_nSecParam, (uint8_t*) m_cConstSeed[player],parties,ports,ips,player);  

	      
	      
	  //--step 2-- hash seed agreement
	      
	  //setting seed and commit according to the crypto var

	  auto start = std::chrono::steady_clock::now();

          crypto* crypt=pi->getCrypto();
	  block seed;
	  Commit commit;
	  functions::set_seed_commit(commit, seed, crypt);

          //seed secret sharing
	      
	  //storage for all the keys
	      
	  block* keys=new block[parties-1];
	  block* keys_recv=new block[parties-1];
            
          //generating keys for each pi. Sending these keys and receiving keys from each other party.
          functions::secret_sharing_seed_pi(pi,parties,crypt,player,keys,keys_recv,&fout_com);

	  //agreement
	  //sending and receiving seed-commit to/from each party. 
	  //calculating the common seed
          functions::seeds_agreement_pi(pi, parties,commit,seed,player,&fout_com);
	      
          //calculating seeds from the common seed using AES
	  unsigned int* hash_seeds=functions::get_seeds(seeds_num,seed);

          start=functions::get_duration(start,fout);


	      
	      
	  //--steps 3+4-- offline_apport
          if (parties>2){
                 //cout<<"Offline_app_ROT executed"<<endl;
                 std::mutex mu;
		 //running offline apport as a receiver and as a sender
	         thread* t1=new thread(functions::run_offline_apport,pi,&ips,ports,player,0,&fout_com,&mu);

                 //--step 5-- distribution of gbf shares
                 //cout<<"Secret-sharing exectued"<<endl;
		 //calculating the secret using all the keys we have been received and AES
	         thread* t2=new thread(functions::secret_sharing_aes,parties,Nbf,test,keys,keys_recv);

                 t1->join();
                 t2->join();

                 delete t1;
                 delete t2;
          }
	      
          else//only 2 parties, no secret-sharing
          {
                 //cout<<"Offline_app_ROT executed"<<endl;
                 std::mutex mu;
                 functions::run_offline_apport(pi,&ips,ports,player,0,&fout_com,&mu);
          }

          //memory allocations for online phase, needed for rerandomization
          block* Y= new block[items_size];
          block* GBF1=new block[Nbf];

          start=functions::get_duration(start,fout);
 

          //Online phase

          //--step 6-- compute BF

	  std::set<int>* h_kokhav=new std::set<int>[items_size];
	  pi->create_BF_threads(h_kokhav,hash_seeds, seed);

          delete [] hash_seeds;

          start=functions::get_duration(start,fout);

          //--step 7+10-- online apport
          std::mutex mu;
          functions::run_online_apport(pi,test,&fout_com,&mu);
          //cout<<"end online appROT pi"<<endl;
          start=functions::get_duration(start,fout);

          //--step 8-- compute codewords
          //cout<<"re_randomize"<<endl;
	  //setting Y and creating randomizeD gbf
          GBF1=functions::re_randomization(items, Y, items_size , Nbf, bytes ,  h_kokhav , crypt, pi, GBF1, test);

          start=functions::get_duration(start,fout);

	  //--step 11-- comulative gbfs
          //sending the GBF to P0
          //cout<<"comulative_gbf_pi"<<endl;
	  functions::comulative_gbf_pi(pi,Nbf,GBF1,&fout_com);
	      
          start=functions::get_duration(start,fout);
          fout<<endl;
          fout.close();
          //delete crypt;  
	  //cout<<"exit"<<endl;
 

     }

     for (int i=0;i<parties;i++){
		delete [] ports[i];
     }
     delete [] ports;
}
