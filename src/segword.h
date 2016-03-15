#ifndef WYR_SEGWORD_H
#define WYR_SEGWORD_H

#include <com_log.h>
#include <isegment.h>
#include "wc_log.h"
#include "common_data.h"

namespace wyr {

class Segword {
public:
    Segword(); 
    ~Segword() {}
    
    // initialize some params of segword
    int init(const scw_worddict_t* pwdict);
    
    int run_segword(char* line);
    
    // get the result of segword
    const token_t* segword_word_result() const; 
    
    // after segword, get amount  of segmented words
    uint32_t segword_word_num () const; 
    
    // destroy some params of segword
    void end_segword(); 

private:
     // not own
    const scw_worddict_t* _p_pwdict;
    
    scw_out_t* _p_pout;
    token_t _tokens[TOKENS_NUM];
    uint32_t _segword_word_num;
};

} // namespace wyr
#endif
