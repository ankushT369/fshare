#include "fserver.h"
#include "fnet.h"
#include "ffile.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

fserver_ctx_t server_ctx;

void fshare_server_start(const char* filepath, int port) {
    fshare_sock_open(port, &server_ctx);
    server_ctx.e = fshare_file_get_ext(filepath);
    fshare_file_config(filepath, &server_ctx);
}

void fshare_server_run() {
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    /* Event Loop */
    while (1) {
        fd_t client_fd = accept(server_ctx.server,
                                (struct sockaddr *)&address,
                                &addrlen);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        printf("client connected\n");

        while (1) {
            meta m;
            int ret = fshare_recv_mdata(client_fd, &m);
            if (ret != 0) {
                // client closed or error
                break;
            }

            if (m.code == FSHARE_META) {
                meta resp = {
                    .code = FSHARE_META,
                    .status = 0,
                    .size = server_ctx.file_size,
                    .e = server_ctx.e,
                };
                fshare_send_mdata(client_fd, &resp);
            }
            else if (m.code == FSHARE_DATA) {
                fshare_file_send(client_fd, server_ctx);
                break; // done, send file once
            }
            else {
                break;
            }
        }

        fshare_close(client_fd);
        printf("client disconnected\n");
    }

}

void fshare_server_stop() {
    fshare_sock_close(server_ctx.server);
    fshare_file_close(server_ctx.file);
}

void fshare_close(fd_t fd) {
    close(fd);
}
