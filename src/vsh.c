#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"
#include "commands.h"
#include "errors.h"
#include "history.h"
#include "path.h"
#include "proc.h"
#include "prompt.h"
#include "signals.h"
#include "terminal.h"
#include "utils.h"

int main() {
    // control debug mode
    int DEBUG = 0;

    // set up signal handlers
    handle_signal(SIGCHLD, reap_zombies);
    handle_signal(SIGINT, interrupt_fg);  // Ctrl + C
    handle_signal(SIGTSTP, suspend_fg);   // Ctrl + Z

    // initialize session history
    refetch_history_cache();

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
    int (*_callback[])(int, char**) = {sys,   __exit,  __cd, __pwd, __echo, ls,
                                       pinfo, history, jobs, sig,   fg,     bg};
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
        kCall_sig,
        kCall_fg,
        kCall_bg,
    };

    // main loop
    while (1) {
        // disable buffering on stdout
        setbuf(stdout, NULL);

        // render prompt
        print_prompt();

        // TODO: get raw input if terminal else default input
        char* input_line = get_raw_input();
        /* char* input_line = get_default_input(); */

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

                // if `repeat` syntax is valid
                if (repeat && token_count > 2) {
                    // reassign to tokens of actual command
                    tokens = &tokens[2];
                    token_count -= 2;
                } else {
                    repeat = 1;
                }
            }

            // determine callback enum
            if (!strcmp(tokens[0], "cd")) {
                c_id = kCall_cd;
                e_id = kExec_parent;
            } else if (!strcmp(tokens[0], "pwd")) {
                c_id = kCall_pwd;
                e_id = kExec_parent;
            } else if (!strcmp(tokens[0], "echo")) {
                c_id = kCall_echo;
            } else if (!strcmp(tokens[0], "ls")) {
                c_id = kCall_ls;
            } else if (!strcmp(tokens[0], "pinfo")) {
                c_id = kCall_pinfo;
                e_id = kExec_parent;
            } else if (!strcmp(tokens[0], "history")) {
                c_id = kCall_history;
            } else if (!strcmp(tokens[0], "exit")) {
                c_id = kCall_exit;
                e_id = kExec_parent;
            } else if (!strcmp(tokens[0], "jobs")) {
                c_id = kCall_jobs;
            } else if (!strcmp(tokens[0], "sig")) {
                c_id = kCall_sig;
                e_id = kExec_parent;
            } else if (!strcmp(tokens[0], "fg")) {
                c_id = kCall_fg;
                e_id = kExec_parent;
            } else if (!strcmp(tokens[0], "bg")) {
                c_id = kCall_bg;
                e_id = kExec_parent;
            }

            // execute command in the background if suffixed with &
            if (command[strlen(command) - 1] == '&' && e_id != kExec_parent) {
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
                    "input_line: %s\n"
                    "e_id: %d\n"
                    "c_id: %d\n"
                    "repeat: %d\n"
                    "token_count: %d\n"
                    "tokens: %s\n"
                    "----------\n",
                    input_line, e_id, c_id, repeat, token_count, join(tokens, token_count, ", "));
            }

            // execute command
            for (int i = 1; i <= repeat; i++) {
                (*_execute[e_id])((*_callback[c_id]), token_count, tokens);
            }
        }

        // write command to history file
        write_history(strip(input_line));
    }

    return 0;
}
