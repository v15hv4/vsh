#include "cd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "errors.h"
#include "path.h"

int cd(int argc, char** argv) {
    char* current_path = get_current_path();
    char* target_path;

    if (argc < 2) {
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
        throw_blocking_error("cd", -1);
    };

    // update previous path
    set_prev_path(current_path);

    return 0;
}
