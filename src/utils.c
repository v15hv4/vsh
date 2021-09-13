#include "utils.h"

#include <stdio.h>

// colored string output
void cprintf(char* color, char* str) {
    printf("%s%s%s", color, str, ANSI_RESET);
}
