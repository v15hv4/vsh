#ifndef BUILTINS_H_
#define BUILTINS_H_

// execute shell builtin `cd`
int __cd(int argc, char** argv);

// execute shell builtin `pwd`
int __pwd(int argc, char** argv);

// execute shell builtin `echo`
int __echo(int argc, char** argv);

// execute shell builtin `exit`
int __exit(int argc, char** argv);

#endif
