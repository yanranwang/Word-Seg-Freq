#ifndef WYR_BLOCKING_QUEUE_H
#define WYR_BLOCKING_QUEUE_H

//This class comes from Stack Over, as below
//http://stackoverflow.com/questions/5018783/c-pthread-blocking-queue-deadlock-i-think

#include <vector>
#include <string>
#include <queue>
#include <pthread.h>
#include "wc_log.h"

namespace wyr {

class BlockingQueue {
public:
    BlockingQueue() {}

    ~BlockingQueue(); 

    int init();
    
    // put one item into queue
    void put(const std::vector<std::string>* item); 

    // pull one item from queue
    std::vector<std::string> pull(); 

private:
    std::queue<std::vector<std::string> > _queue;
    pthread_mutex_t _lock;
    pthread_cond_t _cond;
};

} // namespace wyr
#endif
