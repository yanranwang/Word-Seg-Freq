/***************************************************************************
 * 
 * Copyright (c) 2014. All Rights Reserved
 * 
 **************************************************************************/
 
/**
 * @file processor.cpp
 * @author wangyanran100@gmail.com
 * @date 2014/07/24 18:17:52
 * @brief 
 *  
 **/

#include "processor.h"

namespace wyr {

Processor::Processor() : 
    _p_file_base_addr(NULL),
    _p_pwdict(NULL),
    _query_count(0) {
}

Processor::~Processor() {
    if (NULL != _p_file_base_addr) {
        if (0 != munmap(const_cast<char*>(_p_file_base_addr), _inputfile_size)) {
            WC_LOG_FATAL("munmap() failed");
        }
    }
    // scw_destroy_worddict() contains scw_destroy_conf();
    // if _p_pwdict != NULL, use scw_destroy_worddict() to destroy both
    // if _p_pwdict == NULL, still need scw_destroy_conf() to destroy pgconf
    if (NULL != _p_pwdict) {
        scw_destroy_worddict(_p_pwdict);
        _p_pwdict = NULL;
        pgconf = NULL;
    }
    if (NULL != pgconf) {
        scw_destroy_conf(pgconf);
        pgconf = NULL;
    }
}

uint64_t Processor::query_count() const {
    return _query_count;
}

const std::vector<wc_result_map_t>& Processor::wc_result_vec() const {
    return _wc_result_vec;
}

const std::vector<wc_basic_vec_t>& Processor::wc_basic_vec() const {
    return _wc_basic_vec;
}

int Processor::_load_input_file() {
    int fd = open(wc_conf().input_file.c_str(), O_RDONLY);
    if (-1 == fd) {
        WC_LOG_FATAL("open inputfile=%s error", wc_conf().input_file.c_str());
        return -1;
    }

    struct stat buf;
    fstat(fd, &buf);
    _inputfile_size = buf.st_size;
    _p_file_base_addr = (char*)mmap(0, _inputfile_size, PROT_READ, MAP_SHARED, (int)fd, SEEK_SET);
    if ((void *)-1 == _p_file_base_addr) {
        com_writelog(COMLOG_FATAL, "mmap error");
        return -1;
    }
    return 0;
}

int Processor::_load_wordseg_conf() {
    pgconf = scw_load_conf(wc_conf().segword_conf_file.c_str());
    if (NULL == pgconf) {
        com_writelog(COMLOG_FATAL, "load wordseg conf error");
        return -1;
    }
    _p_pwdict = scw_load_worddict(wc_conf().segword_dict_path.c_str());
    if (NULL == _p_pwdict) {
        com_writelog(COMLOG_FATAL, "load worddict error");
        return -1;
    }
    return 0;
}

int Processor::init() {
    if (0 != _load_wordseg_conf()) {
        WC_LOG_FATAL("_load_wordseg_conf() failed");
        return -1;
    }
    if (0 != _load_input_file()) {
        WC_LOG_FATAL("_load_input_file() failed");
        return -1;
    }
    if (0 != _wc_queue.init()) {
        WC_LOG_FATAL("BlockingQueue init() failed");
        return -1;
    }
    
    _consumer_thread_count = wc_conf().consumer_thread_count;
    _producer_thread_count = wc_conf().producer_thread_count;
    try {
        _wc_result_vec.resize(_consumer_thread_count);
        _wc_basic_vec.resize(_consumer_thread_count);
    } catch (...) {
        WC_LOG_FATAL("vector resize() failed");
        return -1;
    }
    for (uint32_t i = 0; i < _consumer_thread_count; ++i) {
        if (0 != _wc_result_vec[i].create(HASHMAP_BITEMS)) {
            WC_LOG_FATAL("Hashmap create() failed");
            return -1;
        }
    }
    return 0;
}

int Processor::run_processor() {
    // producer
    pthread_t producer_thread[_producer_thread_count]; 
    Producer wc_producer[_producer_thread_count];
    uint64_t block_size = _inputfile_size / _producer_thread_count;
    for (uint32_t i = 0; i < _producer_thread_count; ++i) {
        const char* head_in = _p_file_base_addr + block_size * i;
        const char* tail_file = _p_file_base_addr + _inputfile_size;
        bool is_head_file = (i == 0);
        int ret = wc_producer[i].init(&_wc_queue, wc_conf().queue_item_capacity,
                                 head_in, tail_file, block_size, is_head_file);
        if (0 != ret) {
            WC_LOG_FATAL("[%lu]producer init() failed", producer_thread[i]);
            return -1;
        }
        ret = pthread_create(&producer_thread[i], NULL, 
                Producer::static_run_producer, &wc_producer[i]);
        if (0 != ret) {
            WC_LOG_FATAL("[%lu]producer pthread_create() failed", producer_thread[i]);
            return -1;
        }
    }

    // consumer
    pthread_t consumer_thread[_consumer_thread_count];
    Consumer wc_consumer[_consumer_thread_count];
    for (uint32_t i = 0; i < _consumer_thread_count; ++i){        
        int ret = wc_consumer[i].init(_p_pwdict, &_wc_result_vec[i], 
                                    &_wc_basic_vec[i], &_wc_queue);
        if (0 != ret) {
            WC_LOG_FATAL("[%lu]consumer init() failed", consumer_thread[i]);
            return -1;
        }
        ret = pthread_create(&consumer_thread[i], NULL, 
                Consumer::static_run_consumer, &wc_consumer[i]);
        if (0 != ret) {
            WC_LOG_FATAL("[%lu]consumer pthread_create() failed", consumer_thread[i]);
            return -1;
        }
    }

    // wait for producers end
    for (uint32_t i = 0; i < _producer_thread_count; ++i) {
        if (0 != pthread_join(producer_thread[i], NULL)) {
            WC_LOG_FATAL("[%lu]pthread_join() failed", producer_thread[i]);
            return -1;
        }        
    }
    WC_LOG_TRACE("*************PRODUCER END***************");
    
    // insert the label of producer end 
    std::vector<std::string> queue_end_item;
    queue_end_item.push_back(PRODUCER_END_LABEL);
    for (uint32_t i = 0; i < _consumer_thread_count; ++i) {
        _wc_queue.put(&queue_end_item);
    }

    // wait for all consumers end
    for (uint32_t i = 0; i < _consumer_thread_count; ++i) {
        if (0 != pthread_join(consumer_thread[i], NULL)) {
            WC_LOG_FATAL("[%lu]pthread_join() failed", consumer_thread[i]);
            return -1;
        }
    }

    WC_LOG_TRACE("*************CONSUMER END***************");
    for (uint32_t i = 0; i < _consumer_thread_count; ++i) {
        _query_count += wc_consumer[i].query_count();
    }

    // merge && storage
    MergeStorage postprocessor;
    int ret = postprocessor.run(_wc_result_vec,
                                _wc_basic_vec, _query_count);
    if (0 != ret) {
        WC_LOG_FATAL("MergeStorage run() failed");
        return -1; 
    }
    WC_LOG_TRACE("*************MERGE_STORAGE_END***************");
    WC_LOG_TRACE("*************RUN_PROCESSOR END***************");
    return 0;
}

} // namespace wyr
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
