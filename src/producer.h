#ifndef WYR_PRODUCER_H
#define WYR_PRODUCER_H

#include <algorithm>
#include <com_log.h>
#include "wc_log.h"
#include "common_data.h"
#include "blocking_queue.h"

namespace wyr {

class Producer {
public:
    Producer();    
    ~Producer() {};

    // set private members
    int init(
        BlockingQueue* wc_queue, 
        uint32_t item_capacity_queue, 
        const char* head_in, 
        const char* tail_file, 
        uint64_t block_size_file, 
        bool is_head_file); 
    
    // get _sentence
    const std::string& sentence() const;
    
    // get _p_head_line
    const char* head_line() const;
    
    // the main function of producer, run producer
    static void* static_run_producer(void* para); 
    
    int run_producer();

private:
    // get one line from the inputfile
    int _get_line(std::string* p_sentence);
    
    // set _p_head_line points to the head of line 
    int _initialize_headline();
    
    // not own
    BlockingQueue* _p_wc_queue;

    const char* _p_head_block;
    const char* _p_tail_file;
    bool _is_head_file;
    const char* _p_head_line;
    std::string _sentence;
    uint64_t _block_size_file;
    uint32_t _item_capacity_queue;
};

} // namespace wyr
#endif
