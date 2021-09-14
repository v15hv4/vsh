#ifndef PROC_H_
#define PROC_H_

// execute system process
int sys(char** args);

// execute process in the foreground
int execute_fg(int (*f)(char**), char** args);

// execute process in the background
int execute_bg(int (*f)(char**), char** args);

#endif
