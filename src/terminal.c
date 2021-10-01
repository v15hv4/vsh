#include "terminal.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "errors.h"
#include "history.h"
#include "proc.h"
#include "prompt.h"

// default terminal structure
struct termios terminal;

// disable raw mode
void disable_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminal) == -1) {
        return throw_fatal_error("terminal: can't disable raw mode");
    }
}

// enable raw terminal mode
void enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &terminal) == -1) {
        return throw_fatal_error("terminal: can't get attributes");
    }
    atexit(disable_raw_mode);
    struct termios raw = terminal;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        return throw_fatal_error("terminal: can't enable raw mode");
    }
}

// handle Return keypress
void handle_line_feed(int* buffer_open) {
    printf("\n");
    *buffer_open = 0;
}

// handle arrow keys
void handle_arrow_keys(char* input_buffer, int* cursor, int* history_idx) {
    struct History cache = read_history_cache();
    char es_buffer[3];
    es_buffer[2] = 0;
    if (read(STDIN_FILENO, es_buffer, 2) == 2) {
        if (es_buffer[1] == 'A') {
            // move history prev
            if (cache.size - 1 >= *history_idx) {
                (*history_idx)++;

                // extract target entry
                char* entry = cache.entries[cache.size - *history_idx];

                // print history entry
                printf("\e[%dD\e[K%s", *cursor ? *cursor : -1, entry);

                // determine new cursor position
                (*cursor) = 0;
                for (int i = 0; i < strlen(entry); i++) {
                    if (entry[i] == '\t') (*cursor)++;
                    (*cursor)++;
                }

                // set new input buffer
                strcpy(input_buffer, entry);
            }
        } else if (es_buffer[1] == 'B') {
            // move history next
            if (*history_idx > 1) {
                (*history_idx)--;

                // extract target entry
                char* entry = cache.entries[cache.size - *history_idx];

                // print history entry
                memset(input_buffer, '\0', INPUT_MAX);
                printf("\e[%dD\e[K%s", *cursor ? *cursor : -1, entry);

                // determine new cursor position
                (*cursor) = 0;
                for (int i = 0; i < strlen(entry); i++) {
                    if (entry[i] == '\t') (*cursor)++;
                    (*cursor)++;
                }

                // set new input buffer
                memset(input_buffer, '\0', INPUT_MAX);
                strcpy(input_buffer, entry);
            } else {
                (*history_idx) = 0;
                printf("\e[%dD\e[K", *cursor ? *cursor : -1);
                (*cursor) = 0;
                memset(input_buffer, '\0', INPUT_MAX);
                strcpy(input_buffer, "");
            }
        } else if (es_buffer[1] == 'C') {
            // move cursor right
            if (*cursor <= (int)strlen(input_buffer) - 1) {
                // traverse a tab
                if (input_buffer[*cursor] == 9) {
                    printf("\e[8C");
                } else {
                    printf("\e[1C");
                }
                (*cursor)++;
            }
        } else if (es_buffer[1] == 'D') {
            // move cursor left
            if (*cursor > 0) {
                // traverse a tab
                if (input_buffer[*cursor - 1] == 9) {
                    printf("\e[8D");
                } else {
                    printf("\e[1D");
                }
                (*cursor)--;
            }
        } else {
            /* printf("%c", es_buffer[1]); */
        }
    }
}

// handle Backspace keypress
void handle_backspace(char* input_buffer, int* cursor) {
    if (*cursor > 0) {
        // remove a tab
        if (input_buffer[*cursor - 1] == 9) {
            printf("\b\b\b\b\b\b\b");
        }
        // terminate buffer if cursor is at the end
        if (*cursor == strlen(input_buffer)) {
            input_buffer[--(*cursor)] = '\0';
        } else {
            --(*cursor);
        }
        printf("\b \b");
    }
}

// handle Tab
void handle_tab(char* input_buffer, int* cursor, char input_char) {
    input_buffer[(*cursor)++] = input_char;
    printf("        ");
}

// handle clear
void handle_clear(char* input_buffer, int* cursor) {
    input_buffer[0] = '\0';
    (*cursor) = 0;
    printf("\e[1;1H\e[2J");
    print_prompt();
}

// handle exit
void handle_exit(char* input_buffer) {
    // kill all children
    clear_jobs();

    free(input_buffer);
    printf("\nexit\n");
    exit(0);
}

// handle standard input
void handle_standard(char* input_buffer, int* cursor, char input_char) {
    if (*cursor == strlen(input_buffer)) {
        input_buffer[(*cursor)++] = input_char;
        printf("%c", input_char);
    } else {
        for (int i = (int)strlen(input_buffer); i >= *cursor; i--) {
            input_buffer[i + 1] = input_buffer[i];
        }
        input_buffer[(*cursor)++] = input_char;
        if ((*cursor) - 1) {
            printf("\e[%dD\e[K%s\e[%dD", (*cursor) - 1, input_buffer,
                   (int)strlen(input_buffer) - (*cursor));
        } else {
            printf("\e[K%s\e[%dD", input_buffer, (int)strlen(input_buffer) - (*cursor));
        }
    }
}

// get raw terminal input
char* get_raw_input() {
    int cursor = 0;
    char input_char;
    char* input_buffer = calloc(INPUT_MAX, sizeof(char));

    // control buffer
    int buffer_open = 1;

    // control history
    int history_idx = 0;

    enable_raw_mode();
    memset(input_buffer, '\0', INPUT_MAX);
    while (buffer_open && (read(STDIN_FILENO, &input_char, 1) == 1)) {
        if (iscntrl(input_char)) {
            // handle control characters
            if (input_char == 10) {
                // line feed
                handle_line_feed(&buffer_open);
            } else if (input_char == 27) {
                // arrow keys
                handle_arrow_keys(input_buffer, &cursor, &history_idx);
            } else if (input_char == 127) {
                // backspace
                handle_backspace(input_buffer, &cursor);
            } else if (input_char == 9) {
                // tab
                handle_tab(input_buffer, &cursor, input_char);
            } else if (input_char == 12) {
                // clear
                handle_clear(input_buffer, &cursor);
            } else if (input_char == 4) {
                // exit
                handle_exit(input_buffer);
            } else {
                // default behavior
                /* handle_standard(input_buffer, &cursor, input_char); */
            }
        } else {
            // handle normal characters
            handle_standard(input_buffer, &cursor, input_char);
        }
    }
    disable_raw_mode();

    return input_buffer;
}

// get default terminal input
char* get_default_input() {
    size_t input_size = 0;
    char* input_buffer = NULL;
    if (getline(&input_buffer, &input_size, stdin) == -1) {
        handle_exit(input_buffer);
    };

    return input_buffer;
}
