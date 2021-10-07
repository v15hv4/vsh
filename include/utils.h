#ifndef UTILS_H_
#define UTILS_H_

// ANSI color codes
#define ANSI_RED "\e[31m"
#define ANSI_GREEN "\e[32m"
#define ANSI_YELLOW "\e[33m"
#define ANSI_BLUE "\e[34m"
#define ANSI_MAGENTA "\e[35m"
#define ANSI_CYAN "\e[36m"
#define ANSI_WHITE "\e[37m"
#define ANSI_RESET "\e[0m"

// whitespace characters
#define WHITESPACE " \t\r\n\v\f"

// format string with colors
char* colorize(char* color, char* str);

// strip leading and trailing whitespace from string
char* strip(char* str);

// return number of tokens in string separated by delim
int num_tokens(char* str, char* delim);

// return list of tokens in string separated by delim
char** split(char* str, char* delim);

// return string made by concatenating array elements with delim
char* join(char** arr, int arr_length, char* delim);

// open file and return fd for redirecting from/to it
int redirect(char mode, char* command, char** tokens, int* token_count);

#endif
