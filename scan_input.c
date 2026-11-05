#include "header.h"
extern char prompt[];
extern char input_string[];
 
char *external_commands[153];
int status;
List_Process *head=NULL;
pid_t process_id;
 
// Signal handler SIGINT, SIGTSTP, SIGQUIT
void signal_handler(int signum)
{
    // SIGINT 
    if(signum==SIGINT)
    {
        if(process_id==0)
        {
            printf(ANSI_COLOR_GREEN"\n%s"ANSI_COLOR_RESET,prompt);
            fflush(stdout);
        }
        else
        {
            printf("\n");
        }
    }
    // SIGTSTP
    else if(signum==SIGTSTP)
    {
        if(process_id==0)
        {
            printf(ANSI_COLOR_GREEN"\n%s"ANSI_COLOR_RESET,prompt);
            fflush(stdout);
        }
        else
        {
            insert_first(&head,input_string,process_id);
            printf("\n");
            printf("Stopped\t\t%s\n",head->command);
        }
    }
   
    else if(signum==SIGCHLD)
    {
        if(process_id!=0)
        {
            waitpid(-1,&status,WNOHANG);
        }
    }
}
 
// minishell program
void scan_input(char *prompt, char *input_string)
{
    extract_external_commands(external_commands);
 
    // Registering the Signal 
    signal(SIGINT,signal_handler);
    signal(SIGTSTP,signal_handler);
    signal(SIGQUIT,signal_handler);
 
    while(1)
    {
        printf(ANSI_COLOR_GREEN"%s"ANSI_COLOR_RESET,prompt);
 
        fgets(input_string, 50, stdin);
        input_string[strcspn(input_string, "\n")] = '\0';
 
        int i=0;
        while(input_string[i])
        {
            // If space is not found exit the loop
            if(input_string[i]!=' ')
            {
                break;
            }
            else
            {
                int j=i;
 
                // Left shift the character one by one
                while(input_string[j])
                {
                    input_string[j]=input_string[j+1];
                    j++;
                }
            }
        }
 
        //Validating for PS1 
        if(strncmp("PS1=",input_string,4)==0)
        {
            int space=0,flag=0,i=4;
            while(input_string[i])
            {
                if(input_string[i]==' ')
                {
                    space=1;
                }
                else if(space)
                {
                    flag=1;
                    char *token=strtok(input_string," ");
                    token = strtok(NULL, " ");
                    printf(ANSI_COLOR_RED"Command \"%s\" not found:\n"ANSI_COLOR_RESET,token);
                    break;
                }
                i++;
            }
            if(flag==0)
            {
                int i=strlen(input_string)-1;
                while(i>=0 && input_string[i]==' ')
                {
                    input_string[i]='\0';
                    i--;
                }
                strcpy(prompt,input_string+4);
            }
            continue;
        }
 
        // Copy the command from input_string and store in to command;
        char *cmd_name=get_command(input_string);
        
        // To finding the  command type is bultin/external command
        int cmd_type=check_command_type(cmd_name);
 
        // Execute buitlin commands
        if(cmd_type==BUILTIN)
        {
            execute_internal_commands(input_string);
        }
 
        // Execute external commands
        else if(cmd_type==EXTERNAL)
        {
            
            int temp_fd[2];
            
            if(pipe(temp_fd)==-1)
            {
                perror(ANSI_COLOR_RED"pipe"ANSI_COLOR_RESET);
                return;
            }
 
            process_id=fork();
 
            if(process_id >0)
            {
                waitpid(process_id,&status,WUNTRACED);
            }
            
            else if(process_id==0)
            {
                signal(SIGINT,SIG_DFL);
                signal(SIGTSTP,SIG_DFL);
                signal(SIGQUIT,SIG_DFL);   
                execute_external_commands(input_string);
                return;
            }
            else
            {
                perror(ANSI_COLOR_RED"fork"ANSI_COLOR_RESET);
                return;
            }
        }
        else if(cmd_type==NO_COMMAND)
        {
                
            if(input_string[0]!='\0')
                // Print the error message
                printf(ANSI_COLOR_RED"%s: command not found\n"ANSI_COLOR_RESET,input_string);
        }
    }
}