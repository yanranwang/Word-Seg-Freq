/***************************************************************************
 * 
 * Copyright (c) 2014. All Rights Reserved
 * 
 **************************************************************************/
#include <string>
#include <vector>
#include <stdio.h>
#include <utility>
#include <uln_sign_f64.h>
#include <gtest/gtest.h>
#include "merge_storage.h"
#include "common_data.h"
#include "wc_log.h"

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class test_MergeStorage_suite : public ::testing::Test{
protected:
    wyr::MergeStorage* p_ms;
    test_MergeStorage_suite(){};
    virtual ~test_MergeStorage_suite(){};
    virtual void SetUp() {
        p_ms = new wyr::MergeStorage();
    };
    virtual void TearDown() {
    };
};
/**
 * @brief 
 * @begin_version 
 **/
TEST_F(test_MergeStorage_suite, test_merge_hashmap)
{
    ASSERT_EQ(0, p_ms->init()); 
    wyr::wc_result_map_t map1;
    wyr::wc_result_map_t map2;
    wyr::wc_result_map_t map3;
    wyr::wc_result_map_t map_final;
    std::string str1("名字");
    std::string str2("贝儿");
    std::string str3("的");

    unsigned long long sign_s;
    ASSERT_NE(-1, uln_sign_f64(str1.c_str(), str1.size(), &sign_s));
    map1.set((uint64_t)sign_s, make_pair(2, str1));    
    map2.set((uint64_t)sign_s, make_pair(1, str1));
    map_final.set((uint64_t)sign_s, make_pair(3, str1));

    ASSERT_NE(-1, uln_sign_f64(str2.c_str(), str2.size(), &sign_s));
    map1.set((uint64_t)sign_s, make_pair(1, str2));    
    map2.set((uint64_t)sign_s, make_pair(3, str2));
    map3.set((uint64_t)sign_s, make_pair(1, str2));
    map_final.set((uint64_t)sign_s, make_pair(5, str2));
    
    ASSERT_NE(-1, uln_sign_f64(str3.c_str(), str3.size(), &sign_s));
    map1.set((uint64_t)sign_s, make_pair(1, str3));    
    map3.set((uint64_t)sign_s, make_pair(1, str3));
    map_final.set((uint64_t)sign_s, make_pair(2, str3));

    std::vector<wyr::wc_result_map_t> results;
    results.push_back(map1);
    results.push_back(map2);
    results.push_back(map3);

    EXPECT_EQ(0, p_ms->_merge_hashmap(results)); 
    
    EXPECT_EQ(p_ms->_result_final_map.size(), map_final.size());
    wyr::wc_result_map_pair_t* ptr;
    wyr::wc_result_map_t::const_iterator iter = map_final.begin();
    for (; iter != map_final.end(); ++iter) {
        ptr = p_ms->_result_final_map.find(iter->first);
        EXPECT_TRUE(NULL != ptr);
        EXPECT_EQ(iter->second.first, (ptr->second).first);
        EXPECT_EQ(iter->second.second, (ptr->second).second);
    }
}

TEST_F(test_MergeStorage_suite, test_merge_vector)
{
    ASSERT_EQ(0, p_ms->init()); 
    wyr::wc_basic_vec_t vec1;
    wyr::wc_basic_vec_t vec2;
    wyr::wc_basic_vec_t vec3;
    wyr::wc_basic_vec_t vec_final;
    std::string str1("名字");
    std::string str2("贝儿");
    std::string str3("的");
    vec1.push_back(str1);
    vec1.push_back(str1);
    vec2.push_back(str2);
    vec2.push_back(str3);
    vec3.push_back(str1);
    vec_final.push_back(str1);
    vec_final.push_back(str1);
    vec_final.push_back(str2);
    vec_final.push_back(str3);
    vec_final.push_back(str1);
    std::vector<wyr::wc_basic_vec_t> results;
    results.push_back(vec1);
    results.push_back(vec2);
    results.push_back(vec3);

    EXPECT_EQ(0, p_ms->_merge_vector(results));
    EXPECT_EQ(vec_final, p_ms->_basic_final_vec);
}
