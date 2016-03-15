#include "producer.h"

namespace wyr {

Producer::Producer() : 
        _p_wc_queue(NULL),
        _p_head_block(NULL), 
        _p_tail_file(NULL), 
        _is_head_file(false),
        _block_size_file(0), 
        _item_capacity_queue(0) {
}

int Producer::init(
        BlockingQueue* wc_queue, 
        uint32_t item_capacity_queue, 
        const char* head_in, 
        const char* tail_file, 
        uint64_t block_size_file, 
        bool is_head_file) { 
    if (NULL == wc_queue || NULL == head_in || NULL == tail_file) {
        WC_LOG_FATAL("params = NULL init() failed");
        return -1;
    }
    _p_head_block = head_in;
    _p_tail_file = tail_file;
    _block_size_file = block_size_file;
    _item_capacity_queue = item_capacity_queue;
    _p_wc_queue = wc_queue;
    _is_head_file = is_head_file;
    return 0;
}

const char* Producer::head_line() const {
    return _p_head_line;
}

const std::string& Producer::sentence() const {
    return _sentence;
}

void* Producer::static_run_producer(void* p_para) {
    Producer* p_this = (Producer*)p_para;
    if (!wc_initthread_log()) {
        WC_LOG_FATAL("wc_initthread_log failed");
        return NULL;    
    }
    if (0 != p_this->run_producer()) {
        WC_LOG_FATAL("run_producer() failed");
    }
    wc_closethread_log();
    return NULL;    
}

int Producer::run_producer() {
    int ret = _initialize_headline();
    if (-1 == ret) {
        WC_LOG_FATAL("run_producer headline initialize error");
        return -1;
    } 
    if (1 == ret) {
        WC_LOG_WARNING("run_producer headline initialize over tail");
        return 1;
    }
    std::vector<std::string> queue_item;
    while (true) {
        queue_item.clear();
        while (queue_item.size() != _item_capacity_queue) {
            ret = _get_line(&_sentence);
            if (-1 == ret) {
                WC_LOG_FATAL("run_producer getline error");
                return -1;
            } else if (1 == ret) {
                WC_LOG_TRACE("run_producer getline end!");
                if (queue_item.size() != 0) {
                    _p_wc_queue->put(&queue_item);
                }
                return 0;
            } else {
                if (_sentence.size() == 0) {
                    continue;
                }
                queue_item.push_back(_sentence);
            }
        }
        _p_wc_queue->put(&queue_item);
    }
}

// set _p_head_line points to the head of a line. 
// if _p_head_block points to the head of file, then _p_head_line points to the head of file
// else _p_head_line points to the next charactor of  "\n" first found from _p_head_block
int Producer::_initialize_headline() {
    char* p_mover = 0;
    _p_head_line = _p_head_block;
    if (!_is_head_file) {
        if (_p_tail_file < _p_head_block) {
            WC_LOG_FATAL("_p_tail_file < _p_head_block error");
            return -1;
        }
        uint32_t seek_length = std::min((uint32_t)(_p_tail_file - _p_head_block), LINE_MAX_LENGTH); 
        p_mover = (char*)memchr((void*)_p_head_block, '\n', seek_length);
        if (NULL == p_mover) {
            WC_LOG_FATAL("headline initialize: cannot find end of line error");
            return -1;
        }

        p_mover++;
        _p_head_line = p_mover;
        if (_p_head_line >= _p_tail_file) {
            WC_LOG_WARNING("headline initialize over tail");
            return 1;
        }
    }
    return 0;
}

int Producer::_get_line(std::string* p_sentence) {
    p_sentence->clear();
    char* p_mover = 0;
    if (_p_head_line >= _p_head_block + _block_size_file) {
        return 1;
    } else {
        if (_p_tail_file < _p_head_line) {
            WC_LOG_FATAL("_p_tail_file < _p_head_line error");
            return -1;
        }
        uint32_t seek_length = std::min((uint32_t)(_p_tail_file - _p_head_line), LINE_MAX_LENGTH); 
        p_mover = (char*)memchr((void*)_p_head_line, '\n', seek_length);
        if (NULL == p_mover) {
            WC_LOG_FATAL("get_line cannot find end of line error");
            return -1;
        }

        int len = p_mover - _p_head_line;
        p_sentence->assign(_p_head_line, len);
        p_mover++;
        _p_head_line = p_mover;
        return 0;
    }
} 

} // namespace wyr

