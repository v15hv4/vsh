#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "errors.h"
#include "path.h"

// execute shell builtin `cd`
int cd(int argc, char** argv) {
    char* current_path = get_current_path();
    char* target_path;

    if (argc > 2) {
        return throw_custom_error("cd: must have exactly one argument", -1);
    } else if (argc < 2) {
        // cd to home
        target_path = expand_path(get_home_path());
    } else if (!strcmp(argv[1], "-")) {
        // cd to prev path
        target_path = expand_path(get_prev_path());
    } else {
        // cd to argv[1]
        target_path = expand_path(argv[1]);
    }

    // execute cd
    if (chdir(target_path)) {
        return throw_blocking_error("cd", -1);
    };

    // update previous path
    set_prev_path(current_path);

    return 0;
}

// execute shell builtin `pwd`
int pwd(int argc, char** argv) {
    printf("%s\n", get_current_path());
    return 0;
}

// execute shell builtin `echo`
int echo(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");

    exit(0);
}
