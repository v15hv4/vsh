#ifndef PINFO_H_
#define PINFO_H_

// can't get filesize from /proc so explicitly define stat buffer size
#define STAT_SIZE 1024

// execute `pinfo`
int pinfo(int argc, char** argv);

#endif
