#include "segword.h"

namespace wyr {

Segword::Segword() : _p_pout(NULL) {
}

int Segword::init(const scw_worddict_t* p_pwdict) {
        if (NULL == p_pwdict) {
            WC_LOG_FATAL("initializing segword pwdict error");
            return -1;
        }
        int scw_out_flag = SCW_OUT_BASIC | SCW_OUT_WPCOMP;
        if ((_p_pout = scw_create_out(SEGWORD_MAX_TERM, scw_out_flag)) == NULL) {
            WC_LOG_FATAL("initializing the output buffer error");
            return -1;
        }
        _p_pwdict = p_pwdict;
        return 0;
}

int Segword::run_segword(char* line) {
    _segword_word_num = 0;
    if (NULL == line) {
        return -1;
    }
    if (0 == strlen(line)) {
        return 1;
    }
    // check if string is consist of space 
    size_t len = strlen(line);
    while ((line[len - 1] == '\r') || (line[len - 1] == '\n') || (line[len - 1] == ' ')) {
        line[--len] = 0;
        if (0 == len) {
            return 1;
        }
    }
    // segment  string
    if (-1 == scw_segment_words(const_cast<scw_worddict_t*>(_p_pwdict), _p_pout, 
            line, len, LANGTYPE_SIMP_CHINESE, NULL)) {
        WC_LOG_WARNING("segword failed error");
        return -1;
    }
    _segword_word_num = scw_get_token_1(_p_pout, SCW_OUT_WPCOMP, _tokens, TOKENS_NUM);
    return 0;
}

const token_t* Segword::segword_word_result() const {
    return _tokens; 
}

uint32_t Segword::segword_word_num() const {
    return _segword_word_num;
}

void Segword::end_segword() {
    if (NULL != _p_pout) {
        scw_destroy_out(_p_pout);
        _p_pout = NULL;
    }
}

} // namespace wyr
