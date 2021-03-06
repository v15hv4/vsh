#include "errors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// display message and exit
void throw_fatal_error(char* errmsg) {
    char* error = calloc(7 + strlen(errmsg) + 1, sizeof(char));
    strcat(error, "FATAL: ");
    strcat(error, errmsg);
    perror(error);
    exit(1);
}

// display message and return error code
int throw_blocking_error(char* errmsg, int errcode) {
    char* error = calloc(7 + strlen(errmsg) + 1, sizeof(char));
    strcat(error, "ERROR: ");
    strcat(error, errmsg);
    perror(errmsg);
    return errcode;
}

// display custom message and return error code
int throw_custom_error(char* errmsg, int errcode) {
    fprintf(stderr, "%s\n", errmsg);
    return errcode;
}
