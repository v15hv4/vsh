#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "errors.h"
#include "path.h"
#include "proc.h"
#include "prompt.h"
#include "utils.h"

int main() {
    size_t input_size = 0;
    char* input_line = NULL;

    // set cwd as global home
    set_home_path(get_current_path());

    // main loop
    while (1) {
        // render prompt and wait for input
        print_prompt();
        if (getline(&input_line, &input_size, stdin) == -1) {
            free(input_line);
            printf("\nexit\n");
            return 0;
        };

        // parse & execute semicolon separated commands
        int num_commands = num_tokens(input_line, ";");
        char** commands = tokenize(input_line, ";");
        for (int i = 0; i < num_commands; i++) {
            // TODO: implement bg process execution here
            char** tokens = tokenize(commands[i], " ");
            execute_fg(sys, tokens);
        }
    }

    return 0;
}
