#include "signals.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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
    }
};

// interrupt the current foreground process
void interrupt_fg(int signal) {
    if (CURRENT_FOREGROUND_PROCESS.pid < 0) {
        printf("\n");
        print_prompt();
    }
}

// suspend the current foreground process
void suspend_fg(int signal) {
    if (CURRENT_FOREGROUND_PROCESS.pid > 0) {
        // create a new process group for the child
        setpgid(CURRENT_FOREGROUND_PROCESS.pid, 0);

        // maintain job info in the parent's job pool
        add_process(CURRENT_FOREGROUND_PROCESS.pid, CURRENT_FOREGROUND_PROCESS.pname);

        // reset current foreground process
        CURRENT_FOREGROUND_PROCESS = (struct Process)PROCESS_DEFAULT;
    }
}
