/***************************************************************************
 * 
 * Copyright (c) 2014. All Rights Reserved
 * 
 **************************************************************************/
#include <string>
#include <vector>
#include <stdio.h>
#include <utility>
#include <gtest/gtest.h>
#include <com_log.h>
#include <Configure.h>
#include "common_data.h"
#include "blocking_queue.h"    
#include "consumer.h"    

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    int ret = com_loadlog("./conf/", "log.conf");
    if (0 != ret) {
        fprintf(stderr, "comlog load error\n");
    }
    return RUN_ALL_TESTS();
}

class test_Consumer_suite : public ::testing::Test{
protected:
    wyr::Consumer* p_consumer;
    const scw_worddict_t* pwdict;
    wyr::wc_result_map_t hp;
    wyr::wc_basic_vec_t basic_vec;  
    test_Consumer_suite(){};
    virtual ~test_Consumer_suite(){};
    virtual void SetUp() {
        p_consumer = new wyr::Consumer();
        ASSERT_EQ(0, hp.create(wyr::HASHMAP_BITEMS));
        comcfg::Configure conf;
        ASSERT_EQ(0, conf.load("./conf/", "wordcount.conf"));
        pgconf = scw_load_conf(conf["PARAMS"]["SEGWORD_CONF_FILE"].to_cstr());
        ASSERT_TRUE(NULL != pgconf);
        pwdict = scw_load_worddict(conf["PARAMS"]["SEGWORD_DICT_PATH"].to_cstr());
        ASSERT_TRUE(NULL !=  pwdict);
    };
    virtual void TearDown() {
        if (NULL != p_consumer) {
            delete p_consumer;
        }
    };
};
/**
 * @brief 
 * @begin_version 
 **/
TEST_F(test_Consumer_suite, test_get_line)
{   
    wyr::BlockingQueue wc_queue;
    ASSERT_EQ(0, wc_queue.init());
    ASSERT_EQ(0, p_consumer->init(pwdict, &hp, &basic_vec, &wc_queue));
 
    std::vector<std::string> myvector;
    std::string str1("ÎÒµÄÃû×Ö½Ð±´¶ù");
    std::string str2("       ");
    std::string str3 = wyr::PRODUCER_END_LABEL;
    myvector.push_back(str3);
    myvector.push_back(str2);
    myvector.push_back(str1);
    (p_consumer->_p_wc_queue)->put(&myvector);
    EXPECT_EQ(0, p_consumer->_get_line(&(p_consumer->_sentence))); 
    EXPECT_EQ(0, str1.compare(p_consumer->sentence()));
    EXPECT_EQ(0, p_consumer->_get_line(&(p_consumer->_sentence))); 
    EXPECT_EQ(0, str2.compare(p_consumer->sentence()));
    EXPECT_EQ(1, p_consumer->_get_line(&(p_consumer->_sentence))); 
    EXPECT_EQ(0, str3.compare(p_consumer->sentence()));
}

// regular query
TEST_F(test_Consumer_suite, test_split_string__true)
{
    wyr::BlockingQueue wc_queue;
    ASSERT_EQ(0, wc_queue.init());
    ASSERT_EQ(0, p_consumer->init(pwdict, &hp, &basic_vec, &wc_queue));
 
    std::string str1("ÎÒµÄ\tÃû×Ö\t½Ð\t±´¶ù\t");
    const char* sep = "\t";
    char query[wyr::LINE_MAX_LENGTH];
    EXPECT_EQ(0, p_consumer->split_string_get_column(str1, 4, 
                            sep, query, wyr::LINE_MAX_LENGTH));
    EXPECT_EQ(0, strcmp(query, "±´¶ù"));
}

// the field requested doesn't exist & the seperator doesn't exist & the seperator is NULL
TEST_F(test_Consumer_suite, test_split_string__false)
{
    wyr::BlockingQueue wc_queue;
    ASSERT_EQ(0, wc_queue.init());
    ASSERT_EQ(0, p_consumer->init(pwdict, &hp, &basic_vec, &wc_queue));
 
    std::string str1("ÎÒµÄ\tÃû×Ö\t½Ð\t±´¶ù\t");
    std::string sep("\t");
    std::string sep_false(" ");
    char query[wyr::LINE_MAX_LENGTH];
    EXPECT_EQ(-1, p_consumer->split_string_get_column(str1, 5, 
                            sep, query, wyr::LINE_MAX_LENGTH));
    EXPECT_EQ(-1, p_consumer->split_string_get_column(str1, 1, 
                            sep_false, query, wyr::LINE_MAX_LENGTH));
    sep.clear();
    EXPECT_EQ(-1, p_consumer->split_string_get_column(str1, 1, 
                            sep, query, wyr::LINE_MAX_LENGTH));
}

TEST_F(test_Consumer_suite, test_seg_word)
{
    wyr::BlockingQueue wc_queue;
    ASSERT_EQ(0, wc_queue.init());
    ASSERT_EQ(0, p_consumer->init(pwdict, &hp, &basic_vec, &wc_queue));
 
    char query_true[] = "ÎÒÏ²»¶±´¶ù";
    char query_false[] = "    \n";
    EXPECT_EQ(0, p_consumer->_seg_word(query_true));
    EXPECT_EQ(1, p_consumer->_seg_word(query_false));
    EXPECT_EQ(-1, p_consumer->_seg_word(NULL));
}

