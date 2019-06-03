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
/*
 * 
 */
int main(int argc, char** argv) {
	signal(SIGPIPE,SIG_IGN);
	
   int ps = appinit(argc,argv);
	if(ps==WYL_APP_START){
		 logger(log_info,"lincd start");
		 daemonize();
	}
	if(ps==WYL_APP_STOP){
		logger(log_info,"lincd  stop");
		 suicide("linc");
		 exit(EXIT_SUCCESS);
	}
	if(ps==WYL_APP_DONE){
		fprintf(stderr,"lincd rum done\n");
		exit(EXIT_SUCCESS);
	}
	char*buflocal=(char*)malloc(sizeof(char)*512);
	memset(buflocal,0,sizeof(buflocal));
	applocal(buflocal);
    int sockfd, uartfd;
    int ret;
    wyl_configure_s cct;
    memset(&cct, 0, sizeof (wyl_configure_s));
    /*读取配置文件增幅* configure_container_t*/
	strcat(buflocal,"conf/linc.conf");	
    if (wyl_configure_init(buflocal, &cct) == -1) {
		logger(log_debug,"wyl_configure_init failed");
        exit(EXIT_FAILURE);
    }

    
    sockfd = wyl_socket_open(cct.socket.ip, cct.socket.port, cct.socket.count);
    if (sockfd == -1) {
		logger(log_error,"wyl_configure_init failed");
        exit(EXIT_FAILURE);
    }

    uartfd = wyl_uart_open(uartfd, cct.serial.com);
    if (uartfd == -1) {
        logger(log_error, "Open uart failed!\n");
        exit(EXIT_FAILURE);
    }

    ret = wyl_uart_configure(uartfd, cct.serial.speed, cct.serial.flow, cct.serial.bits, cct.serial.event, cct.serial.stop);
    if (ret == -1) {
        logger(log_error, "Configure Serial failed!\n");
        exit(EXIT_FAILURE);
    }

    wyl_socket_block handle;
    memset(&handle, 0, sizeof (wyl_socket_block));
    handle.arg=&uartfd;
    wyl_socket_router(sockfd,&handle);
    
    return EXIT_SUCCESS;
}

