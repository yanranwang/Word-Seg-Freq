#include <pthread.h>
#include <stdarg.h>
#include "wc_log.h"

namespace wyr{

bool g_enable_debug = false;

// 进程级日志初始化
bool wc_init_log(const char* log_conf_dir, const char* log_conf_name)
{
    if ((NULL == log_conf_dir) || (NULL == log_conf_name)) {
        fprintf(stderr, "[%s:%d][%s] Invalid arguments path=[%s] fname=[%d]\n",
                __FILE__, __LINE__, __FUNCTION__, log_conf_dir, log_conf_name);
        return false;
    }
    
    if (0 != com_loadlog(log_conf_dir, log_conf_name)) {
        fprintf(stderr, "[%s:%d][%s] com_loadlog(%s, %s) failed\n",
                __FILE__, __LINE__, __FUNCTION__, log_conf_dir, log_conf_name);
        return false;
    }
    
    if (true == com_getcategory()->logstat()->unAccept(COMLOG_DEBUG)) {
        g_enable_debug = false;
    } else {
        g_enable_debug = true;
    }

    return true;
}

// 线程级日志初始化
bool wc_initthread_log()
{
    if (0 != com_openlog_r()) {
        fprintf(stderr, "Fail to com_openlog_r");
        return false;
    }
    return true;
}

void wc_closethread_log()
{
    if (0 != com_closelog_r()) {
        WC_LOG_FATAL("Fail to com_closelog_r");
    }
}

void wc_close_log()
{
    if (0 != com_closelog()) {
        WC_LOG_FATAL("Fail to com_closelog");
    }
}

} // namespace wyr
