#include "path.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "errors.h"

char* HOME_PATH;
char* PREV_PATH;

// get absolute path of input relative path
char* expand_path(char* path) {
    char* expanded_path = calloc(strlen(HOME_PATH) + strlen(path), sizeof(char));

    // tilde expansion
    if (path[0] == '~') {
        strcat(expanded_path, HOME_PATH);
        strcat(expanded_path, path + 1);
    } else {
        strcat(expanded_path, path);
    }

    return expanded_path;
}

// shorten input path by replacing home directory with ~
char* shorten_path(char* path) {
    char* shortened_path = calloc(strlen(path), sizeof(char));

    // replace home path with tilde
    char* prefix = calloc(strlen(HOME_PATH), sizeof(char));
    strncpy(prefix, path, strlen(HOME_PATH));
    prefix[strlen(HOME_PATH)] = '\0';
    if (!strcmp(HOME_PATH, prefix)) {
        strcat(shortened_path, "~");
        strcat(shortened_path, path + strlen(HOME_PATH));
    } else {
        strcat(shortened_path, path);
    }

    return shortened_path;
}

// get the shell's current working directory path
char* get_current_path() {
    char* cwd = calloc(PATH_MAX, sizeof(char));
    if (!getcwd(cwd, PATH_MAX)) {
        throw_fatal_error("Unable to get current working directory path");
    }
    return cwd;
}

// set the shell's home path
void set_home_path(char* path) { HOME_PATH = strdup(path); }

// get the shell's home path
char* get_home_path() { return HOME_PATH; }

// set the shell's previous working directory path
void set_prev_path(char* path) { PREV_PATH = strdup(path); }

// get the shell's previous working directory path
char* get_prev_path() { return PREV_PATH; }
