#ifndef HISTORY_H_
#define HISTORY_H_

// maximum num of commands to track
#define HISTORY_MAX 20

// path to history file
#define HISTORY_PATH "/tmp/.vsh_history"

// write to history file
int write_history(char* command);

// read all entries from history file
struct History read_history();

// execute `history`
int history(int argc, char** argv);

#endif
