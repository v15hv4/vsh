#ifndef HISTORY_H_
#define HISTORY_H_

// maximum num of commands to track
#define HISTORY_MAX 20

// path to history file
#define HISTORY_PATH "/tmp/.vsh_history"

// maintain history info
struct History {
    int size;
    char** entries;
};

// maintain current history cache to minimize disk reads
static struct History HISTORY_CACHE;

// read all entries from history file
struct History read_history();

// write to history file
int write_history(char* command);

// read history cache
struct History read_history_cache();

// refresh history cache
void refetch_history_cache();

// execute `history`
int history(int argc, char** argv);

#endif
