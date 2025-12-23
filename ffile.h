#ifndef FFILE_H
#define FFILE_H

#include "ftype.h"
#include "fserver.h"
#include "fext.h"

#include <stddef.h>


/* File API */
void fshare_file_config(const char* filepath, fserver_ctx_t* ctx);
void fshare_file_open(const char* filepath, fserver_ctx_t* ctx);
void fshare_file_close(fd_t file);
void fshare_file_send(fd_t client, fserver_ctx_t ctx);
ext fshare_file_get_ext(const char* filename);

#endif // FFILE_H
