#ifndef WYR_CONSUMER_H
#define WYR_CONSUMER_H

#include <com_log.h> 
#include <uln_sign_f64.h>
#include "common_data.h"
#include "segword.h"
#include "wc_log.h"
#include "blocking_queue.h"

namespace wyr {

class Consumer {
public:
    // split the sentence, and get the requested field
    static int split_string_get_column(
            const std::string& sentence, 
            uint32_t nfields,
            const std::string& delimiter, 
            char* p_query,
            uint32_t query_length); 
 
    Consumer();
    ~Consumer(); 
    
    // initialize some params
    int init(
            const scw_worddict_t* p_pwdict, 
            wc_result_map_t* p_counter_map,
            wc_basic_vec_t* p_basic_vec,
            BlockingQueue* p_wc_queue);
    
    // get _query_count
    uint64_t query_count() const; 
    
    // get _sentence
    const std::string& sentence() const; 
    
    // the main function of Consumer: run_consumer
    static void* static_run_consumer(void* para); 
    
    int run_consumer();

private:
    // get one line from buffer
    int _get_line(std::string* p_sentence);
    
    // run segword
    int _seg_word(char* p_query);
    
    // insert the results of segword into hashmap
    int _segword_result_into_storage(); 

    // not own
    BlockingQueue* _p_wc_queue;
    // not own
    wc_result_map_t* _p_counter_map;
    // not own
    wc_basic_vec_t* _p_basic_vec;

    Segword* _p_wc_segword;
    std::vector<std::string> _queue_item;
    std::string _sentence;
    char _query[LINE_MAX_LENGTH];
    uint64_t _query_count;
};

} // namespace wyr
#endif
