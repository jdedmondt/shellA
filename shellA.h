/* shellA.h: header file for structs and so forth */

#ifndef SHELLA_H_
#define SHELLA_H_

/*** functions ***/

/* utility */
void die(const char *msg, int ret);

/* program execution */
int execute(char **argv);

int shell_cd(char *path);

/* input */
int process_cmdl_args(char **argv);
char *get_input(void);
int process(int argc, char **argv);
char **split(char *input, const char *delim, int *size);

/* misc */
char *extract_dir_name(char *full_path);

/* output */
void print_ps1(void);
void print_usage(void);


/*** structs ***/

struct command {
	int iden;
	const char *name;
	const char *desc;
};

/*** command line arguements ***/

struct command args[] = {
	{0, "--help", "display help message"},
};

/*** internal commands ***/

struct command commands[] = {
	{0, "exit", "terminate the shell"},
	{1, "cd", "change directory"},
};

#endif
