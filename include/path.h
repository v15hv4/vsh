#ifndef PATH_H_
#define PATH_H_

// maximum path length
#define PATH_MAX 4096

// get absolute path of input relative path
char* expand_path(char* path);

// shorten input path by replacing home directory with ~
char* shorten_path(char* path);

// get the shell's current working directory path
char* get_current_path();

// set the shell's home path
void set_home_path();

// get the shell's home path
char* get_home_path();

// set the shell's previous working directory path
void set_prev_path(char* path);

// get the shell's previous working directory path
char* get_prev_path();

#endif
