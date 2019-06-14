/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   linc.c
 * Author: Administrator
 *
 * Created on 2019年5月27日, 下午5:40
 */
#include <wyl_core.h>
#include <wyl_config.h>
#include <wyl_socket.h>
#include <wyl_configure.h>
#include <wyl_uart.h>
#include <wyl_daemon.h>
#include <wyl_log.h>
#include <threadpool.h>
int localhandle=0;
void * sockwork(void * arg) {
    char recvBuf[1024] = {0};
    int ret = 999;
    int rs = 1;
    fd_t *fd=(fd_t*)arg;
    localhandle=fd->sockfd;

    while (rs) {
        ret = recv(fd->sockfd, recvBuf, 1024, 0); // 接受客户端消息
        if (ret < 0) {
            //由于是非阻塞的模式,所以当errno为EAGAIN时,表示当前缓冲区已无数据可//读在这里就当作是该次事件已处理过。
            if (errno == EAGAIN) {
                printf("EAGAIN\n");
                logger(log_error,"sockwork recv EAGAIN \n");
                break;
            } else {
                logger(log_error,"sockwork recv error! errno: %s\n", strerror(errno));
                close(fd->sockfd);
                break;
            }
        } else if (ret == 0) {
            // 这里表示对端的socket已正常关闭. 
            rs = 0;
        }
        if (ret == sizeof (recvBuf))
            rs = 1; // 需要再次读取
        else
            rs = 0;
    }

    if (ret > 0) {

        if (socket_tcp_established(fd->sockfd)) {
            int ret = write(fd->uartfd, recvBuf, strlen(recvBuf));
            if(ret==-1){
                logger(log_error, "sockwork write %s\n", strerror(errno));
                
            }
        } else {
         
            logger(log_error, "sockwork socket_tcp_established not conection \n");
        }

    }


}

void * uartwork(void * arg) {
    char recvBuf[1024] = {0};
    int ret = 999;
    int rs = 1;
    fd_t *fd=(fd_t*)arg;
    fd->sockfd=localhandle;
   
    while (rs) {
        ret = read(fd->uartfd, recvBuf, 1024); 
        if (ret ==-1) {
              logger(log_error,"uartwork read error! errno: %s\n", strerror(errno));
            close(fd->sockfd);
            break;

        } else if (ret == 0) {
            // 这里表示对端的socket已正常关闭. 
            rs = 0;
        }
        if (ret == sizeof (recvBuf))
            rs = 1; // 需要再次读取
        else
            rs = 0;
    }

    if (ret > 0) {
        if(send(fd->sockfd,recvBuf,strlen(recvBuf),0)<0){
             logger(log_info,"uartwork send : %s！\n", recvBuf);
        }
    }

}

void *sock_application(void *arg) {
    threadpool_application_t *app = (threadpool_application_t *)arg;
    threadpool_work_t work;
    work.funcation = sockwork;
    work.fd=*(fd_t*)app->arg;
    socket_routine(work.fd.sockfd, app->pool,&work);
    return;
}

void *uart_application(void * arg) {
    threadpool_application_t *app = (threadpool_application_t *)arg;


    threadpool_work_t work;
    work.funcation = uartwork;
     work.fd=*(fd_t*) app->arg;
    uart_routine(work.fd.uartfd, app->pool,&work);
    return;
}

int main(int argc, char** argv) {
    signal(SIGPIPE,SIG_IGN);
    
   int ps = appinit(argc,argv);
    if(ps==WYL_APP_START){
         daemonize();
    }
    if(ps==WYL_APP_STOP){
         suicide("linc");
         exit(EXIT_SUCCESS);
    }
    if(ps==WYL_APP_DONE){
        logger(log_error,"lincd rum done\n");
        exit(EXIT_SUCCESS);
    }
    char*buflocal=(char*)malloc(sizeof(char)*512);
    memset(buflocal,0,sizeof(buflocal));
    applocal(buflocal);
   
    int ret;
    wyl_configure_s cct;
    memset(&cct, 0, sizeof (wyl_configure_s));
    /*读取配置文件增幅* configure_container_t*/
    strcat(buflocal,"conf/linc.conf");  
    if (wyl_configure_init(buflocal, &cct) == -1) {
        logger(log_debug,"wyl_configure_init failed");
        exit(EXIT_FAILURE);
    }

    struct threadpool* pool = threadpool_create(10, 20);
    

    uart_handle uartfd;
    uartfd = uart_create(cct.serial.com,cct.serial.speed, cct.serial.bits,  cct.serial.event, cct.serial.stop);
    if (uartfd == -1) {
        logger(log_error, "uart_create failure");
        exit(EXIT_FAILURE);
    }
    printf("uart_create %d\n", uartfd);
    socket_handle sockfd;
    sockfd = socket_create(7132,10);
    threadpool_application_t app;
    fd_t fd;

    app.pool=pool;
  
    fd.uartfd=uartfd;
     app.arg=&fd;
    threadpool_add(pool, uart_application, &app);


  
    fd.sockfd=sockfd;
    app.arg=&fd;
    threadpool_add(pool,sock_application, &app);

    while (1);
    close(sockfd);
    return EXIT_SUCCESS;
}

