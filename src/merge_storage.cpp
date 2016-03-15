/***************************************************************************
 * 
 * Copyright (c) 2014, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file merge_storage.cpp
 * @author wangyanran100@gmail.com
 * @date 2014/07/24 21:12:51
 * @brief 
 *  
 **/
#include "merge_storage.h"

namespace wyr{

int MergeStorage::init() {
    if (0 != _result_final_map.create(HASHMAP_BITEMS)) {
        WC_LOG_FATAL("hashmap create() failed");
        return -1;
    } 
    return 0;
}

int MergeStorage::_merge_hashmap(const std::vector<wc_result_map_t>& p_results) {
    int idx = 0;
    wc_result_map_pair_t* ptr;
    int map_num = p_results.size();
    while (idx < map_num) {
        wc_result_map_t::const_iterator iter = p_results[idx].begin();
        for (; iter != p_results[idx].end(); ++iter) {
            ptr = _result_final_map.find(iter->first);
            if (NULL != ptr) {
                (ptr->second).first += (iter->second).first;
            } else {
                int ret = _result_final_map.set(iter->first, iter->second);
                if (-1 == ret) {
                    WC_LOG_FATAL("insert hashmap error");
                    return -1;
                }
            }
        }
        idx++;
    }
    return 0;
}

int MergeStorage::_merge_vector(const std::vector<wc_basic_vec_t>& p_basics) {
    int item_num = p_basics.size();
    try {
        for (int i = 0; i < item_num; i++) {
            _basic_final_vec.insert(_basic_final_vec.end(),
                        p_basics[i].begin(), p_basics[i].end());
        }
    } catch (...) {
        WC_LOG_FATAL("vector insert() failed");
        return -1;
    }
    return 0;
}

int MergeStorage::run(const std::vector<wc_result_map_t>& p_results,
                      const std::vector<wc_basic_vec_t>& p_basics, 
                      const uint64_t query_count) {
    if (0 != init()) {
        WC_LOG_FATAL("init() failed");
        return -1;
    }

    //basic words
    if (0 != _merge_vector(p_basics)) {
        WC_LOG_FATAL("_merge_vector() failed");
        return -1;
    }
    std::ofstream fout_basic(wc_conf().basic_words_file.c_str());
    if (!fout_basic) {
        WC_LOG_FATAL("basic_words_file(%s) open failed", 
                      wc_conf().basic_words_file.c_str());
        return -1;
    }
    for (wc_basic_vec_t::iterator it = _basic_final_vec.begin(); 
        it != _basic_final_vec.end(); ++it) {
        fout_basic << *it << std::endl;
    }  
    fout_basic.close();
    fprintf(stdout, "The Ratio of Basic Words is [%d/%d]\n", 
            _basic_final_vec.size(), query_count); 

    //segment words 
    if (0 != _merge_hashmap(p_results)) {
        WC_LOG_FATAL("_merge_hashmap() failed");
        return -1;
    }
    std::ofstream fout(wc_conf().word_count_file.c_str());
    if (!fout) {
        WC_LOG_FATAL("word_count_file(%s) open failed", 
                wc_conf().word_count_file.c_str());
        return -1;
    }
    uint64_t word_count = 0;
    for (wc_result_map_t::iterator it = _result_final_map.begin();
         it != _result_final_map.end(); ++it){
        word_count += (it->second).first;
    }
    double ratio;
    for (wc_result_map_t::iterator it = _result_final_map.begin();
            it != _result_final_map.end(); ++it){
        ratio = (it->second).first * 1.0 / word_count;  
        fout << (it->second).second << "\t" << (it->second).first 
             << "\t" << ratio << std::endl;
    }
    fout.close();
    return 0;
}

}
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
