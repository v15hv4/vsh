#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"
#include "errors.h"
#include "history.h"
#include "ls.h"
#include "path.h"
#include "pinfo.h"
#include "proc.h"
#include "prompt.h"
#include "utils.h"

int main() {
    // control debug mode
    int DEBUG = 0;

    // initialize input buffer
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
        sys, __exit, __cd, __pwd, __echo, ls, pinfo, history, jobs,
    };
    enum callback {
        kCall_sys,
        kCall_exit,
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
        if (getline(&input_line, &input_size, stdin) == -1) {
            free(input_line);
            break;
        };

        // parse & execute semicolon separated commands
        int command_count = num_tokens(input_line, ";");
        char** commands = split(input_line, ";");
        for (int i = 0; i < command_count; i++) {
            // current command
            char* command = strip(commands[i]);

            // command properties
            int repeat = 1;                        // number of times to execute command
            enum execute e_id = kExec_foreground;  // execution layer id
            enum callback c_id = kCall_sys;        // callback id

            // tokenize command
            int token_count = num_tokens(command, " \t\r\n\v\f");
            char** tokens = split(command, " \t\r\n\v\f");

            // determine number of times to execute command
            if (!strcmp(tokens[0], "repeat")) {
                repeat = atoi(tokens[1]);

                // reassign to tokens of actual command
                tokens = &tokens[2];
                token_count -= 2;
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
            } else if (!strcmp(tokens[0], "exit")) {
                c_id = kCall_exit;
            }

            // determine execution enum
            if (c_id == kCall_exit || c_id == kCall_cd || c_id == kCall_pwd || c_id == kCall_echo ||
                c_id == kCall_pinfo) {
                // execute shell builtins in parent process
                e_id = kExec_parent;
            } else if (command[strlen(command) - 1] == '&') {
                // execute command in the background if suffixed with &
                e_id = kExec_background;

                // strip & from command and tokens
                if (tokens[token_count - 1][0] == '&') {
                    // remove final token
                    token_count--;
                    char** new_tokens = calloc(token_count, sizeof(char*));
                    for (int i = 0; i < token_count; i++) new_tokens[i] = tokens[i];
                    tokens = new_tokens;
                } else {
                    // remove last character of final token
                    int new_final_token_len = strlen(tokens[token_count - 1]);
                    char* new_final_token = calloc(new_final_token_len, sizeof(char));
                    for (int i = 0; i < new_final_token_len - 1; i++) {
                        new_final_token[i] = tokens[token_count - 1][i];
                    }
                    new_final_token[new_final_token_len - 1] = '\0';
                    tokens[token_count - 1] = new_final_token;
                }
            }

            // DEBUG OUTPUT
            if (DEBUG) {
                printf(
                    "----------\n"
                    "e_id: %d\n"
                    "c_id: %d\n"
                    "repeat: %d\n"
                    "token_count: %d\n"
                    "tokens: %s\n"
                    "----------\n",
                    e_id, c_id, repeat, token_count, join(tokens, token_count, ", "));
            }

            // execute command
            for (int i = 1; i <= repeat; i++) {
                (*_execute[e_id])((*_callback[c_id]), token_count, tokens);
            }

            // write command to history file
            write_history(strip(input_line));
        }
    }

    return 0;
}
