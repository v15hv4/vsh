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

// maintain dynamic process pool
struct ProcessPool {
    int id;
    struct Process process;
    struct ProcessPool* next;
};

// global job pool
extern struct ProcessPool* JOB_POOL;
extern int JOB_COUNT;

// default process info
static const struct Process PROCESS_DEFAULT = {-1, "", "", "", 0, 0};

// default process pool info
static const struct ProcessPool PROCESSPOOL_DEFAULT = {-1, {}, NULL};

// get stats of process given by pid
struct Process get_stats(pid_t pid);

// add job to process pool
int add_job(pid_t pid, char* pname);

// remove job from pool
struct Process remove_job(pid_t pid);

// get job given by id from pool
struct ProcessPool get_job(int job_id);

// clear all jobs from pool
int clear_jobs();

// print current job list
int jobs(int argc, char** argv);

// execute process in the foreground
int execute_foreground(int (*f)(int, char**), int argc, char** argv);

// execute process in the background
int execute_background(int (*f)(int, char**), int argc, char** argv);

// execute process in the parent
int execute_parent(int (*f)(int, char**), int argc, char** argv);

// execute system process
int sys(int argc, char** argv);

#endif
