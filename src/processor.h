/***************************************************************************
 * 
 * Copyright (c) 2014. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file processor.h
 * @author wangyanran100@gmail.com
 * @date 2014/07/24 17:56:32
 * @brief 
 *  
 **/
#ifndef  WYR_PROCESSOR_H
#define  WYR_PROCESSOR_H

#include <fcntl.h>
#include "blocking_queue.h"
#include "segword.h"
#include "producer.h"
#include "consumer.h"
#include "word_count_conf.h"
#include "merge_storage.h"
#include "wc_log.h"

namespace wyr {

class Processor {
public:
    Processor();
    ~Processor();
    int init();
    int run_processor();
    uint64_t query_count() const;
    const std::vector<wc_result_map_t>& wc_result_vec() const;
    const std::vector<wc_basic_vec_t>& wc_basic_vec() const; 

private:
    int _load_wordseg_conf();
    int _load_input_file();

    const char* _p_file_base_addr;
    BlockingQueue _wc_queue; 
    scw_worddict_t* _p_pwdict;
    std::vector<wc_result_map_t> _wc_result_vec;
    std::vector<wc_basic_vec_t> _wc_basic_vec;
    std::vector<Segword> _wc_segword_vec;
    uint64_t _query_count;
    uint64_t _inputfile_size;
    uint32_t _consumer_thread_count;
    uint32_t _producer_thread_count;
};

} // namespace wyr

#endif  // __PROCESSOR_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
