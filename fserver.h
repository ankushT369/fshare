#ifndef FSERVER_H
#define FSERVER_H

#include "ftype.h"
#include "fext.h"

#include <stddef.h>

typedef struct {
    fd_t server;
    fd_t file;
    size_t file_size;
    ext e;
} fserver_ctx_t;

/* Server API */
void fshare_server_start(const char* filepath, int port);
void fshare_server_run();
void fshare_server_stop();

void fshare_close(fd_t fd);

#endif // FSERVER_H
