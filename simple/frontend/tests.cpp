#include <gtest/gtest.h>
#include "../src/Functions.h"


TEST (IfDisjointSets, test1)
{
	int group1[]={6,8,2,11,24,15,38,31,12,14};
	int group2[]={5,10,16,18,19,3,21,22,17,33,1,28,0,39,10,22,9,35,30,27};
        int result=functions::if_disjoint_sets(group1,10,group2,20,40);
	EXPECT_EQ(1,result);
}

TEST (IfDisjointSets, test2)
{
	int group1[]={6,8,9,11,24,15,38,31,12,19};
	int group2[]={5,10,16,18,19,3,21,22,17,33,1,28,0,39,10,22,9,35,30,27};
        int result=functions::if_disjoint_sets(group1,10,group2,20,40);
	EXPECT_EQ(0,result);
}

TEST (IfDisjointSets, test3)
{
	int group1[]={6,8,9,11,24,15,38,31,0,19};
	int group2[]={5,10,16,18,19,3,21,22,17,33,1,28,0,39,10,22,9,35,30,27};
        int result=functions::if_disjoint_sets(group1,10,group2,20,40);
	EXPECT_EQ(0,result);
}

TEST (IfDisjointSets, test4)
{
	int group1[]={6,8,9,11,24,15,39,31,0,19};
	int group2[]={5,10,16,18,19,3,21,22,17,33,1,28,0,39,10,22,9,35,30,27};
    int result=functions::if_disjoint_sets(group1,10,group2,20,40);
	EXPECT_EQ(0,result);
}

TEST(get_zeros_ones, test1)
{
	BitVector* b=new BitVector("011001001001001110101001101010");
	int Not=30;
	int Nc=5;
	
	int arr0[]={0,3,4,6,7,9,10,12,13,17,19,21,22};
	int arr1[]={1,2,5,8,11,14,15,16,18,20,23,24};
	
	set <int> v0_expected(arr0,arr0+13),v1_expected(arr1,arr1+12);
	
	vector<int>* arr_indexes;
	crypto crypt(128,(uint8_t*)"837398417082347831734964176");
	
	functions::get_zeros_ones(b,Not,Nc,arr_indexes,&crypt);
	
	set <int> v0,v1;
	
	for (auto i:arr_indexes[0]) v0.insert(i);
	for (auto i:arr_indexes[1]) v1.insert(i);
	
	delete [] arr_indexes;
	
	EXPECT_EQ(v0,v0_expected);
	EXPECT_EQ(v1,v1_expected);	
	
	
}


TEST(checkTheItem, test1)
{
	block GBF[]={toBlock(1436786867,57576545645),toBlock(78437437,7954547653748),toBlock(4566546456,5676756783234),toBlock(2943874329645,19984376236),toBlock(763473628564,4552638218),
	toBlock(63243902347,878435326),toBlock(4345453566,5884954757),toBlock(4348954777321,23243849830),toBlock(4839483726662,88783623632),toBlock(983847362553,9343262553)};
        int indexes[]={3,6,7};
	std::set<int> h_kokhav(indexes,indexes+3);
        block* result=functions::check_the_item(h_kokhav,GBF);
	EXPECT_EQ(toBlock(1481737543994,2426218831),*result);
	delete result;
}


TEST(arrangeTheIndexes, test1)
{
	
	             //{0,1,1,0 ,0,1,0,0,1 ,0,0 ,1 ,0 ,0 ,1 ,1 ,1 ,0 ,1 ,0 ,1 ,0 ,0 ,1 ,1 ,0,1 ,0 ,1,0 }  //bitstring
	int indexes_[]={0,1,2,25,4,5,6,7,28,9,10,11,12,13,29,15,16,17,18,19,20,27,26,23,24,3,22,21,8,14}; //Nc=5,Not=30,Not-Nc=25
	
	int* indexes=new int[30];
	for (int i=0;i<30;i++) indexes[i]=indexes_[i];
	
	BYTE arr[]={0,1,1,0,1 ,1,0,1,1,0, 0,0,1,0,0, 1,0,0,1,0};
	Bitstring BF(20,arr);
    int Nbf=20;
	
	crypto* crypt;
	vector<int>* arr_indexes=new vector<int>[2];
	
	int indexes_0[]={10,6 ,9 ,3,17,19,12,4 ,13,7 ,21,0,22};
	int indexes_1[]={8 ,15,11,1,24,14,5 ,16,23,18,2,20};
	
	int indexes_0_[]={0,25,4,6 ,7 ,9 ,10,12,13,17,19,27,26};
	int indexes_1_[]={1,2 ,5,28,11,29,15,16,18,20,23,24};
	
	std::set<int> set_0(indexes_0_,indexes_0_+13);
	std::set<int> set_1(indexes_1_,indexes_1_+12);
	
	arr_indexes[0].insert(arr_indexes[0].end(),indexes_0,indexes_0+13);
	arr_indexes[1].insert(arr_indexes[1].end(),indexes_1,indexes_1+12);
	
	functions::arrange_the_indexes(BF,Nbf,arr_indexes, &indexes,0,0,crypt);
	
	set <int> zeros;
	set <int> ones;
	
	for (int i=0;i<Nbf;i++){
		cout<<indexes[i]<<" ";
		if ((BF.get_bit(i)==0) && (set_0.find(indexes[i])!=set_0.end())) zeros.insert(indexes[i]);
		else if((BF.get_bit(i)==1) && (set_1.find(indexes[i])!=set_1.end())) ones.insert(indexes[i]);
	}
	EXPECT_EQ(zeros.size(),11);
	EXPECT_EQ(ones.size(),9);
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}
