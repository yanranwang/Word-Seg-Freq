/***************************************************************************
 * 
 * Copyright (c) 2014. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file main.cpp
 * @author wangyanran100@gmail.com
 * @date 2014/07/24 15:32:19
 * @brief 
 *  
 **/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <gflags/gflags.h>
#include "wc_log.h"
#include "word_count_logger.h"
#include "word_count_conf.h"
#include "processor.h"
#include "merge_storage.h"

DEFINE_string(d, "./conf/", "Configure File Dir" );
DEFINE_string(i, "wordcount.conf", "Program Configure File Name" );
DEFINE_string(j, "log.conf", "Log Configure File Name" );
DEFINE_bool(h, false, "Print Description");

void print_description() {
    fprintf(stdout, "SYNOPSIS:  ./wordcount [-i] command\n");
    fprintf(stdout, "OPTION:    -d\n");
    fprintf(stdout, "       input the dir of configure file\n");
    fprintf(stdout, "OPTION:    -i\n");
    fprintf(stdout, "       input the name of the program configure file\n");
    fprintf(stdout, "OPTION:    -j\n");
    fprintf(stdout, "       input the name of the log configure file\n");
}

int main(int argc, char* argv[]) {
    try {
        google::ParseCommandLineFlags(&argc, &argv, true);
        if (FLAGS_h) {
            print_description();
            return 0;
        }
    } catch (...) {
        fprintf(stderr, "main params error\n");
        return -1;
    }
  
    wyr::WordCountLogger logger; 
    const char* dir = FLAGS_d.c_str();
    const char* file_j = FLAGS_j.c_str();
    if (!logger.init(dir, file_j)) {
       fprintf(stderr, "init log [%s/%s] failed\n", dir, file_j); 
       return -1;
    } 
    
    const char* file_i = FLAGS_i.c_str();
    if (-1 == wyr::wc_conf().load_conf(dir, file_i)) {
        WC_LOG_FATAL("load_conf(%s, %s) failed", dir, file_i);
        return -1;
    }

    wyr::Processor wc_processor;
    if (0 != wc_processor.init()) {
        WC_LOG_FATAL("processor init() failed");
        return -1;
    }
    if (0 != wc_processor.run_processor()) {
        WC_LOG_FATAL("processor run_processor() failed");
        return -1;
    }

    return 0;
}
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
