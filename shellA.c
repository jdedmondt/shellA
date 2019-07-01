#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "shellA.h"
#include "config.h"

/* defines */
#define DBG(x) printf("DEBUG: %s\n", x) 
#define DBGI(x) printf("DEBUG INT: %d\n", x)

#define BUF_LEN 1024
#define PATH_LEN 1024

/* die: print optioal error message, exit program with specified exit value */
void die(const char *msg, int ret)
{
	if (msg != NULL) {
		perror(msg);
	}
	exit(ret);
}


/* execute: fork process and execute command with optional arguements */
int execute(char **argv)
{
	pid_t pid;
	int status;
	
	if ((pid = fork()) < 0) {
		die("execute: fork", -1);
	} else if (pid == 0) { /* we are the child process */
		if (execvp(*argv, argv) < 0) {
			die("execute: execvp", -1);
		}
	} else { /* we are the parent process */
		while (wait(&status) != pid) {
			;
		}
	}
	
	return 0;
}

/* shell_cd: change directory with chdir() */
int shell_cd(char *path)
{
	return chdir(path);
}

/* process: process input */
int process(int argc, char **argv) 
{
	/* look for internal commands first */
	size_t end = sizeof(commands)/sizeof(commands[0]);
	for (int i = 0; i < end; i++) {
		if (!strcmp(argv[0], commands[i].name)) {
			switch (i) {
			case 0:
				return 1;
			case 1:
				if (argc == 2) {
					shell_cd(argv[1]);
				} else {
					printf("invalid number of arguments for cd");
				}
				break;
			default:
				break;
			}
			
			return 0;
		}
	}
	
	/* otherwise, just execute argv */
	execute(argv);
	
	
	return 0;
}
/* split: split input by delim into an array of null-terminated strings, return pointer to array */
char **split(char *input, const char *delim, int *size)
{
	char *temp = malloc(sizeof(char)*strlen(input));
	
	char *token;
	char *saveptr;
	char **output = malloc(sizeof(char*));
	size_t i = 0;
	
	strcpy(temp, input);
	while ((token = strtok_r(temp, delim, &saveptr)) != NULL) {
		temp = NULL;
		output = realloc(output, sizeof(char*)*(i+1));
		output[i++] = token;
	}
	output = realloc(output, sizeof(char*)*(i+1));
	output[i] = NULL;
	
	if (size != NULL) 
	{
		*size = i;
	}
	free(temp);
	return output;
}

/* get_input: read a line from the terminal, return pointer to input */
char *get_input(void)
{
	char *input = NULL;
	int nread;
	size_t size = BUF_LEN;
	
	if ((nread = getline(&input, &size, stdin)) != -1) {
		for (int i = strlen(input); input[i] == '\0' || input[i] == '\n';i--) {
			input[i] = '\0';
		}	
		return input;
	} else {
		die("get_input: getline", -1);
	}
}

/* extract_dir_name: given a full directory path, extract the name of the directory */
char *extract_dir_name(char *full_path)
{
	char **split_path = split(full_path, "/", NULL);
	
	int i;
	for (i = 0; split_path[i] != NULL; i++)
		;
	
	char *dir_name = malloc(sizeof(char)*strlen(split_path[i-1]));
	strcpy(dir_name, split_path[i-1]);
	
	free(split_path);
	return dir_name;
	
}	

/* print_ps1: print prompt statement 1 */
void print_ps1(void)
{
	char cwd[PATH_LEN];
	char *dir_name;
	
	if (getcwd(cwd, PATH_LEN) == NULL) {
		die("print_ps1: getcwd", -1);
	}
	dir_name = extract_dir_name(cwd);
	printf(PS1, dir_name);
	
	free(dir_name);
}

/* print_usage: print command line arguments, internal commands */
void print_usage(void)
{
	printf("Command line arguments:\n");
	
	size_t end = sizeof(args)/sizeof(args[0]);
	for (int i = 0; i < end; i++) {
		printf("%s: %s\n", args[i].name, args[i].desc);
	}
	
	printf("\nInternal shell commands:\n");
	end = sizeof(commands)/sizeof(commands[0]);
	for (int i = 0; i < end; i++) {
		printf("%s: %s\n", commands[i].name, commands[i].desc);
	}
}

/* process_cmdl_args: ditto */
int process_cmdl_args(char **argv)
{
	size_t end = sizeof(args)/sizeof(args[0]);
	for (int i = 1; argv[i]; i++)
		for (int j = 0; j < end; j++) {
			if (!strcmp(argv[i], args[j].name) || !strcmp(argv[i], args[j].name_alt)) {
				switch (j) {
				case 0:
					print_usage();
					die(NULL, 0);
				default:
					die("invalid arguement", 0);
				}
			}
	}
}

int main(int argc, char **argv)
{
	/* process command line arguements */
	if (argc > 1) {
		process_cmdl_args(argv);
	}
	
	/* main shell loop */
	char **output;
	char *input;
	int args;
	
	while (1) {
		print_ps1();
		input = get_input();
		output = split(input, " ", &args);
		if (process(args, output)) {
			break;		
		}
	}
	
	free(output);
	free(input);	
	exit(EXIT_SUCCESS);
}
