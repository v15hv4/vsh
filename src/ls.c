#include "ls.h"

#include <dirent.h>
#include <linux/limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
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
        while ((entry = readdir(dir))) {
            // ignore hidden files if -a is not set
            if (!a_flag && entry->d_name[0] == '.') continue;

            // handle -l behaviour
            if (!l_flag) {
                // print only entry names
                printf("%s\n", entry->d_name);
            } else {
                struct stat stats;
                char* path = calloc(strlen(dirs[i]) + strlen(entry->d_name) + 2,
                                    sizeof(char));
                strcat(path, dirs[i]);
                strcat(path, "/");
                strcat(path, entry->d_name);

                if (lstat(path, &stats) == 0) {
                    // determine file type
                    char file_type = S_ISREG(stats.st_mode)    ? '-'
                                     : S_ISLNK(stats.st_mode)  ? 'l'
                                     : S_ISDIR(stats.st_mode)  ? 'd'
                                     : S_ISCHR(stats.st_mode)  ? 'c'
                                     : S_ISBLK(stats.st_mode)  ? 'b'
                                     : S_ISFIFO(stats.st_mode) ? 'p'
                                     : S_ISSOCK(stats.st_mode) ? 's'
                                                               : '-';

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
                    int file_hls = stats.st_nlink;

                    // determine ownership
                    char* file_owner = getpwuid(stats.st_uid)->pw_name;

                    // determine group
                    char* file_group = getpwuid(stats.st_gid)->pw_name;

                    // determine size
                    int file_size = stats.st_size;

                    // determine last modified timestamp
                    char* file_modified = calloc(32, sizeof(char));
                    strftime(file_modified, 32, "%b %d %Y %R",
                             localtime((time_t*)&stats.st_mtim));

                    // determine name
                    char* file_name = colorize(file_type == 'd'   ? ANSI_BLUE
                                               : file_type == 'l' ? ANSI_CYAN
                                                                  : ANSI_WHITE,
                                               entry->d_name);

                    // determine link path if symlink
                    char* file_link = "";
                    if (file_type == 'l') {
                        file_link = calloc(PATH_MAX, sizeof(char));
                        if (readlink(path, file_link, PATH_MAX) == -1) {
                            return throw_custom_error(
                                "ls: unable to read symbolic link pathname",
                                -1);
                        }
                    }

                    // print long listing format
                    printf("%c%s %2d %-6s %-6s %6d %s %s %s %s\n", file_type,
                           file_perms, file_hls, file_owner, file_group,
                           file_size, file_modified, file_name,
                           file_type == 'l' ? "->" : "", file_link);
                }
            }
        }

        printf("\n");
    }

    return 0;
}
