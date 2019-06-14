#include <wyl_core.h>
#include <wyl_uart.h>
#include <wyl_log.h>

uart_handle uart_create(uart_name uart, int speed, int bits, char event, int stop) {
    assert(uart);
    int fd;
    if ((fd = open(uart, O_RDWR | O_NOCTTY | O_NDELAY)) < 0) {
        logger(log_error, "Open UART failed!");
        return -1;
    }
    /*清除串口非阻塞标志*/
    if (fcntl(fd, F_SETFL, 0) < 0) {
        logger(log_error, "fcntl failed!\n");
        return -1;
    }
    if (uart_configure(fd, speed, 0, bits, event, stop) < 0) {
        logger(log_error, "uart_configure failed!\n");
        return -1;
    }
    return fd;
};

int uart_configure(int fd, int speed, int flow, int bits, char event, int stop) {
    struct termios options;

    /*获取终端属性*/
    if (tcgetattr(fd, &options) < 0) {
        logger(log_error, "Cannot get standard input description");
        return -1;
    }


    /*设置输入输出波特率，两者保持一致*/
    switch (speed) {
        case 4800:
            cfsetispeed(&options, B4800);
            cfsetospeed(&options, B4800);
            break;
        case 9600:
            cfsetispeed(&options, B9600);
            cfsetospeed(&options, B9600);
            break;
        case 19200:
            cfsetispeed(&options, B19200);
            cfsetospeed(&options, B19200);
            break;
        case 38400:
            cfsetispeed(&options, B38400);
            cfsetospeed(&options, B38400);
            break;
        case 115200:
            cfsetispeed(&options, B115200);
            cfsetospeed(&options, B115200);
            break;
        default:
            logger(log_error, "Unkown baude!\n");
            return -1;
    }

    /*设置控制模式*/
    options.c_cflag |= CLOCAL; //保证程序不占用串口
    options.c_cflag |= CREAD; //保证程序可以从串口中读取数据

    /*设置数据流控制*/
    switch (flow) {
        case 0://不进行流控制
            options.c_cflag &= ~CRTSCTS;
            break;
        case 1://进行硬件流控制
            options.c_cflag |= CRTSCTS;
            break;
        case 2://进行软件流控制
            options.c_cflag |= IXON | IXOFF | IXANY;
            break;
        default:
            logger(log_error, "Unkown c_flow!\n");
            return -1;
    }

    /*设置数据位*/
    switch (bits) {
        case 5:
            options.c_cflag &= ~CSIZE; //屏蔽其它标志位
            options.c_cflag |= CS5;
            break;
        case 6:
            options.c_cflag &= ~CSIZE; //屏蔽其它标志位
            options.c_cflag |= CS6;
            break;
        case 7:
            options.c_cflag &= ~CSIZE; //屏蔽其它标志位
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag &= ~CSIZE; //屏蔽其它标志位
            options.c_cflag |= CS8;
            break;
        default:
            logger(log_error, "Unkown bits!\n");
            return -1;
    }

    /*设置校验位*/
    switch (event) {
            /*无奇偶校验位*/
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB; //PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~INPCK; //INPCK：使奇偶校验起作用
            break;
            /*设为空格,即停止位为2位*/
        case 's':
        case 'S':
            options.c_cflag &= ~PARENB; //PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~CSTOPB; //CSTOPB：使用两位停止位
            break;
            /*设置奇校验*/
        case 'o':
        case 'O':
            options.c_cflag |= PARENB; //PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag |= PARODD; //PARODD：若设置则为奇校验,否则为偶校验
            options.c_cflag |= INPCK; //INPCK：使奇偶校验起作用
            options.c_cflag |= ISTRIP; //ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
            break;
            /*设置偶校验*/
        case 'e':
        case 'E':
            options.c_cflag |= PARENB; //PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~PARODD; //PARODD：若设置则为奇校验,否则为偶校验
            options.c_cflag |= INPCK; //INPCK：使奇偶校验起作用
            options.c_cflag |= ISTRIP; //ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
            break;
        default:
            logger(log_error, "Unkown parity!\n");
            return -1;
    }

    /*设置停止位*/
    switch (stop) {
        case 1:
            options.c_cflag &= ~CSTOPB; //CSTOPB：使用两位停止位
            break;
        case 2:
            options.c_cflag |= CSTOPB; //CSTOPB：使用两位停止位
            break;
        default:
            logger(log_error, "Unkown stop!\n");
            return -1;
    }

    /*设置输出模式为原始输出*/
    options.c_oflag &= ~OPOST; //OPOST：若设置则按定义的输出处理，否则所有c_oflag失效

    /*设置本地模式为原始模式*/
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    /*
     *ICANON：允许规范模式进行输入处理
     *ECHO：允许输入字符的本地回显
     *ECHOE：在接收EPASE时执行Backspace,Space,Backspace组合
     *ISIG：允许信号
     */

    /*设置等待时间和最小接受字符*/
    options.c_cc[VTIME] = 0; //可以在select中设置
    options.c_cc[VMIN] = 1; //最少读取一个字符

    /*如果发生数据溢出，只接受数据，但是不进行读操作*/
    tcflush(fd, TCIFLUSH);

    /*激活配置*/
    if (tcsetattr(fd, TCSANOW, &options) < 0) {
        perror("tcsetattr failed");
        return -1;
    }

    return 0;
}

void * uart_routine(uart_handle uartfd, struct threadpool* pool, threadpool_work_t * work) {
    //epoll初始化
    //epoll描述符
    struct epoll_event events[MAX_EVENTS];
    int epollfd = epoll_create(MAX_EVENTS);
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = uartfd;

    //epoll_ctl设置属性,注册事件
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, uartfd, &event) < 0) {
        logger(log_error, "epoll 加入失败 fd:%d\n", uartfd);
        exit(EXIT_FAILURE);
    }
    int i;
    for (;;) {
        int ret = epoll_wait(epollfd, events, 1, 50);
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

            if (events[i].events & EPOLLIN) {
                work->fd.uartfd=events[i].data.fd;
    
                threadpool_add(pool, work->funcation, &work->fd);
            }
        }
    }
    return 0;
}
