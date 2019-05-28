
#include <wyl_core.h>
#include <wyl_daemon.h>

int wyl_daemon()
{
	int pid;
	int i;
	pid=fork();
	if(pid<0)    
    		return WYL_ERROR;
	else if(pid>0)
    		return WYL_ERROR;
    
	setsid();
	pid=fork();
	if(pid>0)
    	return WYL_ERROR;
	else if(pid<0)    
    	return WYL_ERROR;
	
	for(i=0;i<NOFILE;i++)
    	close(i);
   	chdir("/root"); 
    	umask(0);

    return WYL_OK;
}
