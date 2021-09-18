#ifndef PROC_H_
#define PROC_H_

// maintain process info
struct Process {
    int pid;
    char* pname;
};

// add job to process pool
int add_job(int pid, char* pname);

// remove job from pool
struct Process remove_job(int pid);

// clear all jobs from pool
int clear_jobs();

// execute process in the foreground
int execute_foreground(int (*f)(int, char**), int argc, char** argv);

// execute process in the background
int execute_background(int (*f)(int, char**), int argc, char** argv);

// execute process in the parent
int execute_parent(int (*f)(int, char**), int argc, char** argv);

// print current job list
int jobs(int argc, char** argv);

// execute system process
int sys(int argc, char** argv);

#endif
