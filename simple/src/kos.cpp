
#include "kos.h"


void TwoChooseOne(Role role, int numThreads, std::string ip,Party* player)
{
        //crypto* crypt=player->getCrypto();
	bool randomOT = true;

	//auto numOTs = player->getNot();

	// get up the networking
	auto rr = role == Role::Sender ? SessionMode::Server : SessionMode::Client;
	IOService ios;
	Session  ep0(ios, ip, rr);
	PRNG prng(sysRandomSeed());

	// for each thread we need to construct a channel (socket) for it to communicate on.
	std::vector<Channel> chls(numThreads);
	for (int i = 0; i < numThreads; ++i)
		chls[i] = ep0.addChannel();

	//Timer timer, sendTimer, recvTimer;
	//timer.reset();
	//auto s = timer.setTimePoint("start");
	//sendTimer.setTimePoint("start");
	//recvTimer.setTimePoint("start");
        auto start = std::chrono::steady_clock::now();
    


	std::vector<KosOtExtSender> senders(numThreads);
	std::vector<KosOtExtReceiver> receivers(numThreads);
	


#ifdef LIBOTE_HAS_BASE_OT
    //!!!!!

	// Now compute the base OTs, we need to set them on the first pair of extenders.
	// In real code you would only have a sender or reciever, not both. But we do 
	// here just showing the example. 
	if (role == Role::Receiver)
	{
        DefaultBaseOT base;
		std::array<std::array<block, 2>, 128> baseMsg;
		base.send(baseMsg, prng, chls[0], numThreads);
		receivers[0].setBaseOts(baseMsg, prng, chls[0]);
		
		//receivers[0].genBaseOts(prng, chls[0]);
	}
	else
	{

        DefaultBaseOT base;
		BitVector bv(128);
		std::array<block, 128> baseMsg;
		bv.randomize(prng);
		base.receive(bv, baseMsg, prng, chls[0], numThreads);
		senders[0].setBaseOts(baseMsg, bv,chls[0]);
	}
#else
    //!!!!!

#endif 

	// for the rest of the extenders, call split. This securely 
	// creates two sets of extenders that can be used in parallel.
	for (auto i = 1; i < numThreads; ++i)
	{
        //cout<<"Can I split?"<<endl;
		if (role == Role::Receiver)
			receivers[i] = receivers[0].splitBase();
		else
			senders[i] = senders[0].splitBase();
        //cout<<"Split"<<endl;
	}


	auto routine = [&](int i)
	{
		// get a random number generator seeded from the system
		PRNG prng(sysRandomSeed());

		if (role == Role::Receiver)
		{
                //cout<<"Number of OTs"<<numOTs<<endl;
			BitVector& choice=*(player->getChoices());
			auto& msgs=*(player->getStringsReceiver());

			// construct the choices that we want.
			// in this case pick random messages.
			//choice.randomize(prng);

			// construct a vector to stored the received messages. 
			if (randomOT)
			{

				// perform  numOTs random OTs, the results will be written to msgs.
     
                                 receivers[i].receive(choice, msgs, prng, chls[i]);			
			}
			else
			{

				// perform  numOTs chosen message OTs, the results will be written to msgs.
			//	receivers[i].receiveChosen(choice, msgs, prng, chls[i]);
			}

			
		}
		else
		{
			// construct a vector to stored the random send messages. 

			// if delta OT is used, then the user can call the following 
			// to set the desired XOR difference between the zero messages
			// and the one messages.
			//
			//     senders[i].setDelta(some 128 bit delta);
			//
                        auto& msgs=*(player->getStrings());
                              
			if (randomOT)
			{
				// perform the OTs and write the random OTs to msgs.
                                senders[i].send(msgs, prng, chls[i]);				
			}
			else
			{
				// Populate msgs with something useful...
				prng.get(msgs.data(), msgs.size());

				// perform the OTs. The receiver will learn one
				// of the messages stored in msgs.
				senders[i].sendChosen(msgs, prng, chls[i]);
			}


			
		}
			
	};

	//senders[0].setTimer(sendTimer);
	//receivers[0].setTimer(recvTimer);

	std::vector<std::thread> thrds(numThreads);
	for (int i = 0; i < numThreads; ++i)
		thrds[i] = std::thread(routine, i);

	for (int i = 0; i < numThreads; ++i)
		thrds[i].join();

    cout<<"End-OT??"<<endl;

	//auto e = timer.setTimePoint("finish");
	//auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count();

    auto end = std::chrono::steady_clock::now();
    std::cout<<"OT time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()<<std::endl;	
	

	//auto com = (chls[0].getTotalDataRecv() + chls[0].getTotalDataSent()) * numThreads;
	
	
}



/*
int main(int argc, char** argv)
{
	unsigned int m_nSecParam = 128;
	const char* m_cConstSeed="98979767865764";
	int numOTs=40000;
	int numOfOnes=20000;
	int Nbf=10000;
	int Nc=1000;
	int k=16;
	int maxOnes=500;
	int seeds_num=5;
	
	crypto *crypt = new crypto(m_nSecParam, (uint8_t*) m_cConstSeed);
	
	BitVector* choice=new BitVector(numOTs);
	Bitstring* b1=new Bitstring(numOTs,numOfOnes,crypt);
        for (int i=0;i<numOTs;i++){
		(*choice)[i]=b1->get_bit(i);
        }		
	delete b1;
		
        std::vector<block> msgs;
	msgs.resize(numOTs);

	std::vector<std::array<block, 2>> msgs1;
	msgs1.resize(numOTs);

        std::vector<int> v;	
	
	P* P1=new P(v,0,numOTs,Nbf,Nc,k,msgs1,maxOnes,choice,msgs,seeds_num);

	if (0==atoi(argv[1])){

	   TwoChooseOne<KosOtExtSender, KosOtExtReceiver>(Role::Sender,1,std::string("localhost:1212"),crypt,*P1);
   
	   std::cout<<0<<std::endl;
	   std::cout<<msgs1[0][0]<<std::endl;
	   std::cout<<msgs1[0][1]<<std::endl;
	   std::cout<<1<<std::endl;
			
	   std::cout<<msgs1[1][0]<<std::endl;
	   std::cout<<msgs1[1][1]<<std::endl;	
	   std::cout<<2<<std::endl;

	   std::cout<<msgs1[2][0]<<std::endl;
	   std::cout<<msgs1[2][1]<<std::endl;
	   std::cout<<3<<std::endl;
			
	   std::cout<<msgs1[3][0]<<std::endl;
	   std::cout<<msgs1[3][1]<<std::endl;

	}
        else{
	   TwoChooseOne<KosOtExtSender, KosOtExtReceiver>(Role::Receiver,1,std::string("localhost:1212"),crypt,*P1);
	   std::cout<<(*choice)[0]<<": "<<msgs[0]<<std::endl;
	   std::cout<<(*choice)[1]<<": "<<msgs[1]<<std::endl;
	   std::cout<<(*choice)[2]<<": "<<msgs[2]<<std::endl;
           std::cout<<(*choice)[3]<<": "<<msgs[3]<<std::endl;
        }
	
        return 0;
}
*/
