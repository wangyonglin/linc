#include <wyl_core.h>

int __current_absolute_path(char * buf) {
	memset(buf,0,sizeof(buf));
    int count = readlink("/proc/self/exe", buf, 1024);
    if (count < 0 || count >= 1024) {
        return ( EXIT_FAILURE);
    }
    int i;
	for (i = count; i >=0; --i)
	{
		if (buf[i] == '/')
		{
			buf[i+1] = '\0';
			break;
		}
	}
    return (EXIT_SUCCESS);
}

char* replace(char* src, char* sub, char* dst) {
    int i = 0;
    int len;
    char* temp1 = NULL;
    char* temp2 = NULL;
    char* SRC = NULL;
    SRC = (char*) malloc(sizeof (src)); //--->为什么要用sizeof src是指针类型 sizeof为 4 
    strcpy(SRC, src);
    len = strlen(src);
    for (; i <= len; i++)//为什么要单个字符循环、sub 与 dst  长度一定一致么？ 最终的字符串一定和src长度一致？
    {

        if (!strncmp(src, sub, strlen(sub))) {

            temp1 = (char*) malloc(i + 1); //这里分配的内存 何时释放 怎么释放
            strncpy(temp1, SRC, i);
            temp1[i] = '\0';
            strcat(temp1, dst);
            src = src + strlen(sub);
            strcat(temp1, src);
            return temp1;
        }

        src++;
    }
      fprintf(stderr, "replace not found!\n");
    return src; //src做了（src++）返回时 指向的是哪里？
}
int applocal(char* buf){
	char * strcur=(char*)malloc(sizeof(char)*512);
	memset(strcur,0,sizeof(strcur));
	__current_absolute_path(strcur);
	strcpy(buf,replace(strcur,"/bin",""));
	free(strcur);
	strcur=NULL;
	return 0;
}

int appinit(int argc, char* argv[]){
	   int ch;
    opterr = 0;
    while ((ch = getopt(argc, argv, "s:")) != -1) {
        switch (ch) {
            case 's':
                if (strcmp(optarg, "start") == 0) {
                    return WYL_APP_START;
                }
                if (strcmp(optarg, "reload") == 0) {
                    return WYL_APP_RELOAD;
                }
                if (strcmp(optarg, "stop") == 0) {
                    return WYL_APP_STOP;
                }
                break;
            default:
                break;
        }
    }
    return WYL_APP_DONE;
	
	
}