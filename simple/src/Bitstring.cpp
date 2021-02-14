#include "Bitstring.h"

Bitstring::Bitstring(int length, int numOfOnes,crypto* c):length(length){
	
    arr=new BYTE[length];
    for (int i=0;i<length;i++) arr[i]=0;
	
    if (numOfOnes!=0){

	int* indexes=new int[length];
	for (int i=0;i<length;i++) indexes[i]=i;
	int counter=0;
	int n=numOfOnes;
	
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
  }
     

}


Bitstring::Bitstring(int length, BYTE* arr):length(length){
	this->arr=new BYTE[length];
	for(int i=0;i<length;i++) this->arr[i]=arr[i];
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
