#ifndef HISTORY_H_
#define HISTORY_H_

// maximum num of commands to track
#define HISTORY_MAX 20

// path to history file
#define HISTORY_PATH "/tmp/.vsh_history"

// write to history file
int write_history(char* command);

// read n entries from history file
char* read_history(int n);

// execute `history`
int history(int argc, char** argv);

#endif
