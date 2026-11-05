#include "header.h"

// storing bild in commands
char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
					"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
					"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", "clear", "fg", "bg", "jobs", NULL};

extern char prompt[];
extern char *external_commands[];
extern int status;
extern List_Process *head;

// command from input_string
char *get_command(char *input_string)
{
	char *command_buf = malloc(10);
	int i = 0;
	while (input_string[i] != ' ' && input_string[i] != '\0')
	{
		command_buf[i] = input_string[i];
		i++;
	}
	command_buf[i] = '\0';
	return command_buf;
}

// find  type is builtin/external command
int check_command_type(char *command)
{
	for (int i = 0; builtins[i]; i++)
	{
		if (strcmp(command, builtins[i]) == 0)
		{
			return BUILTIN;
		}
	}
	for (int i = 0; i < 152; i++)
	{
		// The External commands are stored in 2D-Array-> char *external_cmd[]
		if (strcmp(command, external_commands[i]) == 0)
		{
			return EXTERNAL;
		}
	}
	return NO_COMMAND;
}

// executeing the Builtin commands
void execute_internal_commands(char *input_string)
{
	// To execute exit command
	if (strncmp(input_string, "exit", 4) == 0)
	{
		printf(ANSI_COLOR_GREEN "logout\n" ANSI_COLOR_RESET);
		sleep(1);
		system("clear");
		exit(0);
	}

	// To execute pwd command
	else if (strncmp(input_string, "pwd", 3) == 0)
	{
		char cwd_buffer[50];
		if (getcwd(cwd_buffer, 50) == NULL)
		{
			perror(ANSI_COLOR_RED "pwd" ANSI_COLOR_RESET);
		}
		else
		{
			printf(ANSI_COLOR_GREEN "%s\n" ANSI_COLOR_RESET, cwd_buffer);
		}
	}

	// To execute cd command
	else if (strncmp(input_string, "cd", 2) == 0)
	{
		cd(prompt, input_string);
	}

	// To execute clear command
	else if (strncmp(input_string, "clear", 5) == 0)
	{
		system("clear");
	}

	// To execute echo command
	else if (strncmp(input_string, "echo", 4) == 0)
	{
		echo(input_string, status);
	}

	// To execute jobs command
	else if (strncmp(input_string, "jobs", 4) == 0)
	{
		print_list(head);
	}

	// To execute fg command
	else if (strncmp(input_string, "fg", 2) == 0)
	{
		if (head != NULL)
		{
			printf("%s\n", head->command);
			kill(head->pid, SIGCONT);
			waitpid(head->pid, &status, WUNTRACED);
			delete_first(&head);
		}
		else
		{
			printf("fg: current: no such job\n");
		}
	}

	// To execute bg command
	else if (strncmp(input_string, "bg", 2) == 0)
	{
		signal(SIGCHLD, signal_handler);
		if (head != NULL)
		{
			printf("%s  &\n", head->command);
			kill(head->pid, SIGCONT);
			delete_first(&head);
		}
		else
		{
			puts("bg: current: no such job");
		}
	}
}

