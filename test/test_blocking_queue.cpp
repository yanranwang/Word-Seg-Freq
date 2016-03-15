/***************************************************************************
 * 
 * Copyright (c) 2014. All Rights Reserved
 * 
 **************************************************************************/
 
#include <blocking_queue.h>
#include <gtest/gtest.h>
#include "common_data.h"

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
 
/**
 * @brief 
 **/
class test_BlockingQueue_suite : public ::testing::Test{
protected:
    wyr::BlockingQueue* p_queue;
    test_BlockingQueue_suite(){};
    virtual ~test_BlockingQueue_suite(){};
    virtual void SetUp() {
        //Called befor every TEST_F(test_BlockingQueue_suite, *)
        p_queue = new wyr::BlockingQueue();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_BlockingQueue_suite, *)
        if (NULL != p_queue) {
            delete p_queue;
        }
    };
};
 
/**
 * @brief 
 * @begin_version 
 **/
TEST_F(test_BlockingQueue_suite, test_put)
{
    EXPECT_EQ(0, p_queue->init());
    EXPECT_EQ(0, (p_queue->_queue).size());
    std::vector<std::string> myvector;
    std::string mystring("Hello World");
    myvector.push_back(mystring);
    p_queue->put(&myvector);
    myvector.clear();
    mystring.assign("Yanran");
    myvector.push_back(mystring);
    p_queue->put(&myvector);
    EXPECT_EQ(2, (p_queue->_queue).size());
    //TODO
}

TEST_F(test_BlockingQueue_suite, test_pull)
{
    EXPECT_EQ(0, p_queue->init());
    std::vector<std::string> myvector1;
    std::string mystring("Hello World");
    myvector1.push_back(mystring);
    std::vector<std::string> myvector2(myvector1);
    p_queue->put(&myvector1);
    myvector1.clear();

    myvector1 = p_queue->pull();
    EXPECT_EQ(1, myvector1 == myvector2);
    EXPECT_EQ(0, (p_queue->_queue).size());
    //TODO
}
/**
 * @file /home/wangyanran/baidu_test/app/ecom/cpro/tools/wangyanran/wordcount/.///test/test_BlockingQueue.cpp
 * @author wangyanran100@gmail.com
 * @date 2014/07/16 20:56:37
 * @brief 
 *  
 **/
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
