#ifndef FSERVER_H
#define FSERVER_H

typedef int fd_t;

typedef struct fdesc {
    fd_t server;
    fd_t epoll;
    fd_t file;
} fdesc;

/* Server API */
void fshare_server_start(const char* filepath, int port);
void fshare_server_run();
void fshare_server_stop();

/* Socket API */
void fshare_sock_create(int port);
void fshare_set_sock_option();
void fshare_sock_destroy();

/* File API */
void fshare_config_file(const char* filepath);
void fshare_openfile(const char* filepath);
void fshare_closefile();
void fshare_sendfile(fd_t client_fd);

void fshare_close(fd_t fd);

#endif // FSERVER_H

