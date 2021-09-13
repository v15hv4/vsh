#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "path.h"
#include "prompt.h"

int main() {
    size_t input_size = 0;
    char* input_line = NULL;

    // set global home as cwd
    set_home_path(get_current_path());

    // main loop
    while (1) {
        print_prompt();
        if (getline(&input_line, &input_size, stdin) == -1) break;
        printf("you entered: %s\n", input_line);
    }

    free(input_line);
    printf("\nexit\n");
    return 0;
}
