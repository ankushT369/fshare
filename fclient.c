#include "fclient.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 8192

fd_t fshare_connect_server(const char* ip, int port)
{
    fd_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        return -1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port   = htons(port);

    if (inet_pton(AF_INET, ip, &server.sin_addr) != 1) {
        perror("inet_pton failed");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect failed");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

void fshare_client_recv(fd_t sockfd)
{
    char buf[BUF_SIZE];

    while (1) {
        ssize_t n = read(sockfd, buf, sizeof(buf));

        if (n < 0) {
            if (errno == EINTR)
                continue;

            perror("read failed");
            break;
        }

        if (n == 0) {
            // EOF → server closed connection
            break;
        }

        ssize_t off = 0;
        while (off < n) {
            ssize_t w = write(STDOUT_FILENO, buf + off, n - off);
            if (w < 0) {
                perror("write failed");
                return;
            }
            off += w;
        }
    }
}
