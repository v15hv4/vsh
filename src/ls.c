#include "ls.h"

#include <dirent.h>
#include <linux/limits.h>
#include <math.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "errors.h"
#include "utils.h"

// execute `ls`
int ls(int argc, char** argv) {
    int flag_count = 0;
    int dir_count = 0;

    char** flags = calloc(argc - 1, sizeof(char*));
    char** dirs = calloc(argc - 1, sizeof(char*));

    // separate flags and dirs from input
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            flags[flag_count++] = &argv[i][1];
        } else {
            dirs[dir_count++] = argv[i];
        }
    }

    // set display flags
    int a_flag = 0;  // show hidden files & directories
    int l_flag = 0;  // print in long listing format

    for (int i = 0; i < flag_count; i++) {
        for (int j = 0; j < strlen(flags[i]); j++) {
            if (flags[i][j] == 'a') {
                a_flag = 1;
            } else if (flags[i][j] == 'l') {
                l_flag = 1;
            }
        }
        if (a_flag && l_flag) break;
    }

    // set ls dir to cwd if no dirs given
    if (!dir_count) dirs[dir_count++] = ".";

    // list files & directories
    for (int i = 0; i < dir_count; i++) {
        if (dir_count > 1) printf("%s:\n", dirs[i]);

        struct dirent* entry;
        DIR* dir = opendir(dirs[i]);
        if (!dir) return throw_blocking_error("ls", -1);

        // count number of entries in target directory
        int entry_count = 0;
        while ((entry = readdir(dir))) {
            // ignore hidden files if -a is not set
            if (!a_flag && entry->d_name[0] == '.') continue;
            entry_count++;
        }

        // initialize output columns
        char** col_type_perms = calloc(entry_count, sizeof(char*));
        char** col_hls = calloc(entry_count, sizeof(char*));
        char** col_owner = calloc(entry_count, sizeof(char*));
        char** col_group = calloc(entry_count, sizeof(char*));
        char** col_size = calloc(entry_count, sizeof(char*));
        char** col_modified = calloc(entry_count, sizeof(char*));
        char** col_name_link = calloc(entry_count, sizeof(char*));

        // initialize maximum size of variable length columns
        int max_hls_width = 1;
        int max_owner_width = 1;
        int max_group_width = 1;
        int max_size_width = 1;

        // count total block size
        double total_block_size = 0;

        int entry_iter = 0;
        rewinddir(dir);
        while ((entry = readdir(dir))) {
            // ignore hidden files if -a is not set
            if (!a_flag && entry->d_name[0] == '.') continue;

            struct stat stats;
            char* path = calloc(strlen(dirs[i]) + strlen(entry->d_name) + 2, sizeof(char));
            strcat(path, dirs[i]);
            strcat(path, "/");
            strcat(path, entry->d_name);

            if (lstat(path, &stats) == 0) {
                // determine file type
                char* file_type = S_ISREG(stats.st_mode)    ? "-"
                                  : S_ISLNK(stats.st_mode)  ? "l"
                                  : S_ISDIR(stats.st_mode)  ? "d"
                                  : S_ISCHR(stats.st_mode)  ? "c"
                                  : S_ISBLK(stats.st_mode)  ? "b"
                                  : S_ISFIFO(stats.st_mode) ? "p"
                                  : S_ISSOCK(stats.st_mode) ? "s"
                                                            : "-";

                // determine name
                char* file_name = colorize(file_type[0] == 'd'   ? ANSI_BLUE
                                           : file_type[0] == 'l' ? ANSI_CYAN
                                                                 : ANSI_WHITE,
                                           entry->d_name);

                // handle -l behaviour
                if (!l_flag) {
                    // determine name
                    col_name_link[entry_iter] = file_name;
                } else {
                    // update total block size
                    total_block_size += stats.st_blocks;

                    // determine permissions
                    char* file_perms = calloc(16, sizeof(char));
                    strcat(file_perms, (stats.st_mode & S_IRUSR) ? "r" : "-");
                    strcat(file_perms, (stats.st_mode & S_IWUSR) ? "w" : "-");
                    strcat(file_perms, (stats.st_mode & S_IXUSR) ? "x" : "-");
                    strcat(file_perms, (stats.st_mode & S_IRGRP) ? "r" : "-");
                    strcat(file_perms, (stats.st_mode & S_IWGRP) ? "w" : "-");
                    strcat(file_perms, (stats.st_mode & S_IXGRP) ? "x" : "-");
                    strcat(file_perms, (stats.st_mode & S_IROTH) ? "r" : "-");
                    strcat(file_perms, (stats.st_mode & S_IWOTH) ? "w" : "-");
                    strcat(file_perms, (stats.st_mode & S_IXOTH) ? "x" : "-");

                    // determine number of hard links
                    char* file_hls = calloc(16, sizeof(char));
                    sprintf(file_hls, "%lu", stats.st_nlink);

                    // determine ownership
                    char* file_owner = getpwuid(stats.st_uid)->pw_name;

                    // determine group
                    char* file_group = getpwuid(stats.st_gid)->pw_name;

                    // determine size
                    char* file_size = calloc(16, sizeof(char));
                    sprintf(file_size, "%ld", stats.st_size);

                    // determine last modified timestamp
                    char* file_modified = calloc(32, sizeof(char));
                    strftime(file_modified, 32, "%b %d %Y %R", localtime((time_t*)&stats.st_mtim));

                    // determine link path if symlink
                    char* file_link = "";
                    if (file_type[0] == 'l') {
                        file_link = calloc(PATH_MAX, sizeof(char));
                        if (readlink(path, file_link, PATH_MAX) == -1) {
                            return throw_custom_error("ls: unable to read symlink pathname", -1);
                        }
                    }

                    // output formatting
                    char* file_type_perms =
                        calloc(strlen(file_type) + strlen(file_perms), sizeof(char));
                    strcat(file_type_perms, file_type);
                    strcat(file_type_perms, file_perms);

                    char* file_name_link = calloc(strlen(file_name) + 4 + PATH_MAX, sizeof(char));
                    strcat(file_name_link, file_name);
                    strcat(file_name_link, file_type[0] == 'l' ? " -> " : "");
                    strcat(file_name_link, file_link);

                    // add stats to output columns
                    col_type_perms[entry_iter] = file_type_perms;
                    col_hls[entry_iter] = file_hls;
                    col_owner[entry_iter] = file_owner;
                    col_group[entry_iter] = file_group;
                    col_size[entry_iter] = file_size;
                    col_modified[entry_iter] = file_modified;
                    col_name_link[entry_iter] = file_name_link;

                    // update maximum lengths of variable columns
                    if (strlen(file_hls) > max_hls_width) max_hls_width = strlen(file_hls);
                    if (strlen(file_owner) > max_owner_width) max_owner_width = strlen(file_owner);
                    if (strlen(file_group) > max_group_width) max_group_width = strlen(file_group);
                    if (strlen(file_size) > max_size_width) max_size_width = strlen(file_size);
                }
            }
            entry_iter++;
        }

        // print total block size if -l set
        if (l_flag) printf("total %d\n", (int)ceil(total_block_size / 2));

        // print list of entries
        for (int i = 0; i < entry_count; i++) {
            if (l_flag) {
                printf("%s %*s %*s %*s %*s %s %s\n", col_type_perms[i], max_hls_width, col_hls[i],
                       max_owner_width, col_owner[i], max_group_width, col_group[i], max_size_width,
                       col_size[i], col_modified[i], col_name_link[i]);
            } else {
                printf("%s\n", col_name_link[i]);
            }
        }

        printf("\n");
    }

    exit(0);
}
