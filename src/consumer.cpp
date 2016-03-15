#include "consumer.h"

namespace wyr {

Consumer::Consumer() : 
    _p_wc_queue(NULL),
    _p_counter_map(NULL),
    _p_basic_vec(NULL),
    _p_wc_segword(NULL),
    _query_count(0) {
    memset(_query, 0, sizeof(_query));
}

Consumer::~Consumer() {
    if (NULL != _p_wc_segword) {
        _p_wc_segword->end_segword();
        delete _p_wc_segword;
        _p_wc_segword = NULL;
    }
}
int Consumer::init(
        const scw_worddict_t* p_pwdict,
        wc_result_map_t* p_counter_map,
        wc_basic_vec_t* p_basic_vec,
        BlockingQueue* p_wc_queue) {
    _p_wc_segword = new (std::nothrow) Segword();
    if (NULL == _p_wc_segword) {
        WC_LOG_FATAL("new Segword() failed");
        return -1;
    }
    if (0 != _p_wc_segword->init(p_pwdict)) {
        WC_LOG_FATAL("segword init() failed");
        return -1;
    }
    _p_counter_map = p_counter_map;
    _p_basic_vec = p_basic_vec;
    _p_wc_queue = p_wc_queue;
    return 0;
}

uint64_t Consumer::query_count() const {
    return _query_count;
}

const std::string& Consumer::sentence() const {
    return _sentence;
}
void* Consumer::static_run_consumer(void* p_para) {
    Consumer* p_c1 = (Consumer*)p_para;
    if (!wc_initthread_log()) {
        WC_LOG_FATAL("wc_initthread_log failed");
        return NULL;    
    }
    if (0 != p_c1->run_consumer()) {
        WC_LOG_FATAL("run_consumer() failed");
    }
    wc_closethread_log();
    return NULL;    
}

int Consumer::run_consumer() {
    while (true) {
        int ret = _get_line(&_sentence);
        if (1 == ret) {
            WC_LOG_TRACE("run_consumer get line end");
            return 0;
        } 

        ret = split_string_get_column(_sentence, COLUMN_INDEX, 
                DELIMITER, _query, LINE_MAX_LENGTH);
        if (0 != ret) {
            WC_LOG_WARNING("[%s] run_consumer split error", _sentence.c_str());
            return -1;
        } 
        if (strlen(_query) == 0) {
            continue;
        }

        ret = _seg_word(_query);
        if (-1 == ret) {
            WC_LOG_WARNING("[%s] run_consumer segword error", _query);
            return -1;
        } else if (1 == ret) {
            continue;
        } else {
            _query_count++;
            int ret1 = _segword_result_into_storage();
            if (1 == ret1) {
                continue;
            } else if (-1 == ret1) {
                WC_LOG_WARNING("run_consumer segword_result_into_hashmap error");
                return -1;
            } else {
                continue;
            }
        }
    }
}

int Consumer::_get_line(std::string* p_sentence) {
    while (0 == _queue_item.size()) {
        _queue_item = _p_wc_queue->pull();
    }
    *p_sentence = _queue_item.back();
    _queue_item.pop_back();
    int ret = p_sentence->compare(PRODUCER_END_LABEL);
    if (0 != ret) {
        return 0;
    } else {
        WC_LOG_TRACE("consumer encounter producer end label");
        return 1;
    }
}

int Consumer::split_string_get_column(
        const std::string& sentence, 
        uint32_t nfields, 
        const std::string& delimiter, 
        char* p_query,
        uint32_t query_length) {
    char* fields[nfields + 1];
    memset(fields, 0, sizeof(fields));
    size_t len = delimiter.size();
    if (len <= 0) {
        return -1;
    }
    char buffer[LINE_MAX_LENGTH];
    snprintf(buffer, sizeof(buffer), "%s", sentence.c_str());
    int len_buffer = strlen(buffer); 
    fields[0] = buffer;
    char* start = fields[0];

    for (uint32_t i = 0; i < nfields; i++) {
        // the nfields requested is over the sentence
        if (start >= buffer + len_buffer) {
            return -1;
        }
        // cannot find the delimiter
        char* pos = strstr(start, delimiter.c_str()); 
        if (NULL == pos) {
            return -1;
        }
        for (uint32_t j = 0; j < len; j++) {
            pos[j] = 0;
        }
        start = pos + len;
        fields[i + 1] = start;
    }
    snprintf(p_query, query_length, "%s", fields[nfields - 1]);
    return 0;
}

int Consumer::_seg_word(char* p_query) {
    int ret = _p_wc_segword->run_segword(p_query);
    if (-1 == ret) {
        WC_LOG_WARNING("[%s] consumer seg_word error", p_query);
        return -1;
    // query is consist of space
    } else if (1 == ret) {
        return 1;
    } else {
        return 0;
    }
}

int Consumer::_segword_result_into_storage() {
    int segword_word_num = _p_wc_segword->segword_word_num();
    if (0 == segword_word_num) {
        WC_LOG_WARNING("segword_result_into_hashmap: segword_word_num = 0");
        return 1;
    }

    const token_t* p_results = _p_wc_segword->segword_word_result();
    if (1 == segword_word_num) {
        std::string basic_word(p_results[0].buffer);  
        _p_basic_vec->push_back(basic_word);
    }

    wc_result_map_pair_t* ptr;
    std::string oneword;
    for (int i = 0; i < segword_word_num; i++) {
        oneword.clear();
        oneword.assign(p_results[i].buffer);
        unsigned long long sign_s_tmp;
        int ret = uln_sign_f64(oneword.c_str(), oneword.size(), &sign_s_tmp);
        if (-1 == ret) {
            WC_LOG_WARNING("[%s] word sign error", oneword.c_str());
            return -1;
        }
        uint64_t sign_s = sign_s_tmp;

        ptr = _p_counter_map->find(sign_s);
        if (NULL != ptr) {
            (ptr->second).first++;
        } else {
            ret = _p_counter_map->set(sign_s, make_pair(1, oneword));
            if (-1 == ret) {
                WC_LOG_WARNING("[%s] [%lu] hashmap insert error", oneword.c_str(), sign_s);
                return -1;
            }
        }
    }
    return 0;
}

} // namespace wyr
