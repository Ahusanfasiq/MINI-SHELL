# MiniShell

A small Unix-style command-line shell written in C as a systems programming and embedded C learning project.

MiniShell can run built-in commands, launch external programs, execute simple pipelines, customize its prompt, handle basic shell signals, and maintain a list of stopped processes for `jobs`, `fg`, and `bg`.

> This project is designed to run on Linux or another Unix-like environment. It uses POSIX process and signal APIs such as `fork()`, `execvp()`, `pipe()`, `waitpid()`, and `kill()`.

---

## Overview

The shell repeatedly reads a command from standard input, identifies whether the command is built in or external, and executes it. External commands are loaded from `ext_command.txt`, so `execvp()` can be used to launch recognized programs and simple pipelines.

This project demonstrates core operating-system concepts including process creation, program replacement, inter-process communication, signal handling, and manual job tracking.

---

## Implemented Features

### Built-in Commands

- `exit` - Exit MiniShell after a short logout message.
- `pwd` - Print the current working directory.
- `cd <directory>` - Change the current working directory and update the prompt.
- `clear` - Clear the terminal screen.
- `echo` - Print text and limited special variables.
- `jobs` - Display stopped processes tracked by MiniShell.
- `fg` - Continue and wait for the first stopped process in the job list.
- `bg` - Continue the first stopped process in the background.

The command recognition table also lists additional shell names such as `printf`, `read`, `export`, `exec`, and `type`, but they are **not custom implementations**. Only the built-in commands listed above are handled by the project.

### External Commands

- Recognizes commands from [`ext_command.txt`](ext_command.txt).
- Executes recognized external programs with `execvp()`.
- Supports the system commands available on the host, such as `ls`, `cat`, `ps`, `grep`, `mkdir`, `ping`, `rm`, and `cp`.

External command support depends on the current contents of `ext_command.txt` and the commands installed on the system.

### Input and Prompt

- Displays a customizable prompt (`minishell$: ` by default).
- Accepts `PS1=<new prompt>` to change it.
- Ignores leading spaces in entered commands.
- Prints an error when a command is not recognized.

### Process, Pipeline, and Signal Handling

- Uses `fork()` to create child processes for external commands.
- Uses `execvp()` to replace a child process with the requested program.
- Uses `waitpid()` to wait for foreground child processes.
- Supports simple pipelines using `|` and POSIX pipes.
- Handles `SIGINT`, `SIGTSTP`, and `SIGQUIT` with a custom signal handler.
- Stores stopped jobs in a singly linked list and supports job continuation.

### Limited `echo` Expansion

MiniShell currently recognizes these forms inside `echo`:

- `echo $$` - Prints the MiniShell process ID followed by the remaining text.
- `echo $?` - Prints the stored child status in exit, signal, or stopped-state form and remaining text.
- `echo $SHELL` - Prints the value of the `SHELL` environment variable and remaining text.
- Other text - Prints the text following `echo`.

---

## Technologies Used

- **Language:** C
- **Platform APIs:** POSIX/Linux
- **Core libraries and system calls:**
  - Standard C library
  - `fork()` and `execvp()`
  - `pipe()` and `dup2()`
  - `wait()` and `waitpid()`
  - `kill()` and POSIX signals
  - `chdir()` and `getcwd()`
- **Data structure:** Singly linked list for stopped job/process tracking
- **Build:** GCC

---

## Project Architecture

| File | Responsibility |
|---|---|
| [`main.c`](main.c) | Program entry point; initializes the prompt and starts the shell loop. |
| [`scan_input.c`](scan_input.c) | Reads input, validates prompt changes, identifies commands, forks external commands, and handles signals. |
| [`command.c`](command.c) | Implements command type detection, built-in commands, external execution, pipelines, echo handling, and job-list operations. |
| [`header.h`](header.h) | Contains includes, constants, function declarations, color macros, and the stopped-process structure. |
| [`ext_command.txt`](ext_command.txt) | List of external commands recognized by the shell. |

### Main Execution Flow

```text
main()
  -> scan_input()
      -> read command
      -> get_command()
      -> check_command_type()
      -> built-in: execute_internal_commands()
      -> external: fork()
          -> child: execute_external_commands()
          -> parent: waitpid()
```

When MiniShell encounters an external command, it creates a child process. The child uses `execvp()` to run the requested program, while the parent waits for the foreground process and stores status information. Pipes divide pipeline input into argument segments before connecting process standard streams.

---

## Requirements

- GCC or another C compiler
- Linux or a Unix-like operating system with POSIX process APIs
- A terminal that supports ANSI color escape sequences

Because this program uses Linux headers and commands such as `clear`, `fork()`, `unistd.h`, and `sys/wait.h`, it is intended for Linux rather than native Windows.

---

## Compilation and Execution

Compile all source files together:

```bash
gcc main.c scan_input.c command.c -o minishell
```

Run the shell:

```bash
./minishell
```

Exit MiniShell:

```text
minishell$: exit
logout
```

### Windows Users

Compile and run MiniShell inside WSL, or use a Linux virtual machine. Native Windows C runtimes do not provide the POSIX headers and functions used by this project. The project also calls the Unix/Linux `clear` command.

---

## Sample Commands

```text
minishell$: pwd
/your/current/directory

minishell$: cd /tmp

minishell$: clear

minishell$: echo hello minishell
hello minishell

minishell$: echo $SHELL
/bin/bash

minishell$: echo $?
0

minishell$: ls
Desktop  Documents  Downloads

minishell$: ps | grep bash
1234 pts/0 S bash

minishell$: PS1=mini$ 
mini$ 
```

The actual output depends on the host operating system and its available files and programs.

---

## Known Limitations

This project is an educational shell implementation and does not yet include every feature of a full shell:

- No quoted-string parsing or variable expansion for arbitrary text.
- No environment-variable assignment commands (`export`, for example).
- No redirection operators such as `>`, `<`, or `>>`.
- No background execution syntax such as trailing `&`.
- No automatic persistent shell history.
- Pipeline handling is limited and is implemented without a full shell parser.
- The external-command list is maintained manually in `ext_command.txt`.
- Command buffers and stored job data have fixed-size limits.

---

## Learning Outcomes

This project helped develop practical understanding of:

- Writing and organizing a C application across multiple source files.
- Parsing and dispatching terminal commands.
- Creating child processes with `fork()`.
- Running external programs with `execvp()`.
- Connecting processes using POSIX pipes and file descriptors.
- Monitoring processes and exit status with `waitpid()`.
- Handling signals such as `SIGINT` and `SIGTSTP`.
- Using linked lists to represent stopped jobs.
- Working with POSIX/Linux system programming APIs.
- Building a Unix-like application directly from a terminal.

---

## Author

**A. Ahusan Fasiq R**

## License

