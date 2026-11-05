#ifndef MAIN_H
#define MAIN_H
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
 
#define FAILURE    -1
#define SUCCESS     0
#define BUILTIN		1
#define EXTERNAL	2
#define NO_COMMAND  3
 
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
 
// Single Linked List structure;
typedef struct node
{
    char command[20];
    pid_t pid;
    struct node* link;
}List_Process;
 
void scan_input(char *prompt, char *input_string);
char *get_command(char *input_string);
void cd(char *prompt, char *input_string);
int check_command_type(char *command);
void echo(char *input_string, int status);
void execute_internal_commands(char *input_string);
void execute_external_commands(char *input_string);
void signal_handler(int sig_num);
void extract_external_commands(char **external_commands);
 


int insert_first(List_Process **head,char *string,pid_t id);
int delete_first(List_Process **head);
void print_list(List_Process *head);
 
#endif