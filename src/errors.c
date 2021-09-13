#include "errors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void throw_fatal_error(char* errmsg) {
    char* error = malloc(7 + strlen(errmsg) + 1);
    strcat(error, "FATAL: ");
    strcat(error, errmsg);
    perror(error);
    exit(1);
}

int throw_blocking_error(char* errmsg, int errcode) {
    perror(errmsg);
    return errcode;
}
