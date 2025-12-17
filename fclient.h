#ifndef FCLIENT_H
#define FCLIENT_H

typedef int fd_t;

fd_t fshare_connect_server(const char* ip, int port);
void fshare_client_recv(fd_t sockfd);

#endif // FCLIENT_H

