#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "errors.h"
#include "path.h"
#include "prompt.h"
#include "utils.h"

int main() {
    size_t input_size = 0;
    char* input_line = NULL;

    // set cwd as global home
    set_home_path(get_current_path());

    // main loop
    while (1) {
        print_prompt();
        if (getline(&input_line, &input_size, stdin) == -1) break;

        // parse & execute semicolon separated commands
        int num_commands = num_tokens(input_line, ";");
        char** commands = tokenize(input_line, ";");
        for (int i = 0; i < num_commands; i++) {
            // TODO: implement fg and bg process execution here
            char** tokens = tokenize(commands[i], " ");
            execvp(tokens[0], tokens);
        }
    }

    free(input_line);
    printf("\nexit\n");
    return 0;
}
