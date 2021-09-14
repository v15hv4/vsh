#ifndef BUILTINS_H_
#define BUILTINS_H_

// execute shell builtin `cd`
int cd(int argc, char** argv);

// execute shell builtin `pwd`
int pwd(int argc, char** argv);

// execute shell builtin `echo`
int echo(int argc, char** argv);

#endif
