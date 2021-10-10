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

// open file, assign fd for redirecting from/to it, return command
char* redirect(char* command, int* in_fd, int* out_fd) {
    char* start = command;
    char* end = strrchr(command, '\0');

    // output redirection
    char* out_ptr = strchr(command, '>');
    if (!(long long)out_ptr) out_ptr = end;

    // input redirection
    char* in_ptr = strchr(command, '<');
    if (!(long long)in_ptr) in_ptr = start;

    // reassign output fd
    if (out_ptr++ != strrchr(command, '\0')) {
        int append = (out_ptr[0] == '>');
        out_ptr += append;

        int out_size = end - out_ptr;
        char* out_path = calloc(out_size, sizeof(char));
        strncpy(out_path, strip(out_ptr), out_size);

        *out_fd = open(strip(out_path), O_WRONLY | O_CREAT | (append ? O_APPEND : 0), 0644);
        if (*out_fd < 0) {
            throw_blocking_error("redirect", -1);
            return NULL;
        }

        // reassign end of command pointer
        end = out_ptr - (1 + append);
    }

    // reassign input fd
    if (in_ptr++ != command) {
        int in_size = out_ptr - in_ptr - 2;
        char* in_path = calloc(in_size, sizeof(char));
        strncpy(in_path, strip(in_ptr), in_size);

        *in_fd = open(strip(in_path), O_RDONLY, 0644);
        if (*in_fd < 0) {
            throw_blocking_error("redirect", -1);
            return NULL;
        }

        // reassign end of command pointer
        end = in_ptr - 1;
    }

    // regenerate command
    int new_command_size = end - start;
    char* new_command = calloc(new_command_size, sizeof(char));
    strncpy(new_command, strip(start), (end - start));

    return new_command;
}
