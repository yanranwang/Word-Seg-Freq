#include "blocking_queue.h"

namespace wyr {

BlockingQueue::~BlockingQueue() {
    if (0 != pthread_mutex_destroy(&_lock)) {
        WC_LOG_FATAL("pthread_mutex_destroy() failed");
    }
    if (0 != pthread_cond_destroy(&_cond)) {
        WC_LOG_FATAL("pthread_cond_destroy() failed");
    }
}

int BlockingQueue::init() {
    int ret = pthread_mutex_init(&_lock, NULL);
    if (0 != ret) {
        WC_LOG_FATAL("QUEUE mutex initial error");
        return -1;
    }
    ret = pthread_cond_init(&_cond, NULL);
    if (0 != ret) {
        WC_LOG_FATAL("QUEUE cond initial error");
        return -1;
    }
    return 0;
}

void BlockingQueue::put(const std::vector<std::string>* item) {
    pthread_mutex_lock(&_lock);
    _queue.push(*item);
    pthread_cond_signal(&_cond);
    pthread_mutex_unlock(&_lock);
}

std::vector<std::string> BlockingQueue::pull() {
    pthread_mutex_lock(&_lock);
    while (_queue.empty()) {
        pthread_cond_wait(&_cond, &_lock);
    }
    std::vector<std::string> data = _queue.front();
    _queue.pop();
    pthread_mutex_unlock(&_lock);
    return data;
}

} // namespace wyr
