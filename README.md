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

### Input/Output Redirection
The output of a process can be redirected to files using the symbols '>' (truncate & write) 
and '>>' (append), while input can be redirected to processes using the symbol '<'. 
Both input and output redirection can be used simultaneously.

### Command Pipelines
The output of a foreground process can be redirected into another process as its input using 
the symbol '|' between two commands. Multiple commands can be piped together. Piping and 
redirection can be used in tandem.

### Signal Handling
The key combination `Ctrl+C` can be used to send a `SIGINT`, which is caught by the shell
and directed to the foreground process for it to handle.  
The key combination `Ctrl+Z` can be used to send a `SIGTSTP`, which is caught by the shell
and causes the current foreground process to be suspended (stopped and sent to the background).

#### `cd`
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

#### `pwd`
```
Usage: pwd
```
Displays the current working directory of the shell.

#### `echo`
```
Usage: echo <string>
```
Echoes the line of text taken as user input onto `stdout`.  
Does not support any options.  
Does not support multi-line strings, quoted strings, escape sequences, 
or environment variables.

### Custom Commands
A list of custom-implemented or user-defined shell commands.

#### `ls`
```
Usage: ls [options] <path_to_directory_1> <path_to_directory_2> ...
```
Lists contents of the directories specified by input arguments. In case of
no arguments, it defaults to contents of the current working directory.
Supports the following options:
- `-a`: do not ignore entries starting with .
- `-l`: use a long listing format
Supports all the wildcards as `cd`.

#### `pinfo`
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

#### `repeat`
```
Usage: repeat <number_of_times> <command>
```
Repeats the command specified by the second input argument sequentially as 
many times as specified by the first input argument.

#### `history`
```
Usage: history <number_of_entries>
```
Prints the latest number of entries from the shell's command history as
specified by the first input argument. In case of no arguments, prints all
the commands stored in history.  
Stores upto 20 of the latest commands, and ignores consecutively repeated ones.  
Commands are tracked across all sessions of the shell.

#### `jobs`
```
Usage: jobs [options]
```
Prints a list of all currently running background processes spawned by the shell
in alphabetical order of the command name, along with their job number, process ID,
and their state, which can either be 'Running' or 'Stopped'.
Supports the following options:
- `-r`: prints only running processes
- `-s`: prints only stopped processes

#### `sig`
```
Usage: sig <job_id> <signal_number>
```
Takes the job number of a running job and sends the signal corresponding to the input 
signal number to the process.

#### `fg`
```
Usage: fg <job_id> 
```
Brings the running or stopped background job corresponding to the input job number
to the foreground and changes its state to running.  
Throws an error if no job with the corresponding number exists.

#### `bg`
```
Usage: bg <job_id> 
```
Changes the state of a stopped background job to running (in the background).  
Throws an error if no job with the corresponding number exists.

### Additional Commands
A list of additional commands added for convenience of the shell user.

#### `exit`
```
Usage: exit
```
Exits the shell, and terminates all of its running background processes.

#### `clear`
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
- The shell user has read and write access to `/tmp`. This is necessary
  to manage the history file.
- Only foreground processes will be pipelined.
- Exactly one argument will be provided to the redirection operator that points
  to the input/output file.
