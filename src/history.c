#include "history.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"

// write to history file
int write_history(char* command) {
    // preprocess new entry
    char* entry_format = "%d:%s\n";
    char* history_entry = calloc(strlen(entry_format) + strlen(command) + 16, sizeof(char));
    sprintf(history_entry, entry_format, (int)time(NULL), command);

    // initialize existing history entries
    char** existing_entries = NULL;

    // read & rewrite existing history file if it exists
    FILE* history_file = fopen(HISTORY_PATH, "r+");
    if (history_file) {
        int existing_count = 0;
        size_t buffer_size = 0;
        char* buffer_line = NULL;
        existing_entries = calloc(HISTORY_MAX + 1, sizeof(char*));
        while ((getline(&buffer_line, &buffer_size, history_file)) != -1) {
            existing_entries[existing_count] = strdup(buffer_line);
            existing_count++;
        }

        // truncate file to fit max length
        if (existing_count > HISTORY_MAX - 1) {
            rewind(history_file);
            for (int i = 1; i < existing_count; i++) {
                fprintf(history_file, "%s", existing_entries[i]);
            }
        }

    } else {
        history_file = fopen(HISTORY_PATH, "w");
    }

    // write new entry to history file
    fprintf(history_file, "%s", history_entry);
    fclose(history_file);

    return 0;
}

// read n entries from history file
char* read_history(int n) {
    // initialize history entries
    char* entries = NULL;

    // read entries from history file
    FILE* history_file = fopen(HISTORY_PATH, "r");
    if (history_file) {
        fseek(history_file, 0, SEEK_END);
        int history_size = ftell(history_file);
        char* history_buffer = calloc(2 * history_size, sizeof(char));
        rewind(history_file);
        fread(history_buffer, history_size, 1, history_file);
        fclose(history_file);

        // return either n or max number of history entries
        int entry_count = num_tokens(history_buffer, "\n");
        int required_count = entry_count > n ? n : entry_count;
        if (required_count < 1) required_count = HISTORY_MAX;

        char** entry_lines = split(history_buffer, "\n");
        return join(&entry_lines[entry_count - required_count], required_count, "\n");
    }

    return entries;
}

// execute `history`
int history(int argc, char** argv) {
    char* history_entries = NULL;
    int entry_count = argc < 2 ? HISTORY_MAX : atoi(argv[1]);

    history_entries = read_history(entry_count);
    if (history_entries) {
        printf("%s\n", history_entries);
    }

    exit(0);
}