TEST_F(test_Consumer_suite, test_segword_result_into_storage)
{
    wyr::BlockingQueue wc_queue;
    ASSERT_EQ(0, wc_queue.init());
    ASSERT_EQ(0, p_consumer->init(pwdict, &hp, &basic_vec, &wc_queue));
 
    char query_true1[] = "ÎÒµÄÃû×Ö½Ð±´¶ù";
    char query_true2[] = "FeynmanÊÇ±´¶ùºÃÅóÓÑ";
    char query_true3[] = "½ã½ãºÃ¿É°®";
    char query_true4[] = "JoeÊÇ½ã½ãµÄ¸ç¸ç";
    char query_false[] = "      ";
    EXPECT_EQ(0, p_consumer->_seg_word(query_true1));
    EXPECT_EQ(0, p_consumer->_segword_result_into_storage());
    EXPECT_EQ(0, p_consumer->_seg_word(query_true2));
    EXPECT_EQ(0, p_consumer->_segword_result_into_storage());
    EXPECT_EQ(0, p_consumer->_seg_word(query_true3));
    EXPECT_EQ(0, p_consumer->_segword_result_into_storage());
    EXPECT_EQ(0, p_consumer->_seg_word(query_true4));
    EXPECT_EQ(0, p_consumer->_segword_result_into_storage());
    EXPECT_EQ(1, p_consumer->_seg_word(query_false));
    EXPECT_EQ(1, p_consumer->_segword_result_into_storage());

    // true result
    std::vector<std::pair<std::string, uint64_t> > result_truth;
    std::string mystring("¸ç¸ç");
    result_truth.push_back(make_pair(mystring, 1));
    mystring.assign("ºÃ¿É°®");
    result_truth.push_back(make_pair(mystring, 1));
    mystring.assign("ºÃÅóÓÑ");
    result_truth.push_back(make_pair(mystring, 1));
    mystring.assign("Joe");
    result_truth.push_back(make_pair(mystring, 1));
    mystring.assign("½Ð");
    result_truth.push_back(make_pair(mystring, 1));
    mystring.assign("Feynman");
    result_truth.push_back(make_pair(mystring, 1));
    mystring.assign("ÎÒµÄÃû×Ö");
    result_truth.push_back(make_pair(mystring, 1));
    mystring.assign("µÄ");
    result_truth.push_back(make_pair(mystring, 1));
    mystring.assign("½ã½ã");
    result_truth.push_back(make_pair(mystring, 2));
    mystring.assign("ÊÇ");
    result_truth.push_back(make_pair(mystring, 2));
    mystring.assign("±´¶ù");
    result_truth.push_back(make_pair(mystring, 2));
    
    // compare with groundtruth 
    wyr::wc_result_map_pair_t* ptr;
    EXPECT_EQ(result_truth.size(), hp.size());
    for (uint32_t i = 0; i != result_truth.size(); ++i) {
        unsigned long long  sign_s_tmp;
        std::string oneword(result_truth[i].first);
        int ret = uln_sign_f64(oneword.c_str(), oneword.size(), &sign_s_tmp);
        ASSERT_NE(-1, ret);
        uint64_t sign_s = sign_s_tmp;
        ptr = hp.find(sign_s);
        EXPECT_TRUE(NULL != ptr);
        EXPECT_EQ(result_truth[i].second, (ptr->second).first);
    }
}

TEST_F(test_Consumer_suite, test_run_consumer)
{
    wyr::BlockingQueue wc_queue;
    ASSERT_EQ(0, wc_queue.init());
    ASSERT_EQ(0, p_consumer->init(pwdict, &hp, &basic_vec, &wc_queue));
 
    std::vector<std::string> myvector;
    std::string mystring("\t\t\t\t\t\t\t\t\t\t\t\t\t\tÎÒµÄÃû×Ö½Ð±´¶ù\t\t");
    myvector.push_back(mystring);
    mystring.assign("\t\t\t\t\t\t\t\t\t\t\t\t\t\t±´¶ùºÃ±¦±´\t\t");
    myvector.push_back(mystring);
    wc_queue.put(&myvector);
    myvector.clear();
    mystring.assign(wyr::PRODUCER_END_LABEL);
    myvector.push_back(mystring);
    wc_queue.put(&myvector);
    
    EXPECT_EQ(0, p_consumer->run_consumer()); 
   
    // true result 
    std::vector<std::pair<std::string, uint64_t> > result_truth;
    mystring.assign("±¦±´");
    result_truth.push_back(make_pair(mystring, 1));
    mystring.assign("½Ð");
    result_truth.push_back(make_pair(mystring, 1));
    mystring.assign("ÎÒµÄÃû×Ö");
    result_truth.push_back(make_pair(mystring, 1));
    mystring.assign("ºÃ");
    result_truth.push_back(make_pair(mystring, 1));
    mystring.assign("±´¶ù");
    result_truth.push_back(make_pair(mystring, 2));

    // compare with groundtruth 
    wyr::wc_result_map_pair_t* ptr;
    EXPECT_EQ(result_truth.size(), hp.size());
    for (uint32_t i = 0; i != result_truth.size(); ++i) {
        unsigned long long  sign_s_tmp;
        std::string oneword(result_truth[i].first);
        int ret = uln_sign_f64(oneword.c_str(), oneword.size(), &sign_s_tmp);
        ASSERT_NE(-1, ret);
        uint64_t sign_s = sign_s_tmp;
        ptr = hp.find(sign_s);
        EXPECT_TRUE(NULL != ptr);
        EXPECT_EQ(result_truth[i].second, (ptr->second).first);
    }
}
