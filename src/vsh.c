#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "prompt.h"

int main() {
    size_t input_size = 0;
    char* input_line = NULL;

    while (1) {
        print_prompt();
        if (getline(&input_line, &input_size, stdin) == -1) break;
        printf("you entered: %s", input_line);
    }

    free(input_line);
    printf("\nexit\n");
    return 0;
}
