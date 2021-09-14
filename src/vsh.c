#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    // function pointer array for foreground/background execution
    int (*execute[])(int (*)(char**), char**) = {execute_fg, execute_bg};

    // main loop
    while (1) {
        // render prompt and wait for input
        print_prompt();
        if (getline(&input_line, &input_size, stdin) == -1) {
            free(input_line);
            printf("\nexit\n");
            break;
        };

        // parse & execute semicolon separated commands
        int command_count = num_tokens(input_line, ";");
        char** commands = tokenize(input_line, ";");
        for (int i = 0; i < command_count; i++) {
            char* command = strip(commands[i]);  // current command
            int repeat = 1;  // number of times to execute command
            int layer = 0;   // foreground: 0, background: 1

            // tokenize command
            int token_count = num_tokens(command, " ");
            char** tokens = tokenize(command, " ");

            // determine number of times to execute command
            if (!strcmp(tokens[0], "repeat")) {
                repeat = atoi(tokens[1]);

                // reassign tokens to that of the command to be repeated
                tokens = &tokens[2];
            }

            // determine layer of command
            if (command[strlen(command) - 1] == '&') {
                layer = 1;
            }

            // execute command
            for (int i = 1; i <= repeat; i++) {
                (*execute[layer])(sys, tokens);
            }
        }
    }

    return 0;
}
