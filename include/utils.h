#ifndef UTILS_H_
#define UTILS_H_

// ANSI color codes
#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_WHITE "\x1b[37m"
#define ANSI_RESET "\x1b[0m"

// format string with colors
char* colorize(char* color, char* str);

// return number of tokens in string separated by delim
int num_tokens(char* str, char* delim);

// return list of tokens in string separated by delim
char** tokenize(char* str, char* delim);

#endif
