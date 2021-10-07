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
    int DEBUG = 1;

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
    int (*_execute[])(int (*)(int, char**), int, char**, int, int) = {
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
            // declare pipes
            int pipe_fds[2][2];
            enum pipe_actions { READ, WRITE };

            // parse & execute piped commands
            int pipe_count = num_tokens(commands[i], "|");
            char** pipes = split(commands[i], "|");

            for (int j = 0; j < pipe_count; j++) {
                // open pipe
                if (pipe(pipe_fds[j % 2]) == -1) {
                    return throw_blocking_error("pipe", -1);
                }

                // current command
                char* command = strip(pipes[j]);

                // command properties
                int repeat = 0;                        // number of times to repeat command
                enum execute e_id = kExec_foreground;  // execution layer id
                enum callback c_id = kCall_sys;        // callback id

                // tokenize command
                int token_count = num_tokens(command, WHITESPACE);
                char** tokens = split(command, WHITESPACE);

                // determine number of times to execute command
                while (!strcmp(tokens[0], "repeat")) {
                    repeat += atoi(tokens[1]);

                    // if `repeat` syntax is valid
                    if (repeat && token_count > 2) {
                        // reassign to tokens of actual command
                        tokens = &tokens[2];
                        token_count -= 2;
                    } else {
                        repeat = 1;
                    }
                }
                if (!repeat) repeat = 1;

                // determine callback enum
                if (!strcmp(tokens[0], "cd")) {
                    c_id = kCall_cd;
                    e_id = kExec_parent;
                } else if (!strcmp(tokens[0], "exit")) {
                    c_id = kCall_exit;
                    e_id = kExec_parent;
                } else if (!strcmp(tokens[0], "sig")) {
                    c_id = kCall_sig;
                    e_id = kExec_parent;
                } else if (!strcmp(tokens[0], "fg")) {
                    c_id = kCall_fg;
                    e_id = kExec_parent;
                } else if (!strcmp(tokens[0], "bg")) {
                    c_id = kCall_bg;
                    e_id = kExec_parent;
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

                // determine io file descriptors based on pipes and redirections
                int in_fd, out_fd;

                // redirect from file
                if (num_tokens(command, "<") > 1) {
                    in_fd = redirect('r', command, tokens, &token_count);
                } else {
                    in_fd = j ? pipe_fds[!(j % 2)][READ] : STDIN_FILENO;
                }

                // redirect to file
                if (num_tokens(command, ">>") > 1) {
                    out_fd = redirect('a', command, tokens, &token_count);
                } else if (num_tokens(command, ">") > 1) {
                    out_fd = redirect('w', command, tokens, &token_count);
                } else {
                    out_fd = j < pipe_count - 1 ? pipe_fds[j % 2][WRITE] : STDOUT_FILENO;
                }

                // DEBUG OUTPUT
                if (DEBUG) {
                    fprintf(stderr,
                            "-----------------------\n"
                            "input_line: %s\n"
                            "e_id: %d\n"
                            "c_id: %d\n"
                            "repeat: %d\n"
                            "token_count: %d\n"
                            "tokens: %s\n"
                            "pipe_count: %d\n"
                            "i: %d, j: %d\n"
                            "\nnow executing: %s\n"
                            "-----------------------\n",
                            input_line, e_id, c_id, repeat, token_count,
                            join(tokens, token_count, ", "), pipe_count, i, j, command);
                }

                // execute command
                for (int c = 1; c <= repeat; c++) {
                    (*_execute[e_id])((*_callback[c_id]), token_count, tokens, in_fd, out_fd);
                }
            }
        }

        // write command to history file
        write_history(strip(input_line));
    }

    return 0;
}
