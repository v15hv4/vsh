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

    // function pointer enum for foreground/background execution
    int (*_execute[])(int (*)(char**), char**) = {
        execute_fg,
        execute_bg,
    };
    enum execute {
        kExec_fg,
        kExec_bg,
    };

    // function pointer enum for command callback
    int (*_callback[])(char**) = {sys, sys, sys, sys, sys, sys, sys, sys};
    enum callback {
        kCall_sys,
        kCall_cd,
        kCall_pwd,
        kCall_echo,
        kCall_ls,
        kCall_pinfo,
        kCall_history,
    };

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
            // current command
            char* command = strip(commands[i]);

            // command properties
            int repeat = 1;                // number of times to execute command
            enum execute e_id = kExec_fg;  // execution layer id
            enum callback c_id = kCall_sys;  // callback id

            // tokenize command
            int token_count = num_tokens(command, " ");
            char** tokens = tokenize(command, " ");

            // determine number of times to execute command
            if (!strcmp(tokens[0], "repeat")) {
                repeat = atoi(tokens[1]);
                tokens = &tokens[2];  // reassign to tokens of actual command
            }

            // check if command is supposed to run in the background
            if (command[strlen(command) - 1] == '&') {
                e_id = kExec_bg;
            }

            // determine callback enum
            if (!strcmp(tokens[0], "cd")) {
                c_id = kCall_cd;
            } else if (!strcmp(tokens[0], "pwd")) {
                c_id = kCall_pwd;
            } else if (!strcmp(tokens[0], "echo")) {
                c_id = kCall_echo;
            } else if (!strcmp(tokens[0], "ls")) {
                c_id = kCall_ls;
            } else if (!strcmp(tokens[0], "pinfo")) {
                c_id = kCall_pinfo;
            } else if (!strcmp(tokens[0], "history")) {
                c_id = kCall_history;
            }

            // execute command
            for (int i = 1; i <= repeat; i++) {
                (*_execute[e_id])((*_callback[c_id]), tokens);
            }
        }
    }

    return 0;
}
