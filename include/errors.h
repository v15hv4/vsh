#ifndef ERRORS_H_
#define ERRORS_H_

// display message and exit
void throw_fatal_error(char* message);

// display message and return error code
int throw_blocking_error(char* message, int errcode);

#endif
