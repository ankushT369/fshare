#include "fserver.h"
#include "fclient.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define DEFAULT_IP   "127.0.0.1"
#define DEFAULT_PORT 8080

static void usage(const char* prog)
{
    fprintf(stderr,
        "Usage:\n"
        "  %s connect\n"
        "  %s connect <ip:port>\n"
        "  %s server <filename> [--port <port>]\n",
        prog, prog, prog);
}

static int parse_ip_port(const char* s,
                         char* ip, size_t iplen,
                         int* port)
{
    const char* colon = strchr(s, ':');
    if (!colon)
        return -1;

    size_t len = colon - s;
    if (len == 0 || len >= iplen)
        return -1;

    memcpy(ip, s, len);
    ip[len] = '\0';

    *port = atoi(colon + 1);
    if (*port <= 0 || *port > 65535)
        return -1;

    return 0;
}

int main(int argc, char** argv)
{
    int port = DEFAULT_PORT;

    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    /* SERVER MODE */
    if (strcmp(argv[1], "server") == 0) {
        if (argc != 3 && argc != 5) {
            usage(argv[0]);
            return 1;
        }

        const char* filename = argv[2];

        if (argc == 5) {
            if (strcmp(argv[3], "--port") != 0) {
                usage(argv[0]);
                return 1;
            }
            port = atoi(argv[4]);
            if (port <= 0 || port > 65535) {
                fprintf(stderr, "Invalid port\n");
                return 1;
            }
        }

        fshare_server_start(100, filename, port);
        fshare_server_run();
        fshare_server_stop();
        return 0;
    }

    /* CLIENT MODE */
    if (strcmp(argv[1], "connect") == 0) {
        char ip[64] = DEFAULT_IP;
        int cport = DEFAULT_PORT;

        if (argc == 3) {
            if (parse_ip_port(argv[2], ip, sizeof(ip), &cport) < 0) {
                fprintf(stderr, "Invalid address format, use ip:port\n");
                return 1;
            }
        } else if (argc != 2) {
            usage(argv[0]);
            return 1;
        }

        fd_t sock = fshare_connect_server(ip, cport);
        if (sock < 0)
            return 1;

        fshare_client_recv(sock);
        close(sock);
        return 0;
    }

    usage(argv[0]);
    return 1;
}