//  store external command from file
void extract_external_commands(char **external_cmd)
{
	int extern_cmd_fd = open("ext_command.txt", O_RDONLY);
	if (extern_cmd_fd == -1)
	{
		perror("open");
		return;
	}
	char buffer[25];
	char ch;
	int size = 0, command_count = 0;
	
	while (read(extern_cmd_fd, &ch, 1) != 0)
	{
		buffer[size] = ch;
		size++;
		if (ch == '\n')
		{
			size--;
			buffer[size - 1] = '\0';
			external_cmd[command_count] = malloc(size);
			if (external_cmd[command_count] == NULL)
			{
				perror(ANSI_COLOR_RED "malloc" ANSI_COLOR_RESET);
				return;
			}
			strcpy(external_cmd[command_count], buffer);
			command_count++;
			size = 0;
		}
	}
	close(extern_cmd_fd);
}
// To execute the External commands in Function
void execute_external_commands(char *input_string)
{
	int count_space = 0, count_pipe = 0;

	for (int i = 0; input_string[i]; i++)
	{
		if (input_string[i] == ' ')
		{
			count_space++;
		}
		else if (input_string[i] == '|')
		{
			count_pipe++;
		}
	}
	count_space++;
	char *argv[count_space + 1];
	char word_buffer[25];
	int size = 0, argc = 0;

	for (int i = 0; i <= strlen(input_string); i++)
	{
		word_buffer[size] = input_string[i];
		size++;
		if (input_string[i] == ' ' || input_string[i] == '\0')
		{
			word_buffer[size - 1] = '\0';
			argv[argc] = malloc(size);
			if (argv[argc] == NULL)
			{
				perror(ANSI_COLOR_RED "malloc" ANSI_COLOR_RESET);
				return;
			}
			strcpy(argv[argc], word_buffer);
			size = 0;
			argc++;
		}
	}
	argv[argc] = NULL;
	if (count_pipe == 0)
	{
		execvp(argv[0], argv);
	}
	else
	{
		int arr[count_pipe];
		int i = 0, index = 1;
		arr[0] = 0;
		while (argv[i])
		{
			if (strcmp(argv[i], "|") == 0)
			{
				argv[i] = NULL;
				arr[index] = i + 1;
				index++;
			}
			i++;
		}

		int fd[2];
		for (int i = 0; i < index; i++)
		{
			if (i < (index - 1))
			{
				if (pipe(fd) == -1)
				{
					perror(ANSI_COLOR_RED "pipe" ANSI_COLOR_RESET);
					return;
				}
			}
			// Use fork to create new process
			pid_t pid = fork();

			if (pid > 0)
			{
				if (i < (index - 1))
				{
					dup2(fd[0], 0);
					close(fd[1]);
				}
				wait(NULL);
			}
			else if (pid == 0)
			{
				if (i < (index - 1))
				{
					dup2(fd[1], 1);
					close(fd[0]);
				}
				execvp(argv[arr[i]], argv + arr[i]);
			}
			else
			{
				perror(ANSI_COLOR_RED "fork" ANSI_COLOR_RESET);
				return;
			}
		}
	}
}

// Function for Insert at first in SLL
int insert_first(List_Process **head, char *string, pid_t id)
{
	List_Process *new_node = malloc(sizeof(List_Process));
	if (new_node == NULL)
	{
		perror(ANSI_COLOR_RED "malloc" ANSI_COLOR_RESET);
		return FAILURE;
	}
	strcpy(new_node->command, string);
	new_node->pid = id;
	new_node->link = NULL;
	if (*head == NULL)
	{
		(*head) = new_node;
	}
	else
	{
		new_node->link = (*head);
		(*head) = new_node;
	}
	return SUCCESS;
}
int delete_first(List_Process **head)
{
	// If head is null list is empty
	if (*head == NULL)
	{
		return FAILURE;
	}

	List_Process *old_head = *head;
	(*head) = (*head)->link;
	free(old_head);

	return SUCCESS;
}

// Function to Print list in SLL
void print_list(List_Process *head)
{
	//  If head is null list is empty

	if (head == NULL)
	{
		return;
	}
	int i = 1;
	while (head != NULL)
	{
		printf("[%d]+\t\tStopped\t\t%s\n", i++, head->command);
		head = head->link;
	}
}

// Function  for echo
void echo(char *input_string, int status)
{
	if (strncmp(input_string + 5, "$$", 2) == 0)
	{
		printf("%d%s\n", getpid(), input_string + 7);
	}
	else if (strncmp(input_string + 5, "$?", 2) == 0)
	{
		if (WIFEXITED(status))
		{
			printf("%d", WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status))
		{
			printf("%d", WTERMSIG(status));
		}
		else if (WIFSTOPPED(status))
		{
			printf("%d", WSTOPSIG(status));
		}
		printf("%s\n", input_string + 7);
	}

	else if (strncmp(input_string + 5, "$SHELL", 6) == 0)
	{
		char shell_buffer[25];
		strcpy(shell_buffer, getenv("SHELL"));
		printf("%s%s\n", shell_buffer, input_string + 11);
	}
	else
	{
		puts(input_string + 5);
	}
}

void cd(char *prompt, char *input_string)
{
	char cwd_buffer[50];
	int ret_cd = chdir(input_string + 3);
	if (ret_cd == -1)
	{
		perror(ANSI_COLOR_RED "cd" ANSI_COLOR_RESET);
	}
	else
	{
		getcwd(cwd_buffer, 50);
		strcpy(prompt + 11, cwd_buffer);
		strcat(prompt, "$ ");
	}
}