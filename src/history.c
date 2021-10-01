#include "history.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"

// maintain current history cache to minimize disk reads
struct History HISTORY_CACHE;

// read all entries from history file
struct History read_history() {
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
            existing.entries[existing.size][strlen(existing.entries[existing.size]) - 1] = '\0';
            existing.size++;
        }
        fclose(history_file);
    }

    return existing;
}

// write to history file
int write_history(char* command) {
    // don't add command to history if it's the same as previous
    struct History cache = read_history_cache();
    if (cache.size > 0 && !strcmp(cache.entries[cache.size - 1], command)) return 0;

    // preprocess command
    char* processed_command = calloc(8 * strlen(command) + 2, sizeof(char));
    int processed_command_length = 0;
    for (int i = 0; i < strlen(command); i++) {
        if (command[i] == '\t') {
            strcat(processed_command, "        ");
            processed_command_length += 8;
        } else {
            char c[2] = {command[i], '\0'};
            strcat(processed_command, c);
            processed_command_length++;
        }
    }
    processed_command[processed_command_length] = '\0';

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
    fprintf(history_file, "%s\n", processed_command);
    fclose(history_file);

    // refresh history cache
    refetch_history_cache();

    return 0;
}

// read history cache
struct History read_history_cache() {
    return HISTORY_CACHE;
}

// refresh history cache
void refetch_history_cache() { HISTORY_CACHE = read_history(); };

// execute `history`
int history(int argc, char** argv) {
    struct History existing = read_history();
    int entry_count = argc < 2 ? existing.size : atoi(argv[1]);

    // print input number of latest history entries
    for (int i = existing.size - entry_count; i < existing.size; i++) {
        printf("%2d  %s\n", i + 1, existing.entries[i]);
    }

    return 0;
}

