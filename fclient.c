#include "fclient.h"
#include "fnet.h"
#include "fsig.h"
#include "ffile.h"
#include "fext.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#define BUF_SIZE 8192

fclient_ctx_t ctx;

static char fshare_rand_char(void) {
    static const char charset[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789";
    return charset[rand() % (sizeof(charset) - 1)];
}

void fshare_rand_filename(fclient_ctx_t *ctx) {
    static int seeded = 0;

    if (!ctx)
        return;

    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }

    const char* ext = "unknown";
    if (ctx->e > EXT_UNKNOWN && ctx->e < EXT_COUNT)
        ext = ext_str[ctx->e];

    while (1) {
        int name_len = 6 + rand() % 5;   /* 6–10 */
        size_t ext_len = strlen(ext);

        size_t total_len = name_len + 1 + ext_len + 1;
        char* filename = malloc(total_len);
        if (!filename)
            return;

        for (int i = 0; i < name_len; ++i)
            filename[i] = fshare_rand_char();

        filename[name_len] = '.';
        memcpy(filename + name_len + 1, ext, ext_len);
        filename[total_len - 1] = '\0';

        int fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0644);
        if (fd != -1) {
            ctx->file = fd;
            printf("created file: %s (fd=%d)\n", filename, fd);
            free(filename);   /* name not needed anymore */
            return;
        }

        free(filename);   /* collision → retry */
    }
}

static void cleanup(void)
{
    if (ctx.sockfd >= 0) {
        fshare_file_close(ctx.sockfd);
        fshare_file_close(ctx.file);

        ctx.sockfd = -1;
        ctx.file = -1;
        ctx.size = 0;
    }

    /* restore cursor */
    fprintf(stderr, "\033[?25h\n");
}

int fshare_connect_server(const char* ip, int port) {
    fd_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        return -1;
    }

    ctx.sockfd = sockfd;

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port   = htons(port);

    if (inet_pton(AF_INET, ip, &server.sin_addr) != 1) {
        perror("inet_pton failed");
        fshare_file_close(ctx.sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect failed");
        fshare_file_close(ctx.sockfd);
        return -1;
    }

    return 0;
}

static void print_progress(size_t downloaded, size_t total)
{
    int percent = (int)((downloaded * 100) / total);
    int bar_width = 40;
    int filled = (percent * bar_width) / 100;

    fprintf(stderr, "\r[");
    for (int i = 0; i < bar_width; i++) {
        if (i < filled)
            fputs("█", stderr);
        else
            fputc('-', stderr);
    }
    fprintf(stderr, "] %3d%%", percent);
    fflush(stderr);
}

void fshare_proto()
{
    const meta reqmeta = {
        .code = FSHARE_META,
        .status = 0,
        .size = 0,
        .e = EXT_UNKNOWN,
    };
    int ret = fshare_send_mdata(ctx.sockfd, &reqmeta);
    if (ret != 0) {
        return ;
    }

    meta resp;
    ret = fshare_recv_mdata(ctx.sockfd, &resp);
    if (ret != 0) {
        return ;
    }

    ctx.size = resp.size;
    ctx.e = resp.e;
    printf("Size of the file, extension: %zu, %d\n", resp.size, resp.e);
    const meta reqdata = {
        .code = FSHARE_DATA,
        .status = 0,
        .size = 0,
        .e = EXT_UNKNOWN,
    };

    ret = fshare_send_mdata(ctx.sockfd, &reqdata);
    if (ret != 0) {
        return ;
    }

}

void fshare_client_recv()
{
    /* Erase cursor */
    fprintf(stderr, "\033[?25l");
    fshare_rand_filename(&ctx);

    if (ctx.file < 0) {
        fprintf(stderr, "failed to create file\n");
        return;
    }

    char buf[BUF_SIZE];
    size_t downloaded = 0;

    while (!fsig_interrupted()) {
        ssize_t n = read(ctx.sockfd, buf, sizeof(buf));

        if (n < 0) {
            if (errno == EINTR)
                continue;
            perror("read failed");
            break;
        }

        if (n == 0)
            break;

        ssize_t off = 0;
        while (off < n) {
            ssize_t w = write(ctx.file, buf + off, n - off);
            if (w < 0) {
                perror("write failed");
                cleanup(); return;
            }
            off += w;
        }

        downloaded += n;
        print_progress(downloaded, ctx.size);
    }

    if (fsig_interrupted()) {
        fprintf(stderr, "\nDownload interrupted by user\n");
        cleanup();
        return;
    }

    fprintf(stderr, "\nDownload complete (%zu bytes)\n", downloaded);
    cleanup();
}

