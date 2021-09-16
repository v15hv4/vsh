#include "history.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"

// maintain history info
struct History {
    int size;
    char** entries;
};

// read n entries from history file
struct History read_history(int n) {
    // initialize history entries
    struct History existing = {0, NULL};

    // read entries from history file
    FILE* history_file = fopen(HISTORY_PATH, "r");
    if (history_file) {
        size_t buffer_size = 0;
        char* buffer_line = NULL;
        existing.entries = calloc(HISTORY_MAX + 1, sizeof(char*));
        while ((getline(&buffer_line, &buffer_size, history_file)) != -1) {
            existing.entries[existing.size] = strdup(buffer_line);
            existing.size++;
        }
        fclose(history_file);
    }

    return existing;
}

// write to history file
int write_history(char* command) {
    // read & rewrite existing history file if it exists
    FILE* history_file = fopen(HISTORY_PATH, "r+");
    if (history_file) {
        int existing_count = 0;
        size_t buffer_size = 0;
        char* buffer_line = NULL;
        char** existing_entries = calloc(HISTORY_MAX + 1, sizeof(char*));
        while ((getline(&buffer_line, &buffer_size, history_file)) != -1) {
            existing_entries[existing_count] = strdup(buffer_line);
            existing_count++;
        }

        // truncate file to fit max length
        if (existing_count > HISTORY_MAX - 1) {
            history_file = freopen(HISTORY_PATH, "w", history_file);
            for (int i = 1; i < existing_count; i++) {
                fprintf(history_file, "%s", existing_entries[i]);
            }
        }

    } else {
        history_file = fopen(HISTORY_PATH, "w");
    }

    // write new entry to history file
    fprintf(history_file, "%s\n", command);
    fclose(history_file);

    return 0;
}

// execute `history`
int history(int argc, char** argv) {
    int entry_count = argc < 2 ? HISTORY_MAX : atoi(argv[1]);

    struct History existing = read_history(entry_count);
    for (int i = 0; i < existing.size; i++) {
        printf("%2d  %s", i + 1, existing.entries[i]);
    }
    printf("\n");

    exit(0);
}
