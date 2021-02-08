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


int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}
