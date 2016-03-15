/***************************************************************************
 * 
 * Copyright (c) 2014. All Rights Reserved
 * 
 **************************************************************************/
/**
 * @file common_data.h
 * @author wangyanran100@gmail.com
 * @date 2014/07/15 18:30:29
 * @brief 
 *  
 **/
#ifndef  WYR_COMMON_DATA_H
#define  WYR_COMMON_DATA_H 

#include <stdint.h>
#include <vector>
#include <string>
#include <bsl/containers/hash/bsl_hashmap.h>

namespace wyr {

typedef bsl::hashmap<uint64_t, std::pair<uint64_t, std::string> > wc_result_map_t;
typedef std::pair<uint64_t, std::pair<uint64_t, std::string> > wc_result_map_pair_t;
typedef std::vector<std::string> wc_basic_vec_t;

static const uint32_t LINE_MAX_LENGTH = 10000;
static const uint32_t SEGWORD_MAX_TERM = 10000;
static const uint32_t TOKENS_NUM = 1024;
static const uint32_t COLUMN_INDEX = 15;
static const std::string DELIMITER("\t");
static const std::string PRODUCER_END_LABEL("#*#*#*#*#*");
static const uint32_t HASHMAP_BITEMS = 6000000;
} // namespace wyr
#endif  //__COMMON_DATA_H_
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
