#include "prompt.h"

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "errors.h"
#include "path.h"

// render prompt string
void print_prompt() {
    char* username = calloc(USERNAME_MAX, sizeof(char));
    char* hostname = calloc(HOSTNAME_MAX, sizeof(char));

    // fetch current username
    username = getpwuid(getuid())->pw_name;
    if (!username) {
        throw_fatal_error("Unable to retrieve username");
    }

    // fetch current hostname
    if (gethostname(hostname, HOSTNAME_MAX)) {
        throw_fatal_error("Unable to retrieve hostname");
    }

    printf("<%s@%s:%s> ", username, hostname, shorten_path(get_current_path()));
}
