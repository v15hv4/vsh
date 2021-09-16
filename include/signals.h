#ifndef SIGNALS_H_
#define SIGNALS_H_

// install handler for given signal number
void handle_signal(int signal, void (*handler)(int));

// remove child processes that are completed
void reap_zombies(int signal);

// take lite
void do_nothing(int signal);

// clear current line
void clear_line(int signal);

#endif
