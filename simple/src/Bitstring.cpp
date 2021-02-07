#include "Bitstring.h"

Bitstring::Bitstring(int length, int numOfOnes,crypto* c):length(length){
    bytes=(int)(ceil((double)length/8));
	
    //std::cout<<"creating arr"<<bytes<<std::endl;
    arr=new BYTE[length];
    for (int i=0;i<length;i++) arr[i]=0;
	
    if (numOfOnes!=0){

        //auto start = std::chrono::steady_clock::now();
        //std::cout<<"creating indexes"<<bytes<<std::endl;	
	int* indexes=new int[length];
	for (int i=0;i<length;i++) indexes[i]=i;
	int counter=0;
	int n=numOfOnes;
	
	//std::cout<<"creating random vals"<<bytes<<std::endl;	
	uint64_t* r=new uint64_t[n];
	c->gen_rnd((BYTE*)r,8*n);
	while (n){

	      int rand=(r[counter]%(length-counter))+counter;
	      int temp=indexes[rand];
	      indexes[rand]=indexes[counter];
	      indexes[counter]=temp;
	      counter++;
	      n--;
	}
	
        delete [] r;
	for (int i=0;i<numOfOnes;i++)
              set_bit(indexes[i],1);
        delete [] indexes;
        //std::cout<<"first 100 bits: ";
        //for (int i=0;i<100;i++) std::cout<<(int)arr[i]<<" ";
        //std::cout<<std::endl;
        //auto end = std::chrono::steady_clock::now();
        //std::cout<<"Random Bitstring generation time = "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()<<std::endl;	
	//int sum=0;
        //for (int i=0;i<length;i++) sum+=get_bit(i);
        //std::cout<<"sum in Bitsring: "<<sum<<", should be? "<<numOfOnes <<std::endl;
        //sum=0;
        //for (int i=0;i<length/2;i++) sum+=get_bit(i);
        //std::cout<<"sum in first half of Bitsring: "<<sum<<", should be~? "<<numOfOnes/2 <<std::endl;

  }
     

}

/**
 * replacing the bit at the requested place 
 *
 * @param place the place of the bit which need to be replaced 
 * @param new_val the bit to set
 */
 
void Bitstring::set_bit(int place, int new_val){
    arr[place]=new_val;

}

/**
 * searching for the bit at the requested place
 *
 * @param place the requested place
 * @return the bit at the place
 */
 
int Bitstring::get_bit(int place) const{
    return arr[place];
}

int Bitstring::get_length() const{ return length;}

BYTE* Bitstring::getArr(){
     return arr;
}

Bitstring::~Bitstring(){
     if (arr!=nullptr) delete [] arr;
}

std::ostream& operator<<(std::ostream& os,const Bitstring& b){
     for (int i=0;i<b.length;i++) os<<b.get_bit(i);
     return os;
}
