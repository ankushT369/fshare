#include "fsig.h"

#include <string.h>

volatile sig_atomic_t fsig_stop = 0;

static void fsig_handler(int sig)
{
    (void)sig;
    fsig_stop = 1;
}

int fsig_init(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = fsig_handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) < 0)
        return -1;

    if (sigaction(SIGTERM, &sa, NULL) < 0)
        return -1;

    return 0;
}

