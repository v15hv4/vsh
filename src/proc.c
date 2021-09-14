#include "proc.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "errors.h"

// execute system process
int sys(int argc, char** argv) { return execvp(argv[0], argv); }

// execute process in the foreground
int execute_foreground(int (*f)(int, char**), int argc, char** argv) {
    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        // child
        if ((*f)(argc, argv)) {
            char* errmsg = calloc(128, sizeof(char));
            sprintf(errmsg, "%s", argv[0]);
            throw_blocking_error(errmsg, -1);
            exit(1);
        };
    } else {
        // parent
        wait(NULL);
    }

    return 0;
}

// execute process in the background
int execute_background(int (*f)(int, char**), int argc, char** argv) {
    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        // child
        printf("TODO: run %s in the background\n", argv[0]);
        return 0;
    } else {
        // parent
        wait(NULL);
    }

    return 0;
}

// execute process in the parent
int execute_parent(int (*f)(int, char**), int argc, char** argv) {
    if ((*f)(argc, argv)) {
        char* errmsg = calloc(128, sizeof(char));
        sprintf(errmsg, "%s", argv[0]);
        throw_blocking_error(errmsg, -1);
    };

    return 0;
}
