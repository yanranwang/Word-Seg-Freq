#include "word_count_conf.h"

namespace wyr {

WordCountConf WordCountConf::_instance;

WordCountConf::WordCountConf() : 
    producer_thread_count(0),
    consumer_thread_count(0),
    queue_item_capacity(0) {}

int WordCountConf::load_conf(const char* p_conf_dir, const char* p_conf_name) {
    if (p_conf_dir == NULL || p_conf_name == NULL) {
        WC_LOG_FATAL("p_conf_dir=%p, p_conf_name=%p", p_conf_dir, p_conf_name);
        return -1;
    }

    comcfg::Configure config;
    if (config.load(p_conf_dir, p_conf_name) != 0) {
        WC_LOG_FATAL("load conf [%s/%s] failed", p_conf_dir, p_conf_name);
        return -1;
    }

    const char* unit_name = "PARAMS";
    const comcfg::ConfigUnit& unit = config[unit_name];
    if (unit.selfType() == comcfg::CONFIG_ERROR_TYPE) {
        WC_LOG_FATAL("get %s unit failed", unit_name);
        return -1;
    }

    try {
        input_file = unit["INPUT_FILE"].to_cstr();
        basic_words_file = unit["BASIC_WORDS_FILE"].to_cstr();
        word_count_file = unit["WORD_COUNT_FILE"].to_cstr();
        segword_conf_file = unit["SEGWORD_CONF_FILE"].to_cstr();
        segword_dict_path = unit["SEGWORD_DICT_PATH"].to_cstr();
        producer_thread_count = unit["PRODUCER_THREAD_COUNT"].to_uint32();
        consumer_thread_count = unit["CONSUMER_THREAD_COUNT"].to_uint32();
        queue_item_capacity = unit["QUEUE_ITEM_CAPACITY"].to_uint32();
    } catch (const comcfg::ConfigException& exception) {
        WC_LOG_FATAL("exception:%s when load %s unit", exception.what(), unit_name);
        return -1;
    }

    return 0;
}

} // namespace wyr
