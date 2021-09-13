#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// string formatted with colors
char* colorize(char* color, char* str) {
    char* colored_str = calloc(strlen(str), sizeof(char));
    sprintf(colored_str, "%s%s%s", color, str, ANSI_RESET);
    return colored_str;
}
