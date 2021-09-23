#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "proc.h"

// comparator function to alphabetically sort jobs
int job_comparator(const void* a, const void* b) {
    return strcmp(((struct ProcessPool*)a)->process.pname, ((struct ProcessPool*)b)->process.pname);
}

// execute `jobs` command
int jobs(int argc, char** argv) {
    if (JOB_COUNT) {
        // initialize empty job list
        struct ProcessPool job_list[JOB_COUNT];

        // add running jobs to list
        int job_idx = 0;
        struct ProcessPool* job = JOB_POOL;
        while (job) {
            job_list[job_idx] = *job;
            job = job->next;
            job_idx++;
        }

        // sort jobs by name
        qsort(&job_list, JOB_COUNT, sizeof(struct ProcessPool), job_comparator);

        // print job list
        for (int i = 0; i < JOB_COUNT; i++) {
            struct Process process = get_stats(job_list[i].process.pid);
            printf("[%d]\t%s %s\t[%d]\n", job_list[i].id,
                   process.pstatus[0] == 'T' ? "Stopped" : "Running", job_list[i].process.pname,
                   job_list[i].process.pid);
        }
    }
    exit(0);
}
