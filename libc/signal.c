#include "../debug/printf.h"
#include <signal.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <string.h>

void (*shandlers[32])(int);

static void sighandler(int signum) {
    printf("received signal %i\n%s", signum, strsignal(signum));
    exit(1);
}

void (*signal(int sig, void (*func)(int)))(int) {
    if (sig <= 0) {
        return SIG_ERR;
    }

    if (func == SIG_DFL) {
        shandlers[sig] = sighandler;
        return sighandler;
    }

    shandlers[sig] = func;
    return func;
}

int raise(int sig) {
    void (*shand)(int) = shandlers[sig];
    shand(sig);
    return 0;
}

void init_signal() {
    int i = 0;
    for (i = 0; i < 32; i++) {
        signal(i, sighandler);
    }
}
