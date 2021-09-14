#ifndef PROC_H_
#define PROC_H_

// execute system process
int sys(int argc, char** argv);

// execute process in the foreground
int execute_fg(int (*f)(int, char**), int argc, char** argv);

// execute process in the background
int execute_bg(int (*f)(int, char**), int argc, char** argv);

#endif
