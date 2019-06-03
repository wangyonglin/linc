#include <wyl_core.h>
#include <wyl_log.h>


int logger(int tab,const char *format, ...) {
	char*buflocal=(char*)malloc(sizeof(char)*512);
	memset(buflocal,0,sizeof(buflocal));
	applocal(buflocal);
	strcat(buflocal,"log/access.log");
	FILE *fp;
	fp = fopen(buflocal, "a");
	if(fp==NULL){
		  fprintf(stderr, "lincd logger failed! %s\n",strerror(errno));	
		  return -1;
	}
	va_list arg;
	int done;

	va_start (arg, format);
	//done = vfprintf (stdout, format, arg);
	time_t time_log = time(NULL);
	struct tm* tm_log = localtime(&time_log);
	fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d ", tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec);
	switch(tab){
		case log_info:fprintf(fp," INFO: ");break;
		case log_error:fprintf(fp," ERROR: ");break;
		case log_debug:fprintf(fp," DEBUG: ");break;
		case log_warn:fprintf(fp," WARN: ");break;
		default:break;
	}
	done = vfprintf (fp, format, arg);
	fprintf(fp,"\n");
	va_end (arg);
	fflush(fp);
	free(buflocal);
	buflocal=NULL;
	return done;
}