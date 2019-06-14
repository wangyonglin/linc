
#include <wyl_core.h>
#include <wyl_socket.h>
#include <wyl_log.h>

socket_handle socket_create(socket_host host, socket_backlog backlog) {
    struct sockaddr_in sockaddr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        logger(log_error, "socket");
        exit(EXIT_FAILURE);
    }
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(host);
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int opt = SO_REUSEADDR;
    if ((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt))) < 0) {
        logger(log_error, "setsockopt failed");
        exit(EXIT_FAILURE);
    }
    /*设置socket为非阻塞模式*/
    socket_accept_nonblocking(sockfd);

    /*bind a socket or rename a sockt*/
    if (bind(sockfd, (struct sockaddr*) &sockaddr, sizeof (sockaddr)) == -1) {
        logger(log_error, "Socket bind  failed! -> error: %s !\n", strerror(errno));
        exit(EXIT_FAILURE);

    }

    if (listen(sockfd, backlog) == -1) {
        logger(log_error, "Socket listen  failed! -> error: %s !\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void * socket_routine(socket_handle sockfd,struct threadpool* pool,threadpool_work_t * work) {
    //epoll初始化
    //epoll描述符
    struct epoll_event events[MAX_EVENTS];
    int epollfd = epoll_create(MAX_EVENTS);
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = sockfd; //把server socket fd封装进events里面

    //epoll_ctl设置属性,注册事件
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD,sockfd, &event) < 0) {
        logger(log_error, "epoll 加入失败 fd:%d\n",sockfd);
        exit(EXIT_FAILURE);
    }
    int i;
    for (;;) {
        //int timeout = 300; //设置超时;在select中使用的是timeval结构体
        //epoll_wait epoll处理
        //ret会返回在规定的时间内获取到IO数据的个数，并把获取到的event保存在eventList中，注意在每次执行该函数时eventList都会清空，由epoll_wait函数填写。
        //而不清除已经EPOLL_CTL_ADD到epollfd描述符的其他加入的文件描述符。这一点与select不同，select每次都要进行FD_SET，具体可看我的select讲解。
        //epoll里面的文件描述符要手动通过EPOLL_CTL_DEL进行删除。
        int ret = epoll_wait(epollfd, events, MAX_EVENTS,300);

        if (ret < 0) {
            logger(log_error, "epoll error");
            break;
        } else if (ret == 0) {
            //超时
            continue;
        }

        //直接获取了事件数量，给出了活动的流，这里就是跟selec，poll区别的关键 //select要用遍历整个数组才知道是那个文件描述符有事件。而epoll直接就把有事件的文件描述符按顺序保存在eventList中
        for (i = 0; i < ret; i++) {
            //错误输出
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || !(events[i].events & EPOLLIN)) {
                logger(log_error, "epoll error : EPOLLERR|EPOLLHUP|EPOLLIN \n");
                close(events[i].data.fd);
               // exit(EXIT_FAILURE);
            }

            if (events[i].data.fd == sockfd) {
                //这个是判断sockfd的，主要是用于接收客户端的连接accept
                socket_accept_listener(sockfd,epollfd);
            } else {
                if(events[i].events & EPOLLIN){
                   
                   work->fd.sockfd=events[i].data.fd;


                    threadpool_add(pool,work->funcation,&work->fd);
                }             
            }
        }
    }
    return 0;
}

socket_handle socket_accept_listener(int sockfd, int epollfd) {
    struct sockaddr_in sin;
    socklen_t len = sizeof (struct sockaddr_in);
    bzero(&sin, len);
    int confd = accept(sockfd, (struct sockaddr *) &sin, &len);
    if (confd < 0) {
        logger(log_error, "connect error \n");
        exit(EXIT_FAILURE);
    }
    socket_accept_nonblocking(confd);
    //把客户端新建立的连接添加到EPOLL的监听中
    struct epoll_event event;
    event.data.fd = confd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, confd, &event);
    char * str= "welcome to Linc!";
    send(confd,str,strlen(str),0);
    return confd;
}

int socket_accept_nonblocking(socket_handle sockfd) {
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1) {
        return -1;
    }
    return 0;
}

void * pthread_handle_message(void * arg) {
    char recvBuf[1024] = {0};
    int ret = 999;
    int rs = 1;
    int sockfd=*(int*)arg;
    while (rs) {
        ret = recv(sockfd, recvBuf, 1024, 0); // 接受客户端消息
        if (ret < 0) {
            //由于是非阻塞的模式,所以当errno为EAGAIN时,表示当前缓冲区已无数据可//读在这里就当作是该次事件已处理过。
            if (errno == EAGAIN) {
                printf("EAGAIN\n");
                break;
            } else {
                printf("recv error! errno: %s\n", strerror(errno));
                close(sockfd);
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
        if (socket_tcp_established(sockfd)) {
            send(sockfd, recvBuf, strlen(recvBuf), 0);
        } else {
            printf("已连断开连接！\n");
            char * buf = "已连断开连接";
            send(sockfd, buf, strlen(buf), 0);
        }
      
    }
  
}



int socket_tcp_established(socket_handle sockfd) {
    struct tcp_info info;
    int len = sizeof (info);
    getsockopt(sockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *) & len);
    if ((info.tcpi_state == TCP_ESTABLISHED))
        return 1;
    else
        return 0;
}
