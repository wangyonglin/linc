
#include <wyl_core.h>
#include <wyl_daemon.h>
#include <wyl_log.h>
pid_t daemonize(){
	pid_t pid;
		int i;
		pid=fork();
		if(pid<0)
			ERROR_EXIT("daemon fork failed!");
		else if(pid>0) //父进程退出
			exit(0);
			
		setsid(); //使子进程成为组长
		pid=fork();
		if(pid>0)
			exit(0); //再次退出，使进程不是组长，这样进程就不会打开控制终端
		else if(pid<0)
			ERROR_EXIT("daemon fork 2 failed!");

		//关闭进程打开的文件句柄
		for(i=0;i<NOFILE;i++)
			close(i);
		chdir("/root/test");  //改变目录
		umask(0);//重设文件创建的掩码

    return pid;
}


int suicide(const char * daemon_name) {
    pid_t childpid;
    // char command[] = "ps aux | grep 'opt' | grep -v grep| awk '{print $2}'";
    char * command = (char*) malloc(sizeof (char)*512);
    memset(command, 0, sizeof (command));
    sprintf(command,"ps aux | grep '%s' | grep -v grep| awk '{print $2}'", daemon_name);
    FILE *fp = popen(command,"r");
    if (NULL == fp){
        logger(log_error, "opt popen failed!\n");
        return -1;
    }
    char *buf = (char*) malloc(sizeof (char)*64);
    memset(buf, 0, sizeof (buf));
    while (fgets(buf, sizeof (buf), fp) != NULL) {
        childpid = atoi(buf);
        int retval = kill(childpid, SIGKILL);
        if (retval) {
            logger(log_error, "kill failed!\n");
        }
    }
    pclose(fp);
    free(buf);
    return 0;
}