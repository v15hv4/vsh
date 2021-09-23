#include <signal.h>
#include <stdlib.h>

#include "commands.h"
#include "errors.h"
#include "proc.h"

int sig(int argc, char** argv) {
    if (argc < 3) throw_custom_error("sig: not enough arguments", -1);

    // get job id and signal number as input
    int job_id = atoi(argv[1]);
    int signum = atoi(argv[2]);

    // get job using id from job pool
    struct ProcessPool job = get_job(job_id);
    if (job.id == -1) throw_custom_error("sig: invalid job id", -2);
    printf("target pid: %d\n", job.process.pid);

    // send signal to target process
    if (kill(job.process.pid, signum)) {
        throw_custom_error("sig", -3);
    }

    return 0;
}
