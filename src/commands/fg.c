#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "commands.h"
#include "errors.h"
#include "proc.h"

int fg(int argc, char** argv) {
    if (argc < 2) return throw_custom_error("fg: not enough arguments", -1);

    // get job id as input
    int job_id = atoi(argv[1]);

    // get job using id from job pool
    struct ProcessPool job = get_job(job_id);
    if (job.id == -1) return throw_custom_error("fg: invalid job id", -2);

    // bring process to the foreground
    setpgid(job.process.pid, getpid());

    // remove process from pool
    remove_process(job.process.pid);

    // wait until process finishes
    wait(NULL);

    return 0;
}
