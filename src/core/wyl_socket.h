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
#include <threadpool.h>
#define CON_QUEUE 20
#define MAX_DATA_SIZE 4096
#define MAX_EVENTS 500
    typedef int socket_handle;
    typedef int socket_host;
    typedef int socket_backlog;
    typedef int epoll_event_handle;
    typedef int epoll_timeout;
    typedef int uart_handle;
    typedef int epoll_handle;
    typedef int socket_int;
    typedef unsigned int epoll_events_handle;

    int setnonblocking(socket_handle sockfd);
    void * pthread_handle_message(void * arg);
    socket_handle socket_create(socket_host host, socket_backlog backlog);
void * socket_routine(socket_handle sockfd,struct threadpool* pool,threadpool_work_t* work);
#endif /* _WYL_SOCKET_H_INCLUDED_ */

