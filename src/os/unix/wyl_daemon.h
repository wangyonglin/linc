/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   wyl_daemon.h
 * Author: Administrator
 *
 * Created on 2019年5月28日, 上午1:21
 */

#ifndef _WYL_DAEMON_H_INCLUDE_
#define _WYL_DAEMON_H_INCLUDE_


	
pid_t daemonize();
int suicide(const char * daemon_name);
#endif /* _WYL_DAEMON_H_INCLUDE_ */

