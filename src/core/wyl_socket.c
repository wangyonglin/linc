
#include <wyl_core.h>
#include <wyl_socket.h>
int w_f(int sockfd, int uartfd, char *wbuf) {
    memset(wbuf, 0, sizeof (wbuf));
    int ret = read(uartfd, wbuf, sizeof (wbuf));
    if (ret <= 0) {
        fprintf(stderr, "uart read failed! error: %s \n", strerror(errno));
        return -1;
    } else {

        ret = send(sockfd, wbuf, strlen(wbuf), 0);
        if (ret == -1) {
            fprintf(stderr, "socket send failed! error: %s \n", strerror(errno));
            return -1;
        }
    }
    return 0;
}

int r_f(int sockfd, int uartfd, char *rbuf) {
    memset(rbuf, 0, sizeof (rbuf));
    int ret = recv(sockfd, rbuf, sizeof (rbuf), 0);
    if (ret <= 0) {
        fprintf(stderr, "socket recv failed! error: %s \n", strerror(errno));
        return -1;
    } else {
        ret = write(uartfd, rbuf, strlen(rbuf));
        if (ret == -1) {
            fprintf(stderr, "Uart write failed! error: %s \n", strerror(errno));
            return -1;
        }
    }
    return 0;
}

void * funcation(void * arg) {
    wyl_socket_block * handle = (wyl_socket_block*) arg;
    int sockfd = handle->sockfd;
    int uartfd = *(int*) &handle->arg;
    printf("sockfd %d|uartfd %d", sockfd, uartfd);
    printf("You got a connection from %s \n", inet_ntoa(handle->addr));
    char * rbuf = (char*) malloc(sizeof (char)*64);
    char * wbuf = (char*) malloc(sizeof (char)*64);
    while (1) {
        if (r_f(sockfd, uartfd, rbuf) == -1) {
            break;
        }
        if (w_f(sockfd, uartfd, rbuf) == -1) {
            break;
        }

    }
    free(rbuf);
    free(wbuf);
    pthread_exit(NULL);
    return NULL;

}
int wyl_socket_open(const char *ip, int host, int backlog) {

    char buf[100];
    memset(buf, 0, 100);
    struct sockaddr_in sockaddr;
    int on;
    //memset(&sockaddr,0,sizeof(sockaddr));
    /*create a socket.type is AF_INET,sock_stream*/
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        fprintf(stderr, "Socket create  failed -> error: %s !\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(host);
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (int));
    /*bind a socket or rename a sockt*/
    if (bind(sockfd, (struct sockaddr*) &sockaddr, sizeof (sockaddr)) == -1) {
        fprintf(stderr, "Socket bind  failed! -> error: %s !\n", strerror(errno));
        exit(EXIT_FAILURE);

    }

    if (listen(sockfd, backlog) == -1) {
        fprintf(stderr, "Socket listen  failed! -> error: %s !\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

int wyl_socket_router(int fockfd, wyl_socket_block *handle) {
    pthread_t tid;
    struct sockaddr_in sockaddr;
    int sockfd;
    while (1) {
        socklen_t len = sizeof (sockaddr);
        if ((sockfd = accept(fockfd, (struct sockaddr*) &sockaddr, &len)) == -1) {
            fprintf(stderr, "Socket connect failed! -> error: %s !\n", strerror(errno));
            exit(EXIT_FAILURE);
        }else{
        handle->sockfd=sockfd;
        handle->addr = sockaddr.sin_addr;
        printf("You got a connection from %s \n", inet_ntoa(handle->addr));
        int  ret = pthread_create(&tid, NULL,funcation,handle);
        if(ret!=0){
           fprintf(stderr, "Socket accept  create pthread failed! !\n");
           return -1;
        }
        } 
    }
    return 0;
}

int wyl_socket_destory(int sockfd) {
    assert(sockfd);
    close(sockfd);
    return 0;
}

