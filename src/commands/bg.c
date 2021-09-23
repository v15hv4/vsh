#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "commands.h"
#include "errors.h"
#include "proc.h"

int bg(int argc, char** argv) {
    if (argc < 2) return throw_custom_error("bg: not enough arguments", -1);

    // get job id as input
    int job_id = atoi(argv[1]);

    // get job using id from job pool
    struct ProcessPool job = get_job(job_id);
    if (job.id == -1) return throw_custom_error("bg: invalid job id", -2);

    // resume process in the background
    if (kill(job.process.pid, SIGCONT)) {
        return throw_custom_error("bg: unable to resume job", -3);
    }

    return 0;
}
