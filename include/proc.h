#ifndef PROC_H_
#define PROC_H_

#include <signal.h>
#include <stdio.h>

// can't get filesize from /proc so explicitly define stat buffer size
#define STAT_SIZE 1024

// maintain process info
struct Process {
    pid_t pid;
    char* pname;
    char* pstatus;
    char* pexecutable;
    int pvmemory;
    int pforeground;
};

// default process info
#define PROCESS_DEFAULT \
    { -1, "", "", "", 0, 0 }

// maintain dynamic process pool
struct ProcessPool {
    int id;
    struct Process process;
    struct ProcessPool* next;
};

// default process pool info
#define PROCESSPOOL_DEFAULT \
    { -1, PROCESS_DEFAULT, NULL }

// global current foreground process
extern struct Process CURRENT_FOREGROUND_PROCESS;

// global job pool
extern struct ProcessPool* JOB_POOL;
extern int JOB_COUNT;

// get stats of process given by pid
struct Process get_stats(pid_t pid);

// add job to process pool
int add_process(pid_t pid, char* pname);

// remove job from pool
struct Process remove_process(pid_t pid);

// get job given by id from pool
struct ProcessPool get_job(int job_id);

// clear all jobs from pool
int clear_jobs();

// print current job list
int jobs(int argc, char** argv);

// execute process in the foreground
int execute_foreground(int (*f)(int, char**), int argc, char** argv, int in_fd, int out_fd);

// execute process in the background
int execute_background(int (*f)(int, char**), int argc, char** argv, int in_fd, int out_fd);

// execute process in the parent
int execute_parent(int (*f)(int, char**), int argc, char** argv, int in_fd, int out_fd);

// execute system process
int sys(int argc, char** argv);

#endif
