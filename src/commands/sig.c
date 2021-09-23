#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "errors.h"
#include "proc.h"

int sig(int argc, char** argv) {
    if (argc < 3) return throw_custom_error("sig: not enough arguments", -1);

    // get job id and signal number as input
    int job_id = atoi(argv[1]);
    int signum = atoi(argv[2]);

    // get job using id from job pool
    struct ProcessPool job = get_job(job_id);
    if (job.id == -1) return throw_custom_error("sig: invalid job id", -2);

    // send signal to target process
    if (kill(job.process.pid, signum)) {
        char* error_format = "sig: unable to send signal %d to process with pid %d";
        char* error_msg = calloc(2 * strlen(error_format), sizeof(char));
        sprintf(error_msg, error_format, signum, job.process.pid);
        return throw_custom_error(error_msg, -3);
    }

    return 0;
}
