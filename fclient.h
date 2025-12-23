#ifndef FCLIENT_H
#define FCLIENT_H

#include "ftype.h"
#include "fext.h"

#include <stddef.h>

typedef struct {
    fd_t sockfd;
    fd_t file;
    size_t size;
    ext e;
} fclient_ctx_t;

int fshare_connect_server(const char* ip, int port);
void fshare_proto();
void fshare_client_recv();

#endif // FCLIENT_H

