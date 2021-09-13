#include "prompt.h"

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "errors.h"

void print_prompt() {
    char* username = malloc(USERNAME_MAX);
    char* hostname = malloc(HOSTNAME_MAX);

    // fetch current username
    username = getpwuid(getuid())->pw_name;
    if (!username) {
        throw_fatal_error("Unable to retrieve username");
    }

    // fetch current hostname
    if (gethostname(hostname, HOSTNAME_MAX)) {
        throw_fatal_error("Unable to retrieve hostname");
    }

    printf("<%s@%s>\n", username, hostname);
}
