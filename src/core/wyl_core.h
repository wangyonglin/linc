/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   wyl_core.h
 * Author: Administrator
 *
 * Created on 2019年5月28日, 上午1:05
 */

#ifndef _WYL_CORE_H_INCLUDE_
#define _WYL_CORE_H_INCLUDE_

#define  WYL_OK          0
#define  WYL_ERROR      -1
#define  WYL_AGAIN      -2
#define  WYL_BUSY       -3
#define  WYL_DONE       -4
#define  WYL_DECLINED   -5
#define  WYL_ABORT      -6

#define WYL_APP_START 	0
#define WYL_APP_RELOAD 	1
#define WYL_APP_STOP 	-1
#define WYL_APP_DONE 	-2

#include<signal.h>
#include<sys/param.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/shm.h>
#include <stdarg.h>

#define ERROR_EXIT(m)\
do\
{\
    perror(m);\
    exit(EXIT_FAILURE);\
}\
while(0)
	char* replace(char* src, char* sub, char* dst);
	int __current_absolute_path(char * buf);
	int applocal(char* buf);
	int appinit(int argc, char* argv[]);
#endif /* _WYL_CORE_H_INCLUDE_ */

