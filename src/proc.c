#include "proc.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// execute system process
int sys(char** args) { return execvp(args[0], args); }

// execute process in the foreground
int execute_fg(int (*f)(char**), char** args) {
    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        // child
        (*f)(args);
    } else {
        // parent
        wait(NULL);
    }

    return 0;
}
