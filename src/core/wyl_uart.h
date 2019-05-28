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
int wyl_uart_open(int fd,const char* uart);
int wyl_uart_configure(int fd, int speed, int flow, int bits, char event, int stop);
int wyl_uart_destory(int fd) ;



#ifdef __cplusplus
}
#endif

#endif /* _UART_H_INCLUDE_ */

