#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>
#include <vector>
#include <poll.h>

#define MAX_EVENTS 10
#define MAX_BUF 1024
int main()
{
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    struct sockaddr_in serv_addr;
    int portno = 8081;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    int yes = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("setsockopt");
        exit(1);
    }

    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("bind error");
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, 10) == -1)
    {
        perror("listen error");
        close(server_fd);
        exit(1);
    }

    std::cout << "pref end\n";

    struct epoll_event ev, events[MAX_EVENTS];
    int conn_sock, nfds, epollfd;
    
    epollfd = epoll_create1(0);
    if (epollfd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &ev) == -1)
    {
        perror("epoll_ctl: server_fd");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof addr;
    for (;;)
    {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        std::cout << nfds << std::endl;
        for (int n = 0; n < nfds; ++n)
        {
            if (events[n].data.fd == server_fd)
            {

                std::cout << "if\n";

                conn_sock = accept(server_fd,
                                   (struct sockaddr *)&addr, &addrlen);
                if (conn_sock == -1)
                {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                //accept4(conn_sock, 0, 0, SOCK_NONBLOCK);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_sock;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
                {
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                char buf[MAX_BUF];
                send(conn_sock, buf, recv(conn_sock, buf, MAX_BUF, 0), 0);
                close(conn_sock);
            }
        }
    }
}