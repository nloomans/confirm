#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <unistd.h>
#include "utils.c"

void print_help()
{
	printf("%s - confirm if you really wanted to execute a command\n", program_invocation_name);
	printf("\n");
	printf("usuage: confirm [-n] command\n");
}

int main(int argc, char **argv, char **envp)
{
	int c;
	bool default_answer = true;

	static char short_options[] = "+nh";
	static struct option long_options[] = {
		{"no", no_argument, 0, 'n'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0},
	};

	while (true) {
		int option_index = 0;

		c = getopt_long (argc, argv, short_options,
                       long_options, &option_index);

		// Detect end of the options
		// This will be after the first non-option, because we
		// specified + in short_options[].
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			print_help();
			return 0;
		case 'n':
			default_answer = false;
			break;
		default:
			abort();
		}
	}

	if (argc <= 1) {
		print_help();
	} else {
		int command_argc = argc - optind;
		char **command_argv = &argv[optind];
		char **command_envp = envp;

		char *human_argv = join_strings(command_argv, " ", command_argc);
		printf("%s: Are you sure you want to run “%s”? %s ",
		       program_invocation_name, human_argv,
		       default_answer ? "[Y/n]" : "[y/N]");
		free(human_argv);

		bool answer;
		char* line = NULL;
		size_t len = 0;

		getline(&line, &len, stdin);
		switch (rpmatch(line)) {
		case 1:
			answer = true;
			break;
		case 0:
			answer = false;
			break;
		default:
			answer = default_answer;
		}
		free(line);

		if (answer) {
			execvpe(command_argv[0], command_argv, command_envp);
		} else {
			abort();
		}
	}

	printf("errno: %d\n", errno);
	return errno;
}
