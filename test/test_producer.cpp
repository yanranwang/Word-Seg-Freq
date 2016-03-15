/***************************************************************************
 * 
 * Copyright (c) 2014. All Rights Reserved
 * 
 **************************************************************************/
#include <iostream>
#include <com_log.h>
#include <gtest/gtest.h>
#include "producer.h"
#include "wc_log.h"

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    int ret = com_loadlog("../conf/", "log.conf");
    if (0 != ret) {
        fprintf(stderr, "comlog load main configure error");
    }
    return RUN_ALL_TESTS();
}

class test_Producer_suite : public ::testing::Test{
protected:
    wyr::Producer* p_producer;
    test_Producer_suite(){};
    virtual ~test_Producer_suite(){};
    virtual void SetUp() {
        p_producer = new wyr::Producer();
    };
    virtual void TearDown() {
        if (NULL != p_producer) {
            delete p_producer;
        }
    };
};

//set _head_block points to the 'W' in "World", after initialize, 
//_head_line points to the 'I' in "I Love You"
TEST_F(test_Producer_suite, test_initialize_headline__true)
{
    wyr::BlockingQueue wc_queue;
    ASSERT_EQ(0, wc_queue.init());
    const char* myfile = "Hello World Yanran!\nI Love You\nMe Me Da!\n";
    uint64_t len = strlen(myfile);
    const char* tail_file = myfile + len;
    const char* head_block = myfile + 6;
    bool is_head_file(false);
    uint64_t block_size_file = myfile + len - head_block;
    ASSERT_EQ(0, p_producer->init(&wc_queue, 1, 
                head_block, tail_file, block_size_file, is_head_file));
    EXPECT_EQ(0, p_producer->_initialize_headline());
    EXPECT_EQ(head_block + 14, p_producer->head_line());
}

//set _head_block points to the 'D' in "Da", after initialize,
//_head_line points to the end of myfile, WARNING, return 1
TEST_F(test_Producer_suite, test_initialize_headline__false_1)
{
    wyr::BlockingQueue wc_queue;
    ASSERT_EQ(0, wc_queue.init());
    const char* myfile = "Hello World Yanran!\nI Love You\nMe Me Da!\n";
    uint64_t len = strlen(myfile);
    const char* tail_file = myfile + len;
    const char* head_block = myfile + len - 4;
    bool is_head_file(false);
    uint64_t block_size_file = myfile + len - head_block;
    ASSERT_EQ(0, p_producer->init(&wc_queue, 1, head_block, 
                                tail_file, block_size_file, is_head_file));
 
    EXPECT_EQ(1, p_producer->_initialize_headline());
}

//there is no '\n' in the block. _head_line cannot 
//points to the next line 
TEST_F(test_Producer_suite, test_initialize_headline__false_2)
{
    wyr::BlockingQueue wc_queue;
    ASSERT_EQ(0, wc_queue.init());
    const char* myfile = "Hello World Yanran!I Love YouMe Me Da!";
    uint64_t len = strlen(myfile);
    const char* tail_file = myfile + len;
    const char* head_block = myfile + 3;
    bool is_head_file(false);
    uint64_t block_size_file = myfile + len - head_block;
    ASSERT_EQ(0, p_producer->init(&wc_queue, 1, head_block, 
                            tail_file, block_size_file, is_head_file));
 
    p_producer->_initialize_headline();
    EXPECT_EQ(-1, p_producer->_initialize_headline());
}

TEST_F(test_Producer_suite, test_get_line__true)
{
    wyr::BlockingQueue wc_queue;
    ASSERT_EQ(0, wc_queue.init());
    const char* myfile = "Hello World Yanran!\nI Love You\nMe Me Da!\n";
    uint64_t len = strlen(myfile);
    const char* tail_file = myfile + len;
    const char* head_block = myfile;
    bool is_head_file(true);
    uint64_t block_size_file = len;
    ASSERT_EQ(0, p_producer->init(&wc_queue, 1, head_block, tail_file, 
                                    block_size_file, is_head_file));
    ASSERT_EQ(0, p_producer->_initialize_headline());
    EXPECT_EQ(0, p_producer->_get_line(&(p_producer->_sentence)));
    EXPECT_EQ(0, p_producer->sentence().compare("Hello World Yanran!"));
    EXPECT_EQ(0, p_producer->_get_line(&(p_producer->_sentence)));
    EXPECT_EQ(0, p_producer->sentence().compare("I Love You"));
    EXPECT_EQ(0, p_producer->_get_line(&(p_producer->_sentence)));
    EXPECT_EQ(0, p_producer->sentence().compare("Me Me Da!"));
    EXPECT_EQ(1, p_producer->_get_line(&(p_producer->_sentence)));
}

//there is no '\n' 
TEST_F(test_Producer_suite, test_get_line__false)
{
    wyr::BlockingQueue wc_queue;
    ASSERT_EQ(0, wc_queue.init());
    const char* myfile = "Hello World Yanran!I Love YouMe Me Da!";
    uint64_t len = strlen(myfile);
    const char* tail_file = myfile + len;
    const char* head_block = myfile;
    bool is_head_file(true);
    uint64_t block_size_file = len;
    ASSERT_EQ(0, p_producer->init(&wc_queue, 1, head_block, 
                            tail_file, block_size_file, is_head_file));
    ASSERT_EQ(0, p_producer->_initialize_headline());
    EXPECT_EQ(-1, p_producer->_get_line(&(p_producer->_sentence)));
}

TEST_F(test_Producer_suite, test_run_producer__true)
{
    wyr::BlockingQueue wc_queue;
    ASSERT_EQ(0, wc_queue.init());
    const char* myfile = "Hello World Yanran!\n\n\nI Love You\nMe Me Da!\n";
    uint64_t len = strlen(myfile);
    const char* tail_file = myfile + len;
    const char* head_block = myfile;
    bool is_head_file(true);
    uint64_t block_size_file = len;
    ASSERT_EQ(0, p_producer->init(&wc_queue, 1, head_block, 
                                tail_file, block_size_file, is_head_file));
 
    EXPECT_EQ(0, p_producer->_initialize_headline());
    EXPECT_EQ(0, p_producer->run_producer());
    EXPECT_EQ(3, (wc_queue._queue).size());
}

//the file is not end with '\n'
TEST_F(test_Producer_suite, test_run_producer__false)
{
    wyr::BlockingQueue wc_queue;
    ASSERT_EQ(0, wc_queue.init());
    const char* myfile = "Hello World Yanran!\nI Love YouMe Me Da!";
    uint64_t len = strlen(myfile);
    const char* tail_file = myfile + len;
    const char* head_block = myfile;
    bool is_head_file(true);
    uint64_t block_size_file = len;
    ASSERT_EQ(0, p_producer->init(&wc_queue, 1, head_block, 
                                tail_file, block_size_file, is_head_file));
 
    EXPECT_EQ(0, p_producer->_initialize_headline());
    EXPECT_EQ(-1, p_producer->run_producer());
    EXPECT_EQ(1, (wc_queue._queue).size());
}
