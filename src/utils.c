#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// format string with colors
char* colorize(char* color, char* str) {
    char* colored_str = calloc(strlen(str), sizeof(char));
    sprintf(colored_str, "%s%s%s", color, str, ANSI_RESET);
    return colored_str;
}

// return number of tokens in string separated by delim
int num_tokens(char* str, char* delim) {
    int token_count = 0;
    char* token = strtok(strdup(str), delim);
    while (token) {
        token_count++;
        token = strtok(NULL, delim);
    }

    // decrement count if last non-whitespace character is the delimiter
    char last_nws_char;
    for (int i = strlen(str) - 1; i >= 0; i--) {
        if (!isspace(str[i])) {
            last_nws_char = str[i];
            break;
        }
    }
    if (last_nws_char == delim[0]) token_count--;

    return token_count;
}

// return list of tokens in string separated by delim
char** tokenize(char* str, char* delim) {
    char* token;

    // determine number of tokens
    int token_count = num_tokens(str, delim);

    // parse tokens into list
    char** tokens = calloc(token_count, sizeof(char*));
    token = strtok(strdup(str), delim);
    for (int i = 0; i < token_count; i++) {
        tokens[i] = calloc(strlen(token), sizeof(char));
        tokens[i] = token;
        token = strtok(NULL, delim);
    }

    return tokens;
}
