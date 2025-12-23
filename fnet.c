#include "fnet.h"
#include "fserver.h"

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>


void fshare_sock_open(int port, fserver_ctx_t* ctx) {
    struct sockaddr_in address;

    ctx->server = socket(AF_INET, SOCK_STREAM, 0);
    if (ctx->server < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    fshare_set_sock_option(ctx);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(ctx->server,
             (struct sockaddr *)&address,
             sizeof(address)) < 0) {
        perror("bind failed");
        fshare_sock_close(ctx->server);
        exit(EXIT_FAILURE);
    }

    if (listen(ctx->server, SOMAXCONN) < 0) {
        perror("listen failed");
        fshare_sock_close(ctx->server);
        exit(EXIT_FAILURE);
    }
}

void fshare_set_sock_option(fserver_ctx_t* ctx) {
    int opt = 1;

    if (setsockopt(ctx->server, SOL_SOCKET,
                   SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(ctx->server, SOL_SOCKET,
                   SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEPORT failed");
        exit(EXIT_FAILURE);
    }
}

void fshare_sock_close(fd_t server) {
    fshare_close(server);
}

int fshare_recv_mdata(fd_t sock, meta* m) {
    size_t total = 0;
    size_t need = sizeof(*m);
    char *buf = (char *)m;

    while (total < need) {
        ssize_t n = recv(sock, buf + total, need - total, 0);

        if (n < 0) {
            if (errno == EINTR)
                continue;   // interrupted, retry

            perror("recv");
            return -1;
        }

        if (n == 0) {
            // peer closed connection before full struct arrived
            fprintf(stderr, "connection closed while receiving metadata\n");
            return -1;
        }

        total += n;
    }

    return 0;
}

int fshare_send_mdata(fd_t sock, const meta* m) {
    size_t total = 0;
    size_t need = sizeof(*m);
    const char *buf = (const char *)m;

    while (total < need) {
        ssize_t n = send(sock, buf + total, need - total, 0);

        if (n < 0) {
            if (errno == EINTR)
                continue;   // interrupted, retry

            perror("send");
            return -1;
        }

        if (n == 0) {
            // socket closed or unable to send more
            fprintf(stderr, "connection closed while sending metadata\n");
            return -1;
        }

        total += n;
    }

    return 0;
}
