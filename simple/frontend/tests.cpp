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
    block* result=functions::check_the_item(h_kokhav,0,0,GBF);
	EXPECT_EQ(toBlock(1481737543994,2426218831),*result);
	delete result;
}


/*
TEST (ArrangeTheIndexes, test1)
{
    BYTE bf[]={122,162,1};//{0,1,0,1,1,1,1,0   0,1,0,0,0,1,0,1  1,0,0,0};
	BYTE b[]= {91 ,77, 174 ,49,214};      //{1,1,0,1,1,0,1,0   1,0,1,1,0,0,1,0  ,0,1,1,1,0,1,0,1   1,0,0,0,1,1,0,0   0,1,1,0,1,0,1,1};	
	int indexes []={0,39,2,3,4,37,6,7,8,32,10,11,12,13,35,15,16,17,18,19,36,21,22,23,38,34,33,27,28,29,30,31,9,26,25,14,20,5,24,1};
	int expected []={37,39,2,3,4,0,6,7,32,8,13,15,12,10,16,11,35,36,22,34};
	functions::arrange_the_indexes(bf,20,b,indexes,40);
	for (int i=0;i<20;i++){
		EXPECT_EQ(indexes[i],expected[i]);
	}
}

TEST (ArrangeTheIndexes, test2)
{
	BYTE bf[]={122,162,1};//{0,1,0,1,1,1,1,0   0,1,0,0,0,1,0,1   1,0,0,0};
	BYTE b[]= {72, 76, 162 , 49 ,214};//{0,0,0,1,0,0,1,0  0,0,1,1,0,0,1,0  0,1,0,0,0,1,0,1   1,0,0,0,1,1,0,0   0,1,1,0,1,0,1,1};	
	int indexes []={0,39,2,3,4,37,6,7,8,32,10,11,12,13,35,15,16,17,18,19,36,21,22,23,38,34,33,27,28,29,30,31,9,26,25,14,20,5,24,1};
	int expected []={0,10,2,3,11,35,6,7,8,17,39,4,12,21,37,23,38,32,18,19};
	functions::arrange_the_indexes(bf,20,b,indexes,40);
	for (int i=0;i<20;i++){
		EXPECT_EQ(indexes[i],expected[i]);
	}
}


TEST (find_intersection, test1)
{
	BYTE key1[]={123,8,79,44,55,8}; BYTE key2[]={42,88,44,132,244,221}; BYTE key3[]={89,97,55,41,11,99}; BYTE key4[]={132,121,78,251,242,76}; BYTE key5[]={99,97,67,251,0,32};
	BYTE key6[]={17,65,99,23,123,77};  BYTE key7[]={88,77,65,1,13,42,56}; BYTE key8[]={154,34,111,24,243,21};  BYTE key9[]={67,72,34,99,121,18}; BYTE key10[]={35,89,216,54,11,90};
	vector<BYTE*> keys1={key1,key2,key3,key4,key5,key6,key7};
	const char* items[]={"Avi","Lior","Maor","Ron","Dolev","May","Dana"};
	vector<BYTE*> keys2={key8,key7,key9,key10,key2};
	vector<const char*> results=functions::find_intersection(7,keys1,5,keys2, items,6);
	const char* expected[]={"Lior","Dana"};
	int num=2;
	EXPECT_EQ(results.size(),num);
	if (results.size()==num)
	    for (int i=0;i<num;i++)
		EXPECT_EQ(expected[i],results[i]);
}

TEST (compute_r, test1)
{
	BYTE string1[]={128,45,66,54,32,124};BYTE string2[]={33,44,234,25,123,38};BYTE string3[]={145,32,148,77,45,33};BYTE string4[]={214,125,22,11,9,18};BYTE string5[]={8,110,23,253,34,12};
	BYTE string6[]={55,66,77,123,221,213};BYTE string7[]={45,55,23,21,33,125};BYTE string8[]={34,123,28,74,58,122};BYTE string9[]={222,12,32,45,121,34};
	BYTE* strings[]={string1,string2,string3,string4,string5,string6,string7,string8,string9};
        int indexes[]={0,2,4,7};
	BYTE expected[]={59,24,221,204,21,43};
	BYTE* result=functions::compute_r(strings,6,indexes,4);
        for (int i=0;i<6;i++)
           EXPECT_EQ(expected[i],result[i]);
}

TEST (compute_R_r, test1)
{
          int sub_group[]={0,3,32,4,10,23,21,38,18,24};
          BYTE b[]= {72, 76, 162 , 49 ,214};//{0,0,0,1,0,0,1,0   0,0, 1, 1, 0, 0, 1, 0    0 ,1 ,0 ,0 ,0 ,1 ,0 ,1    1 ,0 ,0 ,0 ,1 ,1 ,0 ,0    0, 1 ,1, 0 ,1 ,0, 1, 1};	
	  int indexes[]=                      {0,1,2,3,4,5,6,7,  8,9,10,11,12,13,14,15,  16,17,18,19,20,21,22,23,  24,25,26,27,28,29,30,31,  32,33,34,35,36,37,38,39};
	  BYTE string1[]={128,45,66,54,32,124};BYTE string2[]={33,44,234,25,123,38};BYTE string3[]={145,32,148,77,45,33};BYTE string4[]={214,125,22,11,9,18};BYTE string5[]={8,110,23,253,34,12};
	  BYTE string6[]={12,35,64,145,36,14};BYTE string7[]={31,144,24,5,1,128};BYTE string8[]={125,42,18,71,21,0};BYTE string9[]={21,167,34,21,5,86};BYTE string10[]={7,134,13,67,44,33};
          BYTE string11[]={13,35,26,55,33,24};BYTE string12[]={26,91,5,24,213,174};BYTE string13[]={17,33,149,79,43,21};BYTE string14[]={11,34,20,1,80,200};BYTE string15[]={88,221,37,66,54,11};
	  BYTE string16[]={38,42,33,51,85,87};BYTE string17[]={56,49,116,164,17,3};BYTE string18[]={124,214,166,67,55,45};BYTE string19[]={2,197,234,119,21,143};BYTE string20[]={93,1,12,24,11,137};
	  BYTE string21[]={11,24,24,59,148,77};BYTE string22[]={146,145,123,192,7,57};BYTE string23[]={119,3,11,254,124,86};BYTE string24[]={27,217,127,100,207,190};BYTE string25[]={84,11,89,44,69,1};
	  BYTE string26[]={131,11,91,189,36,67};BYTE string27[]={213,13,83,41,34,35};BYTE string28[]={102,167,231,43,87,208};BYTE string29[]={34,84,98,4,76,180};BYTE string30[]={14,170,10,25,45,44};
	  BYTE string31[]={145,31,42,99,31,51};BYTE string32[]={189,63,231,11,4,69};BYTE string33[]={113,57,139,177,47,35};BYTE string34[]={13,31,42,14,90,118};BYTE string35[]={15,39,75,241,3,36};
	  BYTE string36[]={172,32,64,41,108,61};BYTE string37[]={42,57,2,7,11,66};BYTE string38[]={167,41,14,223,49,8};BYTE string39[]={89,43,220,10,91,87};BYTE string40[]={23,48,1,222,11,70};
	  BYTE* strings[]={string1,string2,string3,string4,string5,string6,string7,string8,string9,string10,string11,string12,string13,string14,string15,string16,string17,string18,string19,string20,
	  string21,string22,string23,string24,string25,string26,string27,string28,string29,string30,string31,string32,string33,string34,string35,string36,string37,string38,string39,string40};
	  int expected_R[]={0,32,4,18};
	  BYTE expected_r[]={251,191,52,13,56,220};
	  BYTE result_r[6];
	  std::vector <int> result_R;
          functions::compute_R_r(sub_group,indexes,b,10,40,strings,result_r,6,result_R);	
	  EXPECT_EQ(result_R.size(),4);
	  if (result_R.size()==4)
	     for (int i=0;i<result_R.size();i++)
                 EXPECT_EQ(expected_R[i],result_R[i]);
	  for (int i=0;i<6;i++)
              EXPECT_EQ(expected_r[i],result_r[i]);

}	
*/
int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}