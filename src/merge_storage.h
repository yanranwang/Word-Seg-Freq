/***************************************************************************
 * 
 * Copyright (c) 2014. All Rights Reserved
 * 
 **************************************************************************/



/**
 * @file merge_storage.h
 * @author wangyanran100@gmail.com
 * @date 2014/07/24 20:52:21
 * @brief 
 *  
 **/

#ifndef  WYR_MERGE_STORAGE_H
#define  WYR_MERGE_STORAGE_H

#include <vector>
#include <fstream>
#include "wc_log.h"
#include "common_data.h"
#include "word_count_conf.h"

namespace wyr {

class MergeStorage {
public:
    MergeStorage() {}
    ~MergeStorage() {}

    //initialize 
    int init();
    
    //merge results of multi-thread and write into file  
    int run(const std::vector<wc_result_map_t>& p_results,
            const std::vector<wc_basic_vec_t>& p_basics, const uint64_t query_count);

private:
    //merge the segment words result
    int _merge_hashmap(const std::vector<wc_result_map_t>& p_results);
    
    //merge basic words result
    int _merge_vector(const std::vector<wc_basic_vec_t>& p_basics);

    wc_result_map_t _result_final_map;
    wc_basic_vec_t _basic_final_vec;
};

}
#endif  //__MERGE_STORAGE_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
