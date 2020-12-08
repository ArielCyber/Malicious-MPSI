# ifndef __BITSTRING_H
# define __BITSTRING_H

# include <cmath>
# include <iostream>
# include <ENCRYPTO_utils/crypto/crypto.h>

class Bitstring{

private:

    BYTE* arr;
    int length;
    int bytes;

public:

    Bitstring(int length, int numOfOnes=0, crypto* c=nullptr);
    void set_bit(int place, int new_val);
    int get_bit(int place) const;
    int get_length() const;
    BYTE* getArr();
    ~Bitstring();
	
    friend std::ostream& operator<<(std::ostream& os,const Bitstring& b);
	
};

# endif
