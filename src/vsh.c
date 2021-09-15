#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"
#include "errors.h"
#include "ls.h"
#include "path.h"
#include "proc.h"
#include "prompt.h"
#include "utils.h"

int main() {
    size_t input_size = 0;
    char* input_line = NULL;

    // set cwd as global home & fallback prev path
    set_home_path(get_current_path());
    set_prev_path(get_current_path());

    // function pointer enum for foreground/background execution
    int (*_execute[])(int (*)(int, char**), int, char**) = {
        execute_foreground,
        execute_background,
        execute_parent,
    };
    enum execute {
        kExec_foreground,
        kExec_background,
        kExec_parent,
    };

    // function pointer enum for command callback
    int (*_callback[])(int, char**) = {
        sys, cd, pwd, echo, ls, sys, sys, jobs,
    };
    enum callback {
        kCall_sys,
        kCall_cd,
        kCall_pwd,
        kCall_echo,
        kCall_ls,
        kCall_pinfo,
        kCall_history,
        kCall_jobs,
    };

    // main loop
    while (1) {
        // render prompt
        print_prompt();

        // wait for input and handle exit command
        if (getline(&input_line, &input_size, stdin) == -1 || !strcmp(strip(input_line), "exit")) {
            printf("%sexit\n", !strcmp(strip(input_line), "exit") ? "" : "\n");
            free(input_line);
            break;
        };

        // parse & execute semicolon separated commands
        int command_count = num_tokens(input_line, ";");
        char** commands = tokenize(input_line, ";");
        for (int i = 0; i < command_count; i++) {
            // current command
            char* command = strip(commands[i]);

            // command properties
            int repeat = 1;                        // number of times to execute command
            enum execute e_id = kExec_foreground;  // execution layer id
            enum callback c_id = kCall_sys;        // callback id

            // tokenize command
            int token_count = num_tokens(command, " \t\r\n\v\f");
            char** tokens = tokenize(command, " \t\r\n\v\f");

            // determine number of times to execute command
            if (!strcmp(tokens[0], "repeat")) {
                repeat = atoi(tokens[1]);

                // reassign to tokens of actual command
                tokens = &tokens[2];
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
            } else if (!strcmp(tokens[0], "jobs")) {
                c_id = kCall_jobs;
            }

            // determine execution enum
            if (c_id == kCall_cd || c_id == kCall_pwd || c_id == kCall_echo) {
                // execute shell builtins in parent process
                e_id = kExec_parent;
            } else if (command[strlen(command) - 1] == '&') {
                // execute command in the background if suffixed with &
                e_id = kExec_background;

                // strip & from command and tokens
                if (tokens[token_count - 1][0] == '&') {
                    // remove last token
                    token_count--;
                    char** new_tokens = calloc(token_count, sizeof(char*));
                    for (int i = 0; i < token_count; i++) new_tokens[i] = tokens[i];
                    tokens = new_tokens;
                } else {
                    // remove last character of last token
                    tokens[token_count - 1][strlen(tokens[token_count - 1] - 2)] = '\0';
                }
            }

            // execute command
            for (int i = 1; i <= repeat; i++) {
                (*_execute[e_id])((*_callback[c_id]), token_count, tokens);
            }
        }
    }

    return 0;
}
