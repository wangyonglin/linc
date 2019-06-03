/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   wyl_configure.h
 * Author: Administrator
 *
 * Created on 2019年5月28日, 下午12:20
 */

#ifndef _WYL_CONFIGURE_H_INCLUDE_
#define _WYL_CONFIGURE_H_INCLUDE_

    struct wyl_socket_s {
        int port;
        char ip[32];
        int count;
    };
    struct wyl_uart_s {
        char com[32];
        int speed;
        int flow;
        int bits;
        char event;
        int stop;
    };
    
    typedef struct {
        struct wyl_socket_s socket;
        struct wyl_uart_s serial;
    } wyl_configure_s;

	int apps(int argc, char* argv[]);
    int wyl_configure_init(char *profile, wyl_configure_s *cc);

#endif /* _WYL_CONFIGURE_H_INCLUDE_ */

