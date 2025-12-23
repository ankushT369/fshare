#ifndef FSIG_H
#define FSIG_H

#include <signal.h>

int fsig_init(void);

/* global interrupt flag (defined in fsig.c) */
extern volatile sig_atomic_t fsig_stop;


static inline int fsig_interrupted(void)
{
    return fsig_stop != 0;
}

static inline void fsig_reset(void)
{
    fsig_stop = 0;
}

#endif /* FSIG_H */

