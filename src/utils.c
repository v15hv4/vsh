#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
char** tokenize(char* str, char* delim) {
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
