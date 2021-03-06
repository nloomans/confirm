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

struct program_opts {
	bool default_answer;
	bool should_print_help;
};

struct program_opts parse_opts(int argc, char **argv) {
	struct program_opts opts = {
		true,  // default_answer
		false, // should_print_help
	};

	char short_opts[] = "+nh";
	struct option long_opts[] = {
		{"no",   no_argument, 0, 'n'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0},
	};

	while (true) {
		int c = getopt_long(argc, argv, short_opts,
		                    long_opts, NULL);

		// Detect end of the opts
		// This will be after the first non-option, because we
		// specified + in short_opts[].
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			opts.should_print_help = true;
			break;
		case 'n':
			opts.default_answer = false;
			break;
		default:
			// TODO: What should we do here?
			abort();
		}
	}

	return opts;
}

bool confirm(int command_argc, char **command_argv, bool default_answer) {
	char *line = NULL;
	char *human_argv = NULL;

	human_argv = join_strings(command_argc, command_argv, " ");

	printf("%s: Are you sure you want to run “%s”? %s ",
		program_invocation_name, human_argv,
		default_answer ? "[Y/n]" : "[y/N]");
	
	bool answer;
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
		if (is_string_all_space(line)) {
			answer = default_answer;
		} else {
			// The user might have mistyped no
			answer = false;
		}
	}

	free(line);
	free(human_argv);
	return answer;
}

int main(int argc, char **argv, char **envp)
{
	struct program_opts opts = parse_opts(argc, argv);

	if (opts.should_print_help || argc < 2) {
		print_help();
		return 0;
	}

	int command_argc = argc - optind;
	char **command_argv = &argv[optind];
	char **command_envp = envp;

	bool is_confirmed = confirm(command_argc, command_argv,
	                            opts.default_answer);

	if (!is_confirmed) {
		abort();
	}

	execvpe(command_argv[0], command_argv, command_envp);
	
	// The current program should have been replaced by the users program.
	// If we are still running, that means something has gone wrong while
	// executing the users program.

	if (errno == ENOENT) {
		// "command not found" is way more user friendly then
		// "No such file or directory"
		fprintf(stderr, "%s: %s: command not found\n",
		        program_invocation_short_name, command_argv[0]);
	} else {
		perror(program_invocation_short_name);
	}

	return errno;
}
