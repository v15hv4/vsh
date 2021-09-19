# vsh
<img src="https://media.discordapp.net/attachments/785528722882560030/889188738993893386/unknown.png" height="50px"/>

## Build
```
make vsh
```

## Usage
Run `./build/vsh` from the source directory.

## Features
vsh is a shell with ~~lots~~ not a lot of features. 

### Multiple Command Execution
Multiple ';' separated commands can be executed in a single line.

### System Command Execution
Execute a file in the system specified by the absolute path or executable
name specified as the first input argument, and pass the remaining
arguments as input to the executable thus launched. If the specified
filename does not contain a slash character, the file is sought in the
colon-separated list of directory pathnames specified in the PATH
environment variable. These files may be executed as a foreground or a
background process depending on the input arguments.

#### Foreground Execution
A system command by default will execute on the foreground. The shell will
wait until the execution of this process is completed before resuming
operation.

#### Background Execution
A system command can be run in the background by appending a `&` to the
input command. The shell will spawn the process in a new, isolated process
group and will not wait for it to complete.  
The Process ID of the new background process will be printed on successful 
spawning, which can be used later to fetch information about the process.  
Once a background process exits, the shell will alert the user about its
termination with the necessary info.

### Keyboard Shortcuts
The `Up` and `Down` arrow keys can be used to cycle through the shell's
command history for easier repeated execution.  
The key combination `Ctrl+D` can be used to exit the shell.  
The key combination `Ctrl+L` can be used to clear the screen.

### Shell Built-ins
A list of commands built-in to the shell whose functions are impossible or
inconvenient to implement by external executables.

#### cd
```
Usage: cd <path_to_directory>
```
Changes the current working directory to the absolute/relative path 
specified by the first input argument. In case of no arguments, it 
defaults to the shell's home.  
Supports the following wildcards:
- `~`: path to shell's home directory
- `-`: path to previous directory
- `.`: path to current directory
- `..`: path to parent directory

#### pwd
```
Usage: pwd
```
Displays the current working directory of the shell.

#### echo
```
Usage: echo <string>
```
Echoes the line of text taken as user input onto `stdout`.  
Does not support any options.  
Does not support multi-line strings, quoted strings, escape sequences, 
or environment variables.

### Custom Commands
A list of custom-implemented or user-defined shell commands.

#### ls
```
Usage: ls <path_to_directory_1> <path_to_directory_2> ...
```
Lists contents of the directories specified by input arguments. In case of
no arguments, it defaults to contents of the current working directory.
Supports the following options:
- `-a`: do not ignore entries starting with .
- `-l`: use a long listing format
Supports all the wildcards as `cd`.

#### pinfo
```
Usage: pinfo <pid>
```
Prints the process-related information of the process specified by the first
input argument. In case of no arguments, it defaults to the current shell's
process.
Gives the following info:
- Process ID
- Process Status
- Virtual Memory
- Path to Executable

#### repeat
```
Usage: repeat <number_of_times> <command>
```
Repeats the command specified by the second input argument sequentially as 
many times as specified by the first input argument.

#### history
```
Usage: history <number_of_entries>
```
Prints the latest number of entries from the shell's command history as
specified by the first input argument. In case of no arguments, prints all
the commands stored in history.  
Stores upto 20 commands, and ignores consecutively repeated commands.

### Additional Commands
A list of additional commands added for convenience of the shell user.

#### exit
```
Usage: exit
```
Exits the shell, and terminates all of its running background processes.

#### clear
```
Usage: clear
```
Clears the terminal screen.

## Assumptions
A few assumptions have been made that impact the performance and output of
the shell as follows:
- The input to the shell may not exceed 2048 characters.
- The username of the current shell user may not exceed 256 characters.
- The hostname of the current shell user may not exceed 256 characters.
- The shell has access to retrieve the host user's username and hostname.
- The shell has permission to read the current working directory in the
  directories it will be run in.
- The maximum length of a path does not exceed 4096 characters.
- The maximum length of a process's `stat` file (under `/proc/<id>/stat`)
  is 1024. This is necessary because the size of files under `/proc` can
  not be determined beforehand.
- 1 month = 30.44 days = 2629743 seconds. This determines whether a file is
  older/newer than 6 months and conditionally displays the year or hours and 
  minutes in long listing format.
