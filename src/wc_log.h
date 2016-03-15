#ifndef WYR_WC_LOG_H
#define WYR_WC_LOG_H

#include <com_log.h>

namespace wyr {

// 是否开启debug日志
extern bool g_enable_debug;

// @brief 初始化log资源
// @param log_conf_dir: 配置文件路径
// @param log_conf_name: 配置文件名
bool wc_init_log(const char* log_conf_dir, const char* log_conf_name);

// 初始化线程级log资源
// 每个线程都必须调用
bool wc_initthread_log();

// 关闭线程级log资源
void wc_closethread_log();

// 关闭进程级log资源
void wc_close_log();

} // namespace wyr

#define WC_WRITE_LOG(_loglevel_, _fmt_, args...)               \
    do {                                                        \
        com_writelog((_loglevel_),                              \
            "[%s:%d][%s]" _fmt_,                       \
            __FILE__, __LINE__, __FUNCTION__, ##args); \
    } while (0)


#define WC_LOG_FATAL(fmt, arg...)                      \
    do {                                                \
        WC_WRITE_LOG(COMLOG_FATAL, fmt, ##arg);        \
    } while (0)

#define WC_LOG_WARNING(fmt, arg...)                    \
    do {                                                \
        WC_WRITE_LOG(COMLOG_WARNING, fmt, ##arg);      \
    } while (0)

#define WC_LOG_NOTICE(fmt, arg...)                     \
    do {                                                \
        WC_WRITE_LOG(COMLOG_NOTICE, fmt, ##arg);       \
    } while (0)

#define WC_LOG_TRACE(fmt, arg...)                      \
    do {                                                \
        WC_WRITE_LOG(COMLOG_TRACE, fmt, ##arg);        \
    } while (0)

#define WC_LOG_DEBUG(fmt, arg...)                      \
    do {                                                \
        if (g_enable_debug) {                       \
            WC_WRITE_LOG(COMLOG_DEBUG, fmt, ##arg);    \
        }                                               \
    } while (0)
#endif
