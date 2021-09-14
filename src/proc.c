#include "proc.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "errors.h"

// execute system process
int sys(char** args) {
    if (execvp(args[0], args)) {
        char* errmsg = calloc(128, sizeof(char));
        sprintf(errmsg, "%s", args[0]);
        return throw_blocking_error(errmsg, -1);
    }
    return 0;
}

// execute process in the foreground
int execute_fg(int (*f)(char**), char** args) {
    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        // child
        return (*f)(args);
    } else {
        // parent
        wait(NULL);
    }

    return 0;
}

// execute process in the background
int execute_bg(int (*f)(char**), char** args) {
    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        // child
        printf("TODO: run %s in the background\n", args[0]);
        return 0;
    } else {
        // parent
        wait(NULL);
    }

    return 0;
}
