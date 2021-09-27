#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "commands.h"
#include "errors.h"
#include "path.h"
#include "proc.h"

// execute `pinfo`
int pinfo(int argc, char** argv) {
    // get pid as input, default to current process
    pid_t pid = 0;
    if (argc > 1) pid = (pid_t)atoi(argv[1]);
    if (!pid) pid = getpid();

    struct Process process = get_stats(pid);
    if (process.pid == -1) return throw_custom_error("pinfo: invalid process id", -1);

    // print process info
    printf(
        "pid -- %d\n"
        "process status -- %s%s\n"
        "memory -- %d\n"
        "executable path -- %s\n",
        process.pid, process.pstatus, process.pforeground ? "+" : "", process.pvmemory,
        process.pexecutable);

    return 0;
}
