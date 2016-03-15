/***************************************************************************
 * 
 * Copyright (c) 2014. All Rights Reserved
 * 
 **************************************************************************/
 
/**
 * @file load_conf.h
 * @author wangyanran100@gmail.com
 * @date 2014/07/24 15:04:41
 * @brief 
 *  
 **/

#ifndef  WYR_WORD_COUNT_CONF_H
#define  WYR_WORD_COUNT_CONF_H

#include <string>
#include <Configure.h>
#include "wc_log.h"

namespace wyr {

class WordCountConf {
public:
    WordCountConf();
    ~WordCountConf() {}

    static WordCountConf& instance() {
        return _instance;
    } 

    int load_conf(const char* p_conf_dir, const char* p_conf_name); 

    std::string input_file;
    std::string basic_words_file;
    std::string word_count_file;
    std::string segword_conf_file;
    std::string segword_dict_path;
    uint32_t producer_thread_count;
    uint32_t consumer_thread_count;
    uint32_t queue_item_capacity; 

private:
    static WordCountConf _instance;
};

inline WordCountConf& wc_conf() {
    return WordCountConf::instance();
}

} // namespace wyr
#endif  //__LOAD_CONF_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
