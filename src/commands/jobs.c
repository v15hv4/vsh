#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

        // set display flags
        int r_flag = 0;  // show only running jobs
        int s_flag = 0;  // show only stopped jobs

        // fetch flags from input string
        int opt;
        while ((opt = getopt(argc, argv, ":rs")) != -1) {
            if (opt == 'r') r_flag = 1;
            if (opt == 's') s_flag = 1;
        }

        // default to showing all jobs if no flags provided
        if (!r_flag && !s_flag) r_flag = s_flag = 1;

        // add current jobs to list
        int job_list_size = 0;
        struct ProcessPool* job = JOB_POOL;
        while (job) {
            // determine status of process
            job->process.pstatus = get_stats(job->process.pid).pstatus;
            if ((job->process.pstatus[0] == 'T' && s_flag) ||
                (job->process.pstatus[0] != 'T' && r_flag)) {
                job_list[job_list_size] = *job;
                job_list_size++;
            }
            job = job->next;
        }

        // sort jobs by name
        qsort(&job_list, job_list_size, sizeof(struct ProcessPool), job_comparator);

        // print job list
        for (int i = 0; i < job_list_size; i++) {
            printf("[%d]\t%s %s\t[%d]\n", job_list[i].id,
                   job_list[i].process.pstatus[0] == 'T' ? "Stopped" : "Running",
                   job_list[i].process.pname, job_list[i].process.pid);
        }
    }

    return 0;
}
