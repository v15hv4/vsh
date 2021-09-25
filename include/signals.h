#ifndef SIGNALS_H_
#define SIGNALS_H_

// install handler for given signal number
void handle_signal(int signal, void (*handler)(int));

// remove child processes that are completed
void reap_zombies(int signal);

// interrupt the current foreground process
void interrupt_fg(int signal);

#endif
