/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   wyl_socket.h
 * Author: Administrator
 *
 * Created on 2019年5月28日, 上午11:32
 */

#ifndef _WYL_SOCKET_H_INCLUDED_
#define _WYL_SOCKET_H_INCLUDED_
#include <arpa/inet.h> 



    typedef struct {
        int sockfd;
        void *arg;
        struct in_addr addr;
    } wyl_socket_block;
    
    int w_f(int sockfd, int uartfd, char *wbuf);
    int r_f(int sockfd, int uartfd, char *rbuf);
    void * funcation(void * arg);
    int wyl_socket_open(const char *ip, int host, int backlog);
    int wyl_socket_router(int fockfd, wyl_socket_block *handle);
    int wyl_socket_destory(int sockfd);
#endif /* _WYL_SOCKET_H_INCLUDED_ */

