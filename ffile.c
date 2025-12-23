#include "ffile.h"
#include "fserver.h"

#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <errno.h>


void fshare_file_config(const char* filepath, fserver_ctx_t* ctx) {
    fshare_file_open(filepath, ctx);
}

long get_file_size(fd_t file) {
    struct stat st;

    // Call stat() function, passing the filename and the address of the stat structure
    if (fstat(file, &st) == -1) {
        perror("Error getting file info");
        return -1; // Return -1 to indicate an error
    }

    // The st_size field contains the file size in bytes
    return st.st_size;
}

void fshare_file_open(const char* filepath, fserver_ctx_t* ctx) {
    ctx->file = open(filepath, O_RDONLY);
    if (ctx->file < 0) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    ctx->file_size = (size_t)get_file_size(ctx->file);
}

void fshare_file_close(fd_t file) {
    fshare_close(file);
}

void fshare_file_send(fd_t client, fserver_ctx_t ctx) {
    off_t offset = 0;
    ssize_t remaining = (ssize_t)ctx.file_size;

    while (remaining > 0) {
        ssize_t sent = sendfile(client, ctx.file,
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

ext fshare_file_get_ext(const char* filename) {
    if (!filename || !*filename)
        return EXT_UNKNOWN;

    const char* end = filename + strlen(filename);
    const char* dot = NULL;

    for (const char* p = end - 1; p >= filename; --p) {
        if (*p == '/' || *p == '\\')
            break;                  /* hit path separator → stop */
        if (*p == '.' && p != end - 1) {
            dot = p;
            break;
        }
    }

    if (!dot || dot == filename)
        return EXT_UNKNOWN;

    for (int i = 1; i < EXT_COUNT; ++i) {
        const char* ext_str_i = ext_str[i];
        const char* s = dot + 1;
        const char* t = ext_str_i;

        while (*s && *t && tolower((unsigned char)*s) == *t) {
            ++s;
            ++t;
        }

        if (*s == '\0' && *t == '\0')
            return (ext)i;
    }

    return EXT_UNKNOWN;
}
