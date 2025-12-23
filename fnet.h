#ifndef FNET_H
#define FNET_H

#include "ftype.h"
#include "fserver.h"
#include "fext.h"

#include <stdint.h>

typedef enum resp_code {
    FSHARE_OK,
    FSHARE_META,
    FSHARE_DATA,
    FSHARE_ERR,
} resp_code;

typedef struct meta {
    resp_code code;

    uint8_t status;
    size_t size;   // File Size 
    ext e;
} meta;

/* Socket API */
void fshare_sock_open(int port, fserver_ctx_t* ctx);
// void fshare_sock_bind();
// void fshare_sock_listen();
// void fshare_sock_accept();
void fshare_set_sock_option(fserver_ctx_t* ctx);
void fshare_sock_close(fd_t server);

/* Network Operation API */
void fshare_sendfile(fd_t client);
int fshare_recv_mdata(fd_t sock, meta* m);
int fshare_send_mdata(fd_t sock, const meta* m);

#endif // FNET_H
