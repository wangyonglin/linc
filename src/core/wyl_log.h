#ifndef _WYL_LOG_INCLUDE_
#define _WYL_LOG_INCLUDE_
#define log_error 1
#define log_debug 2
#define log_info 3	
#define log_warn 4

	int logger(int tab,const char *format, ...);
#endif /* _WYL_LOG_INCLUDE_ */