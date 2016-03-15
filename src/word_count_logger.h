/***************************************************************************
 * 
 * Copyright (c) 2014. All Rights Reserved
 * 
 **************************************************************************/
/**
 * @file word_count_logger.h
 * @author wangyanran100@gmail.com
 * @date 2014/07/24 16:52:14
 * @brief 
 *  
 **/

#ifndef  WYR_WORD_COUNT_LOGGER_H
#define  WYR_WORD_COUNT_LOGGER_H

#include "wc_log.h"

namespace wyr {

class WordCountLogger {
public:
    WordCountLogger() {}
    ~WordCountLogger() {
        wc_close_log();
    }
   
    bool init(const char* p_dir, const char* p_file) {
        return wc_init_log(p_dir, p_file);
    }
};

} // namespace wyr
#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
