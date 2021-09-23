#include <stdio.h>
#include <stdlib.h>

#include "commands.h"
#include "proc.h"

// execute `jobs` command
int jobs(int argc, char** argv) {
    struct ProcessPool* job = JOB_POOL;
    while (job) {
        struct Process process = get_stats(job->process.pid);
        printf("[%d]\t%s %s\t[%d]\n", job->id, process.pstatus[0] == 'T' ? "Stopped" : "Running",
               job->process.pname, job->process.pid);
        job = job->next;
    }

    exit(0);
}
