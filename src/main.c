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
/*
 * 
 */
int main(int argc, char** argv) {
   
    wyl_daemon();
    int sockfd, uartfd;
    int ret;
    wyl_configure_s cct;
    memset(&cct, 0, sizeof (wyl_configure_s));
    /*读取配置文件增幅* configure_container_t*/
    if (wyl_configure_init("./conf/linc.conf", &cct) == -1) {
        exit(EXIT_FAILURE);
    }

    printf("%d \n", cct.socket.port);
    sockfd = wyl_socket_open(cct.socket.ip, cct.socket.port, cct.socket.count);
    if (sockfd == -1) {
        fprintf(stderr, "Socket initialize failed!\n");
        exit(EXIT_FAILURE);
    }
    printf("ip:%s  port:%d con:%d \n", cct.socket.ip, cct.socket.port, cct.socket.count);

    uartfd = wyl_uart_open(uartfd, cct.serial.com);
    if (uartfd == -1) {
        fprintf(stderr, "Open uart failed!\n");
        exit(EXIT_FAILURE);
    }
    printf("main uartfd:%d \n", uartfd);
    ret = wyl_uart_configure(uartfd, cct.serial.speed, cct.serial.flow, cct.serial.bits, cct.serial.event, cct.serial.stop);
    if (ret == -1) {
        fprintf(stderr, "Configure Serial failed!\n");
        exit(EXIT_FAILURE);
    }

    wyl_socket_block handle;
    memset(&handle, 0, sizeof (wyl_socket_block));
    handle.arg = &uartfd;
    wyl_socket_router(sockfd,&handle);
    
    return EXIT_SUCCESS;
}

