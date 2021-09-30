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

// global current foreground process
struct Process CURRENT_FOREGROUND_PROCESS = PROCESS_DEFAULT;

// global job pool
struct ProcessPool* JOB_POOL = NULL;
int JOB_COUNT = 0;

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

// add process to job pool
int add_process(pid_t pid, char* pname) {
    struct Process new_process = PROCESS_DEFAULT;
    new_process.pid = pid;
    new_process.pname = pname;

    struct ProcessPool* new_job = calloc(1, sizeof(struct ProcessPool));
    new_job->id = 1;
    new_job->process = new_process;
    new_job->next = NULL;

    if (!JOB_POOL) {
        JOB_POOL = new_job;
    } else {
        struct ProcessPool* job = JOB_POOL;
        while (job->next) {
            // increment new job's id if it already exists in pool
            if (new_job->id == job->id) (new_job->id)++;
            job = job->next;
        }
        // increment new job's id if it is the latest job's id
        if (new_job->id == job->id) (new_job->id)++;
        job->next = new_job;
    }
    printf("[%d] %d\n", new_job->id, pid);

    JOB_COUNT++;

    return 0;
}

// remove process from job pool
struct Process remove_process(pid_t pid) {
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

    JOB_COUNT--;

    return process;
}

// get job given by id from pool
struct ProcessPool get_job(int job_id) {
    struct ProcessPool target = PROCESSPOOL_DEFAULT;

    struct ProcessPool* job = JOB_POOL;
    while (job) {
        if (job->id == job_id) {
            target = *job;
            break;
        }
        job = job->next;
    }

    return target;
}

// clear all jobs from pool
int clear_jobs() {
    struct ProcessPool* job = JOB_POOL;
    while (job) {
        killpg(job->process.pid, SIGKILL);
        job = job->next;
    }

    JOB_COUNT = 0;

    return 0;
}

// execute process in the foreground
int execute_foreground(int (*f)(int, char**), int argc, char** argv, int in_fd, int out_fd) {
    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        // set io file descriptors
        dup2(in_fd, STDIN_FILENO);
        dup2(out_fd, STDOUT_FILENO);

        // execute command in the child process
        (*f)(argc, argv);
    } else {
        // set current foreground process to child
        CURRENT_FOREGROUND_PROCESS.pid = pid;
        CURRENT_FOREGROUND_PROCESS.pname = join(argv, argc, " ");

        // close io file descriptors
        if (in_fd != STDIN_FILENO) close(in_fd);
        if (out_fd != STDOUT_FILENO) close(out_fd);

        // wait for child to finish execution in the parent process
        int status;
        waitpid(pid, &status, WUNTRACED);

        // reset current foreground process
        CURRENT_FOREGROUND_PROCESS = (struct Process)PROCESS_DEFAULT;
    }

    return 0;
}

// execute process in the background
int execute_background(int (*f)(int, char**), int argc, char** argv, int in_fd, int out_fd) {
    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        // create a new process group for the child
        setpgid(0, 0);

        // execute command in the child process
        (*f)(argc, argv);
    } else {
        // maintain job info in the parent's job pool
        add_process(pid, join(argv, argc, " "));
    }

    return 0;
}

// execute process in the parent
int execute_parent(int (*f)(int, char**), int argc, char** argv, int in_fd, int out_fd) {
    (*f)(argc, argv);

    return 0;
}

// execute system process
int sys(int argc, char** argv) {
    char** argv_nt = calloc(argc + 1, sizeof(char*));
    for (int i = 0; i < argc; i++) argv_nt[i] = argv[i];
    argv_nt[argc] = NULL;

    if (execvp(argv_nt[0], argv_nt)) {
        char* errmsg = calloc(128, sizeof(char));
        sprintf(errmsg, "%s", argv_nt[0]);
        throw_blocking_error(errmsg, -1);
        exit(1);
    }

    return 0;
}

