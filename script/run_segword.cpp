/***************************************************************************
 * 
 * Copyright (c) 2014. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file run_segword.cpp
 * @author wangyanran100@gmail.com
 * @date 2014/07/24 15:32:19
 * @description: 
         -input: query
             -format:
                 query1
                 query2
                 ...
         -output: the segword results of each query
             -format:
                 query1   segword1    segword2 ...
 **/

#include <fstream>
#include <iostream> 
#include <stdio.h>
#include <isegment.h>

const uint32_t LINE_MAX_SIZE = 10000;
const uint32_t SEGWORD_MAX_TERM = 10000;
const uint32_t TOKENS_NUM = 1024;

int main() {
    FILE* fin = fopen("../data/inputfile/query.truth", "r");
    if (!fin) {
        fprintf(stderr, "ifstream open error");
        return -1;
    }
    std::ofstream fout("../data/result/segword_result");
    if (!fout) {
        fprintf(stderr, "ofstream open error");
        return -1;
    }
   
    //initialize segword 
    pgconf = scw_load_conf("../conf/scw.conf");
    if (NULL == pgconf) {
        fprintf(stderr, "load wordseg conf error");
        return -1;
    }
    const scw_worddict_t* p_wdict = scw_load_worddict("../data/");
    if (NULL == p_wdict) {
        fprintf(stderr, "load worddict error");
        return -1;
    }
    int scw_out_flag = SCW_OUT_BASIC | SCW_OUT_WPCOMP;
    scw_out_t* p_pout;
    if (NULL == (p_pout = scw_create_out(SEGWORD_MAX_TERM, scw_out_flag))) {
        fprintf(stderr, "initializing the output buffer error");
        return -1;
    }
    char line[LINE_MAX_SIZE];
    token_t tokens[TOKENS_NUM];
    uint32_t segword_word_num;
    uint32_t count = 0;

    //run segword
    while (fgets(line, LINE_MAX_SIZE, fin)) {
        if (NULL == line) {
            fprintf(stderr, "line == NULL");
            continue;
        }
        if (0 == strlen(line)) {
            continue;
        }
        // check if string is consist of space 
        size_t len = strlen(line);
        while ((line[len - 1] == '\r') || (line[len - 1] == '\n') || (line[len - 1] == ' ')) {
            line[--len] = 0;
            if (0 == len) {
                continue;
            }
        }
        // segment  string
        if (-1 == scw_segment_words(const_cast<scw_worddict_t*>(p_wdict), p_pout, 
                                    line, len, LANGTYPE_SIMP_CHINESE, NULL)) {
            fprintf(stderr, "[%s]segword failed error", line);
            continue;
        }
        segword_word_num = scw_get_token_1(p_pout, SCW_OUT_WPCOMP, tokens, TOKENS_NUM);
        if (0 != segword_word_num) {
            fout << line;
            for (uint32_t i = 0; i < segword_word_num; ++i) {
                fout << "\t" << tokens[i].buffer;
            }
            fout << std::endl;
        }
        count++;
        if (0 == count % 100000) {
            fprintf(stdout, "%d finished\n", count);
        }
    }

    //destroy segword
    if (NULL != p_pout) {
        scw_destroy_out(p_pout);
        p_pout = NULL;
    }
    if (NULL != p_wdict) {
        scw_destroy_worddict(const_cast<scw_worddict_t*>(p_wdict));
        p_wdict = NULL;
        pgconf = NULL;
    }
    if (NULL != pgconf) {
        scw_destroy_conf(pgconf);
        pgconf = NULL;
    }

    fout.close();
    return 0;
}
