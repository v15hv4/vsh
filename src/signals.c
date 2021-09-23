#include "signals.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proc.h"
#include "prompt.h"

// install handler for given signal number
void handle_signal(int signum, void (*handler)(int)) {
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = handler;
    action.sa_flags = SA_RESTART;
    sigaction(signum, &action, NULL);
};

// remove child processes that are completed
void reap_zombies(int signal) {
    pid_t pid;
    int status;
    int killed = 0;

    // print exit message for each finished bg process
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        status = WIFEXITED(status);
        struct Process process = remove_process(pid);
        printf("\n%s with pid %d exited %snormally\n", process.pname, pid, status ? "" : "ab");
        killed = 1;
    }

    // reprint prompt string in case a process was killed
    if (killed) {
        print_prompt();
        fflush(stdout);
    }
};

// take lite
void do_nothing(int signal) {
    printf("\n");
    print_prompt();
    fflush(stdout);
}
