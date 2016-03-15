/***************************************************************************
 * 
 * Copyright (c) 2014. All Rights Reserved
 * 
 **************************************************************************/
#include <iostream>
#include <gtest/gtest.h>
#include <com_log.h>
#include "word_count_conf.h"
#include "segword.h"
    
int main(int argc, char **argv)
{   
    testing::InitGoogleTest(&argc, argv);
    int ret = com_loadlog("./conf/", "log.conf");
    if (0 != ret) {
        fprintf(stderr, "comlog load main configure error");
    }
    return RUN_ALL_TESTS();
}
 
/**
 * @brief 
 **/
class test_Segword_suite : public ::testing::Test{
protected:
    wyr::Segword* p_segword;
    scw_worddict_t* pwdict; 
    test_Segword_suite(){};
    virtual ~test_Segword_suite(){};
    virtual void SetUp() {
        comcfg::Configure conf;
        ASSERT_EQ(0, conf.load("./conf/", "wordcount.conf"));
        pgconf = scw_load_conf(conf["PARAMS"]["SEGWORD_CONF_FILE"].to_cstr());
        ASSERT_TRUE(NULL != pgconf);
        pwdict = scw_load_worddict(conf["PARAMS"]["SEGWORD_DICT_PATH"].to_cstr());
        ASSERT_TRUE(NULL != pwdict);
        p_segword = new wyr::Segword();
    };
    virtual void TearDown() {
        if (NULL != p_segword) {
            p_segword->end_segword();
            //destory segword dic & conf
            if (NULL != pwdict) {
                scw_destroy_worddict(pwdict);
                pwdict = NULL;
                pgconf = NULL;  
            }
            if (NULL != pgconf) {
                scw_destroy_conf(pgconf);
                pgconf = NULL;
            }
            delete p_segword;
        }
    };
};
/**
 * @brief 
 * @begin_version 
 **/
TEST_F(test_Segword_suite, test_initialize_segword__true)
{
    EXPECT_EQ(0, p_segword->init(pwdict));
}

TEST_F(test_Segword_suite, test_initialize_segword__false)
{
    EXPECT_EQ(-1, p_segword->init(NULL));
}

//regular words
TEST_F(test_Segword_suite, test_run_segword__true)
{
    ASSERT_EQ(0, p_segword->init(pwdict));
    char line[] = "我的名字叫贝儿 \n";
    EXPECT_EQ(0, p_segword->run_segword(line));
    int num = p_segword->segword_word_num();
    EXPECT_EQ(3, num);
    const token_t* results = p_segword->segword_word_result();
    EXPECT_EQ(0, strcmp(results[0].buffer, "我的名字"));
    EXPECT_EQ(0, strcmp(results[1].buffer, "叫"));
    EXPECT_EQ(0, strcmp(results[2].buffer, "贝儿"));
}

//the word is NULL or is consist of space
TEST_F(test_Segword_suite, test_run_segword__false)
{
    //when query is NULL
    ASSERT_EQ(0, p_segword->init(pwdict));
    char* line = NULL;
    EXPECT_EQ(-1, p_segword->run_segword(line));
    //when query is consist of space
    char empty_line[] = "      \n\r\n";
    EXPECT_EQ(1, p_segword->run_segword(empty_line));
}
/**
 * @file /home/wangyanran/baidu_test/app/ecom/cpro/tools/wangyanran/wordcount/.///test/test_Segword.cpp
 * @author wangyanran100@gmail.com
 * @date 2014/07/16 20:56:39
 * @brief 
 *  
 **/
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
