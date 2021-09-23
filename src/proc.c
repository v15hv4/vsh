#include "proc.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "errors.h"
#include "path.h"
#include "utils.h"

// global job pool head
struct ProcessPool* JOB_POOL = NULL;

// get stats of process given by pid
struct Process get_stats(pid_t pid) {
    struct Process target = PROCESS_DEFAULT;
    char* proc_path_format = "/proc/%d/%s";

    // generate process stat file path for given pid
    char* stat_path = calloc(strlen(proc_path_format) + 16, sizeof(char));
    sprintf(stat_path, proc_path_format, pid, "stat");

    // read process stats
    FILE* stat_file = fopen(stat_path, "r");
    char* stat_buffer = calloc(STAT_SIZE, sizeof(char));
    if (!stat_file) return target;

    fread(stat_buffer, 1, STAT_SIZE, stat_file);
    char** stat_values = split(stat_buffer, " ");

    target.pid = pid;
    target.pstatus = stat_values[2];  // status given by state field
    target.pforeground =
        (atoi(stat_values[7]) == atoi(stat_values[4]));  // in foreground if tpgid == pgrp
    target.pvmemory = atoi(stat_values[22]);             // virtual memory given by vsize field

    // generate process stat file path for given pid
    char* exe_path = calloc(strlen(proc_path_format) + 16, sizeof(char));
    sprintf(exe_path, proc_path_format, pid, "exe");

    // read process exe
    target.pexecutable = calloc(PATH_MAX, sizeof(char));
    if (readlink(exe_path, target.pexecutable, PATH_MAX) == -1) {
        target.pexecutable = "not found";
    } else {
        target.pexecutable = shorten_path(target.pexecutable);
    }

    return target;
}

// add job to pool
int add_job(pid_t pid, char* pname) {
    struct Process new_process = PROCESS_DEFAULT;
    new_process.pid = pid;
    new_process.pname = pname;

    struct ProcessPool* new_job = calloc(1, sizeof(struct ProcessPool));
    new_job->id = 1;
    new_job->process = new_process;
    new_job->next = NULL;

    if (!JOB_POOL) {
        JOB_POOL = new_job;
        printf("[1] %d\n", pid);
    } else {
        int job_count = 2;
        struct ProcessPool* job = JOB_POOL;
        while (job->next) {
            job = job->next;
            job_count++;
        }
        new_job->id = job_count;
        job->next = new_job;
        printf("[%d] %d\n", job_count, pid);
    }

    return 0;
}

// remove job from pool
struct Process remove_job(pid_t pid) {
    struct Process process = PROCESS_DEFAULT;

    if (JOB_POOL->process.pid == pid) {
        process = JOB_POOL->process;
        JOB_POOL = JOB_POOL->next;
    } else {
        struct ProcessPool* job = JOB_POOL;
        while (job->next) {
            if (job->next->process.pid == pid) {
                struct ProcessPool* target = job->next;
                job->next = job->next->next;
                process = target->process;
                break;
            }
            job = job->next;
        }
    }

    return process;
}

// clear all jobs from pool
int clear_jobs() {
    struct ProcessPool* job = JOB_POOL;
    while (job) {
        killpg(job->process.pid, SIGKILL);
        job = job->next;
    }
    return 0;
}

// execute process in the foreground
int execute_foreground(int (*f)(int, char**), int argc, char** argv) {
    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        // execute command in the child process
        (*f)(argc, argv);
    } else {
        // wait for child to finish execution in the parent process
        wait(NULL);
    }

    return 0;
}

// execute process in the background
int execute_background(int (*f)(int, char**), int argc, char** argv) {
    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        // create a new process group for the child
        setpgid(0, 0);

        // execute command in the child process
        (*f)(argc, argv);
    } else {
        // maintain job info in the parent's job pool
        add_job(pid, join(argv, argc, " "));
    }

    return 0;
}

// execute process in the parent
int execute_parent(int (*f)(int, char**), int argc, char** argv) {
    (*f)(argc, argv);

    return 0;
}

// execute system process
int sys(int argc, char** argv) {
    if (execvp(argv[0], argv)) {
        char* errmsg = calloc(128, sizeof(char));
        sprintf(errmsg, "%s", argv[0]);
        throw_blocking_error(errmsg, -1);
        exit(1);
    }

    return 0;
}

