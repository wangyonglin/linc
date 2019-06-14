/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   uart.h
 * Author: Administrator
 *
 * Created on 2019年5月27日, 下午10:51
 */

#ifndef _UART_H_INCLUDE_
#define _UART_H_INCLUDE_

#ifdef __cplusplus
extern "C" {
#endif
    #define MAX_EVENTS 500

    typedef int uart_handle;
    typedef const char* uart_name;
    typedef int epoll_handle;
uart_handle uart_create(uart_name uart, int speed, int bits, char event, int stop);
int uart_configure(int fd, int speed, int flow, int bits, char event, int stop);
void * uart_routine(uart_handle uartfd, struct threadpool* pool,threadpool_work_t * work);
epoll_handle uart_epoll(uart_handle handle);

#ifdef __cplusplus
}
#endif

#endif /* _UART_H_INCLUDE_ */

