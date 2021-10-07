#include "utils.h"

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "errors.h"

// format string with colors
char* colorize(char* color, char* str) {
    char* colored_str = calloc(strlen(str) + 3 * strlen(ANSI_RESET), sizeof(char));
    sprintf(colored_str, "%s%s%s", color, str, ANSI_RESET);
    return colored_str;
}

// strip leading and trailing whitespace from string
char* strip(char* str) {
    int len = strlen(str);
    while (isspace(str[len - 1])) --len;
    while (*str && isspace(*str)) ++str, --len;
    return strndup(str, len);
}

// return number of tokens in string separated by delim
int num_tokens(char* str, char* delim) {
    // strip whitespace
    char* stripped_str = strip(str);

    // enumerate tokens
    int token_count = 0;
    char* token = strtok(stripped_str, delim);
    while (token) {
        token_count++;
        token = strtok(NULL, delim);
    }

    // decrement count if last non-whitespace character is the delimiter
    if (stripped_str[strlen(stripped_str) - 1] == delim[0]) token_count--;

    return token_count;
}

// return list of tokens in string separated by delim
char** split(char* str, char* delim) {
    // strip whitespace
    char* stripped_str = strip(str);

    // determine number of tokens
    int token_count = num_tokens(stripped_str, delim);

    // parse tokens into list
    char** tokens = calloc(token_count, sizeof(char*));
    char* token = strtok(strdup(stripped_str), delim);
    for (int i = 0; i < token_count; i++) {
        tokens[i] = calloc(strlen(token), sizeof(char));
        tokens[i] = token;
        token = strtok(NULL, delim);
    }

    return tokens;
}

// return string made by concatenating array elements with delim
char* join(char** arr, int arr_length, char* delim) {
    // determine joined string length
    int str_length = 0;
    for (int i = 0; i < arr_length - 1; i++) {
        str_length += strlen(arr[i]) + strlen(delim);
    }
    str_length += strlen(arr[arr_length - 1]);

    // generate string
    char* str = calloc(str_length, sizeof(char));
    for (int i = 0; i < arr_length - 1; i++) {
        strcat(str, arr[i]);
        strcat(str, delim);
    }
    strcat(str, arr[arr_length - 1]);

    return str;
}

// open file and return fd for redirecting from/to it
int redirect(char mode, char* command, char** tokens, int* token_count) {
    int fd = -1;
    char** redir_tokens;
    char** trailing_tokens;
    char* trailing_command;

    // TODO: handle multiple < and >

    if (mode == 'r') {
        redir_tokens = split(command, "<");
        trailing_tokens = split(redir_tokens[1], ">");
        fd = open(strip(trailing_tokens[0]), O_RDONLY, 0644);
    } else if (mode == 'a') {
        redir_tokens = split(command, ">>");
        trailing_tokens = split(redir_tokens[1], "<");
        fd = open(strip(trailing_tokens[0]), O_WRONLY | O_APPEND | O_CREAT, 0644);
    } else if (mode == 'w') {
        redir_tokens = split(command, ">");
        trailing_tokens = split(redir_tokens[1], "<");
        fd = open(strip(trailing_tokens[0]), O_WRONLY | O_CREAT, 0644);
    }

    if (fd < 0) {
        char* errmsg = calloc(256, sizeof(char));
        char* errformat = "redirect: %s";
        sprintf(errmsg, errformat, redir_tokens[1]);
        throw_fatal_error(errmsg);
    }

    // update command and tokens
    printf("0: %s \n 1: %s\n", redir_tokens[0], trailing_tokens[1]);
    command = redir_tokens[0];  // TODO: also redir_tokens[2:]
    tokens = split(command, WHITESPACE);
    *token_count = num_tokens(command, WHITESPACE);

    return fd;
}
