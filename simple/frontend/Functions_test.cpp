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
	
	vector<int>* arr_indexes=new vector<int>[2];
	
	int indexes_0[]={10,6 ,9 ,3,17,19,12,4 ,13,7 ,21,0,22};
	int indexes_1[]={8 ,15,11,1,24,14,5 ,16,23,18,2,20};
	
	int indexes_0_[]={0,25,4,6 ,7 ,9 ,10,12,13,17,19,27,26};
	int indexes_1_[]={1,2 ,5,28,11,29,15,16,18,20,23,24};
	
	std::set<int> set_0(indexes_0_,indexes_0_+13);
	std::set<int> set_1(indexes_1_,indexes_1_+12);
	
	arr_indexes[0].insert(arr_indexes[0].end(),indexes_0,indexes_0+13);
	arr_indexes[1].insert(arr_indexes[1].end(),indexes_1,indexes_1+12);
	
	functions::arrange_the_indexes(BF,Nbf,arr_indexes, &indexes);
	
	set <int> zeros;
	set <int> ones;
	
	for (int i=0;i<Nbf;i++){
		if ((BF.get_bit(i)==0) && (set_0.find(indexes[i])!=set_0.end())) zeros.insert(indexes[i]);
		else if((BF.get_bit(i)==1) && (set_1.find(indexes[i])!=set_1.end())) ones.insert(indexes[i]);
	}
	EXPECT_EQ(zeros.size(),11);
	EXPECT_EQ(ones.size(),9);
}

TEST(compute_r, test1){

	std::vector <std::array<block, 2>> strings{{toBlock(8667565670,34632836472),toBlock(73673436754,3264736)},
	{toBlock(454364563453454,73647235645),toBlock(87237478263,53241423123)},
	{toBlock(87384636274,43434378248),toBlock(4798676554453,878767565454332)},
	{toBlock(54432287767,95415689),toBlock(7653211887654322,94321679325780)}, 
	{toBlock(78665433355,11243465767),toBlock(98766545453435,76867675657656)},
	{toBlock(653211678898,7321456778978),toBlock(38765321988432,86554453453434)},
	{toBlock(422189878786,96645654533),toBlock(2998778656006,7675656432323)},
	{toBlock(875564534226,1557567564567),toBlock(555463456688,775769988976)},
	{toBlock(453465653543,2443535634534),toBlock(3654353463453,2432453465346)}};
	int indexes[]={0,3,4,8};
	int indexes_size=4;

	block r_res;	
	functions::compute_r(r_res,strings,indexes,indexes_size);
	
	block r_exp=toBlock(504334762589,2415479993520); //toBlock(8667565670,34632836472)^toBlock(54432287767,95415689)^toBlock(78665433355,11243465767)^toBlock(453465653543,2443535634534)
	
	EXPECT_EQ(r_res,r_exp);
}

TEST(compute_R_r, test1){
	
	int sub_group[]={8,4,22,7,1};
	int Nc=5;
	int Not=30;
	BitVector b_res("011001001001001110101001101010");
	
                    //{0,1,1,0,0,1,0,0,1,0 ,0,1 ,0 ,0 ,1 ,1 ,1 ,0 ,1 ,0 ,1 ,0 ,0 ,1 ,1 ,0 ,1 ,0 ,1 ,0 }
	int indexes_res[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29};

	BitVector b_exp("001011001001001110101001110010");	
	                //{0,0 ,1,0,1 ,1,0,0 ,1 ,0,0 ,1 ,0 ,0 ,1 ,1 ,1 ,0 ,1 ,0 ,1 ,0 ,0 ,1 ,1 ,1,0,0,1,0 }
	int indexes_exp[]={0,25,2,3,26,5,6,27,28,9,10,11,12,13,14,15,16,17,18,19,20,21,29,23,24,1,4,7,8,22};
	
	std::vector<block> strings{
	    toBlock(8667565670,34632836472),
	    toBlock(73673436754,3264736), //1
	    toBlock(454364563453454,73647235645),
	    toBlock(87237478263,53241423123),
	    toBlock(87384636274,43434378248), //4 - 0 bit
	    toBlock(4798676554453,878767565454332),
	    toBlock(54432287767,95415689),
	    toBlock(7653211887654322,94321679325780), //7- 0 bit
	    toBlock(78665433355,11243465767), //8
	    toBlock(98766545453435,76867675657656),
	    toBlock(653211678898,7321456778978),
	    toBlock(38765321988432,86554453453434),
	    toBlock(2998778656006,7675656432323),
	    toBlock(422189878786,96645654533),
	    toBlock(875564534226,1557567564567),
	    toBlock(555463456688,775769988976),
	    toBlock(453465653543,2443535634534),
	    toBlock(324545436656,767567567456),
	    toBlock(1074379875453,4534676765857),
	    toBlock(5678688976756,645645775675),	 
	    toBlock(6578790785674,123243647657),
	    toBlock(134346786786,45647867856),
	    toBlock(453678696789,675687687689678), //22 -0 bit
	    toBlock(422189878786,985674523457),
	    toBlock(546566578678,5647658680083),
	    toBlock(645645768786,546457678779),
	    toBlock(453465653543,54365654887),
	    toBlock(324545436656,1343565676),
	    toBlock(456565464564,322343454667),
	    toBlock(453465635635,56456456456654)
	};

	block r_exp=toBlock(7653647930910613,619343583792018);//toBlock(87384636274,43434378248)^toBlock(7653211887654322,94321679325780)^toBlock(453678696789,675687687689678)
	std::vector<int> R_exp{22,7,4};
	
	block r_res;
	std::vector<int> R_res;
	
        functions::compute_R_r(  sub_group,indexes_res,&b_res,Nc,Not, strings, r_res, R_res);	

	EXPECT_EQ(r_res,r_exp);	
	EXPECT_EQ(R_res,R_exp);
	
        for (int i=0;i<Not;i++)
	{
	     EXPECT_EQ(indexes_res[i],indexes_exp[i]);
	     EXPECT_EQ(b_res[i],b_exp[i]);
	}

}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}
