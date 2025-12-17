#include "fserver.h"

#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

/* Global variable */
static fdesc fd = {
    .server = -1,
    .epoll = -1,
    .file = -1,
};


void fshare_server_start(const char* filepath, int port) {
    fshare_sock_create(port);
    fshare_config_file(filepath);
}

void fshare_server_run() {
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    while (1) {
        fd_t client_fd = accept(fd.server,
                                (struct sockaddr *)&address,
                                &addrlen);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        fshare_sendfile(client_fd);
        fshare_close(client_fd);
    }
}

void fshare_server_stop() {
    fshare_sock_destroy();
    fshare_closefile();
}

void fshare_sock_create(int port) {
    struct sockaddr_in address;

    fd_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    fd.server = sockfd;

    fshare_set_sock_option();

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(fd.server,
             (struct sockaddr *)&address,
             sizeof(address)) < 0) {
        perror("bind failed");
        fshare_sock_destroy();
        exit(EXIT_FAILURE);
    }

    if (listen(fd.server, SOMAXCONN) < 0) {
        perror("listen failed");
        fshare_sock_destroy();
        exit(EXIT_FAILURE);
    }
}

void fshare_set_sock_option() {
    int opt = 1;

    if (setsockopt(fd.server, SOL_SOCKET,
                   SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(fd.server, SOL_SOCKET,
                   SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEPORT failed");
        exit(EXIT_FAILURE);
    }
}

void fshare_sock_destroy() {
    fshare_close(fd.server);
}

void fshare_config_file(const char* filepath) {
    fshare_openfile(filepath);
}

void fshare_openfile(const char* filepath) {
    int filefd = open(filepath, O_RDONLY);
    if (filefd < 0) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    fd.file = filefd;
}

void fshare_closefile() {
    fshare_close(fd.file);
}

void fshare_sendfile(fd_t client_fd) {
    off_t offset = 0;
    struct stat st;

    if (fstat(fd.file, &st) < 0) {
        perror("fstat failed");
        return;
    }

    ssize_t remaining = st.st_size;

    while (remaining > 0) {
        ssize_t sent = sendfile(client_fd, fd.file,
                                &offset, remaining);
        if (sent < 0) {
            if (errno == EINTR)
                continue;
            perror("sendfile failed");
            break;
        }
        if (sent == 0)
            break;

        remaining -= sent;
    }
}

void fshare_close(fd_t fd) {
    close(fd);
}

