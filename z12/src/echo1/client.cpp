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

#define MAX_BUF 1024

int main()
{
    char buffer[MAX_BUF];
    char request[MAX_BUF];
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(buffer, 0, MAX_BUF);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8081);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    
    scanf("%s", &request);

    send(fd , request , strlen(request), 0);
    int x = 1;

    while (x > 0)
    {
        x = recv(fd, buffer, MAX_BUF, 0);
    }
    std::cout << "text:" << buffer;
    close (fd);
}