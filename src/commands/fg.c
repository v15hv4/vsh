#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "commands.h"
#include "errors.h"
#include "proc.h"
#include "utils.h"

int fg(int argc, char** argv) {
    if (argc < 2) return throw_custom_error("fg: not enough arguments", -1);

    // get job id as input
    int job_id = atoi(argv[1]);

    // get job using id from job pool
    struct ProcessPool job = get_job(job_id);
    if (job.id == -1) return throw_custom_error("fg: invalid job id", -2);

    // bring process to the foreground
    setpgid(job.process.pid, getpid());  // TODO: set to parent's PGID instead

    // remove process from pool
    remove_process(job.process.pid);

    // resume process if it is stopped
    if (kill(job.process.pid, SIGCONT)) {
        return throw_custom_error("fg: unable to resume job", -3);
    }

    // set current foreground process to child
    CURRENT_FOREGROUND_PROCESS.pid = job.process.pid;
    CURRENT_FOREGROUND_PROCESS.pname = job.process.pname;

    // wait for child to finish execution in the parent process
    int status;
    waitpid(job.process.pid, &status, WUNTRACED);

    // reset current foreground process
    CURRENT_FOREGROUND_PROCESS = (struct Process)PROCESS_DEFAULT;

    return 0;
}
