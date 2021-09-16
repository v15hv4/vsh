#include "pinfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "errors.h"
#include "path.h"
#include "utils.h"

// execute `pinfo`
int pinfo(int argc, char** argv) {
    // get pid as input, default to current process
    pid_t pid = 0;
    if (argc > 1) pid = (pid_t)atoi(argv[1]);
    if (!pid) pid = getpid();

    // initialize process attributes
    int pforeground = 0;
    char* pstatus;
    char* pmemory;
    char* pexecutable = calloc(PATH_MAX, sizeof(char));
    char* proc_path_format = "/proc/%d/%s";

    // generate process stat file path for given pid
    char* stat_path = calloc(strlen(proc_path_format) + 16, sizeof(char));
    sprintf(stat_path, proc_path_format, pid, "stat");

    // read process stats
    FILE* stat_file = fopen(stat_path, "r");
    char* stat_buffer = calloc(STAT_SIZE, sizeof(char));
    if (!stat_file) return throw_blocking_error("pinfo", -1);
    fread(stat_buffer, 1, STAT_SIZE, stat_file);
    char** stat_values = split(stat_buffer, " ");

    pstatus = stat_values[2];                                      // status given by state field
    pforeground = (atoi(stat_values[7]) == atoi(stat_values[4]));  // in foreground if tpgid == pgrp
    pmemory = stat_values[22];  // virtual memory given by vsize field

    // generate process stat file path for given pid
    char* exe_path = calloc(strlen(proc_path_format) + 16, sizeof(char));
    sprintf(exe_path, proc_path_format, pid, "exe");

    // read process exe
    if (readlink(exe_path, pexecutable, PATH_MAX) == -1) {
        pexecutable = "not found";
    } else {
        pexecutable = shorten_path(pexecutable);
    }

    // print process info
    printf(
        "pid -- %d\n"
        "process status -- %s%s\n"
        "memory -- %s\n"
        "executable path -- %s\n",
        pid, pstatus, pforeground ? "+" : "", pmemory, pexecutable);

    return 0;
}
