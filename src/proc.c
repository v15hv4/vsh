#include "proc.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "errors.h"
#include "utils.h"

// maintain process info
struct Process {
    pid_t pid;
    char* pname;
};

// maintain dynamic process pool
struct ProcessPool {
    struct Process process;
    struct ProcessPool* next;
}* job_pool = NULL;

// add job to pool
int add_job(pid_t pid, char* pname) {
    struct Process new_process = {pid, pname};
    struct ProcessPool* new_job = calloc(1, sizeof(struct ProcessPool));
    new_job->process = new_process;
    new_job->next = NULL;

    if (!job_pool) {
        job_pool = new_job;
        printf("[1] %d\n", pid);
    } else {
        int job_count = 2;
        struct ProcessPool* iter = job_pool;
        while (iter->next) {
            iter = iter->next;
            job_count++;
        }
        iter->next = new_job;
        printf("[%d] %d\n", job_count, pid);
    }

    return 0;
}

// remove job from pool
int remove_job(pid_t pid) {
    if (job_pool->process.pid == pid) {
        job_pool = job_pool->next;
    } else {
        struct ProcessPool* iter = job_pool;
        while (iter->next) {
            if (iter->next->process.pid == pid) {
                struct ProcessPool* target = iter->next;
                iter->next = iter->next->next;
                free(target);
                break;
            }
            iter = iter->next;
        }
    }

    return 0;
}

// print current job list
int jobs(int argc, char** argv) {
    int job_count = 1;
    struct ProcessPool* iter = job_pool;
    while (iter) {
        printf("[%d]\t%d\t%s\n", job_count, iter->process.pid, iter->process.pname);
        iter = iter->next;
        job_count++;
    }

    exit(0);
}

// execute process in the foreground
int execute_foreground(int (*f)(int, char**), int argc, char** argv) {
    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        // child
        (*f)(argc, argv);
    } else {
        // parent
        wait(NULL);
    }

    return 0;
}

// execute process in the background
int execute_background(int (*f)(int, char**), int argc, char** argv) {
    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        // child
        setpgid(0, 0);
        (*f)(argc, argv);
    } else {
        // parent
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

