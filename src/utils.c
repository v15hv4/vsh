#include "libs.h"

void cprintf(char *color, char *str) {
    printf("%s%s%s", color, str, ANSI_RESET);
}
